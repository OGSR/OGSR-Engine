#include "stdafx.h"

#include <concurrentqueue/concurrentqueue.h>

#include <sqfs/compressor.h>
#include <sqfs/data_reader.h>
#include <sqfs/dir_entry.h>
#include <sqfs/dir_reader.h>
#include <sqfs/id_table.h>
#include <sqfs/inode.h>
#include <sqfs/io.h>

#include "FS_internal.h"
#include "stream_reader.h"

#pragma comment(lib, "libsquashfs")
#pragma comment(lib, "libzstd")
#pragma comment(lib, "liblz4")

struct CLocatorAPI::archive::xr_sqfs
{
    struct reader
    {
        sqfs_file_t* file;
        sqfs_dir_reader_t* dr;
        sqfs_data_reader_t* data;
    };

private:
    moodycamel::ConcurrentQueue<reader*> q;

    reader* alloc(const archive& arc);

    SpinLock alloc_lock;

public:
    sqfs_super_t super{};

    reader* get(const archive& arc)
    {
        reader* rd;

        if (!q.try_dequeue(rd))
        {
            alloc_lock.lock();
            rd = alloc(arc);
            alloc_lock.unlock();
        }

        return rd;
    }

    bool put(reader* rd) { return q.enqueue(rd); }

    void drain();
};

CLocatorAPI::archive::xr_sqfs::reader* CLocatorAPI::archive::xr_sqfs::alloc(const archive& arc)
{
    reader* rd = xr_new<reader>();

    R_ASSERT(!sqfs_file_open(&rd->file, arc.path.c_str(), SQFS_FILE_OPEN_READ_ONLY | SQFS_FILE_OPEN_NO_CHARSET_XFRM));

    rd->dr = sqfs_dir_reader_create(&arc.fs->super, arc.cmp, rd->file, 0);
    rd->data = sqfs_data_reader_create(rd->file, arc.fs->super.block_size, arc.cmp, 0);

    R_ASSERT(rd->dr && rd->data);
    R_ASSERT(!sqfs_data_reader_load_fragment_table(rd->data, &arc.fs->super));

    return rd;
}

void CLocatorAPI::archive::xr_sqfs::drain()
{
    reader* rd;

    while (q.try_dequeue(rd))
    {
        sqfs_drop(rd->data);
        sqfs_drop(rd->dr);
        sqfs_drop(rd->file);

        xr_delete(rd);
    }
}

/* Stream reader for a file inside squashfs */

class CLocatorAPI::archive::xr_sqfs_stream : public CStreamReader
{
private:
    const archive* arc{};
    xr_sqfs::reader* rd{};
    sqfs_inode_generic_t* inode{};

    size_t fbase{};
    size_t fsize{};
    size_t foff{};

    size_t bsize{};
    size_t bread{};
    size_t boff{};

    u8* buf{};
    u64 ref{};
    size_t wnd{};

    void destroy()
    {
        if (!arc)
            return;

        xr_free(buf);
        sqfs_free(inode);
        arc->fs->put(rd);

        memset(reinterpret_cast<u8*>(this) + offsetof(xr_sqfs_stream, arc), 0, sizeof(*this) - offsetof(xr_sqfs_stream, arc));
    }

public:
    void construct(const archive* arcin, u64 inodein, size_t base, size_t sz, size_t wndin);
    void construct(const archive* arcin, u64 inodein, size_t sz, size_t wndin) { construct(arcin, inodein, 0, sz, wndin); }

    size_t elapsed() const override { return fsize - tell(); }
    const size_t& length() const override { return fsize; }
    size_t tell() const override { return foff + boff; }
    void seek(const int& offset) override { advance(offset - tell()); }

    void close() override
    {
        destroy();

        xr_sqfs_stream* self = this;
        xr_delete(self);
    }

    void advance(const int& offset) override
    {
        s64 noff = s64(boff) + offset;
        if (noff >= 0 && size_t(noff) < bsize)
        {
            boff = noff;
            s64 nread = s64(bread) - offset;
            bread = std::max<s64>(nread, 0);

            if (offset < 0)
                R_ASSERT(sqfs_data_reader_read(rd->data, inode, fbase + foff + boff, buf + boff, -offset) == -offset);
        }
        else
        {
            foff += noff;

            size_t woff = rounddown(foff, wnd);
            boff = foff - woff;
            foff = woff;

            bread = 0;
        }
    }

    void r(void* buffer, size_t buffer_size) override;
    CStreamReader* open_chunk(const u32& chunk_id) override;
};

void CLocatorAPI::archive::xr_sqfs_stream::construct(const CLocatorAPI::archive* arcin, u64 inodein, size_t base, size_t sz, size_t wndin)
{
    if (arc)
        destroy();

    arc = arcin;
    fbase = base;
    fsize = sz;

    ref = inodein;
    wnd = wndin;
    R_ASSERT(std::has_single_bit(wnd));

    rd = arc->fs->get(*arc);
    R_ASSERT(!sqfs_dir_reader_get_inode(rd->dr, ref, &inode));

    bsize = std::min(wnd, fsize);
    buf = xr_alloc<u8>(bsize);
}

void CLocatorAPI::archive::xr_sqfs_stream::r(void* buffer, size_t buffer_size)
{
    u8* cbuffer = reinterpret_cast<u8*>(buffer);

    while (buffer_size && !eof())
    {
        if (!bread)
        {
            size_t precache = std::min<s64>(bsize - boff, elapsed());

            R_ASSERT(sqfs_data_reader_read(rd->data, inode, fbase + foff + boff, buf + boff, precache) == (sqfs_s32)precache);
            bread += precache;
        }

        size_t copy = std::min(buffer_size, bread);
        memcpy(cbuffer, buf + boff, copy);

        cbuffer += copy;
        buffer_size -= copy;

        int off = (int)copy;
        advance(off);
    }
}

CStreamReader* CLocatorAPI::archive::xr_sqfs_stream::open_chunk(const u32& chunk_id)
{
    BOOL compressed;

    const auto size = find_chunk(chunk_id, &compressed);
    if (!size)
        return nullptr;

    R_ASSERT(!compressed, "cannot use xr_sqfs_stream on compressed chunks");

    xr_sqfs_stream* st = xr_new<xr_sqfs_stream>();
    st->construct(arc, ref, tell(), size, wnd);

    return st;
}

/* SquashFS */

void CLocatorAPI::archive::open_sqfs()
{
    type = container::SQFS;
    fs = xr_new<xr_sqfs>();

    R_ASSERT(!sqfs_file_open(&file, path.c_str(), SQFS_FILE_OPEN_READ_ONLY | SQFS_FILE_OPEN_NO_CHARSET_XFRM));
    R_ASSERT(!sqfs_super_read(&fs->super, file));

    sqfs_compressor_config_t cfg;
    sqfs_compressor_config_init(&cfg, (SQFS_COMPRESSOR)fs->super.compression_id, fs->super.block_size, SQFS_COMP_FLAG_UNCOMPRESS);

    R_ASSERT(!sqfs_compressor_create(&cfg, &cmp));
}

void CLocatorAPI::archive::index_dir_sqfs(CLocatorAPI& loc, const char* path, sqfs_dir_iterator_t* it)
{
    sqfs_dir_entry_t* ent;

    for (;;)
    {
        if (it->next(it, &ent))
            break;

        string256 full;
        strconcat(sizeof(full), full, path, ent->name);

        if ((ent->mode & SQFS_INODE_MODE_MASK) == SQFS_INODE_MODE_DIR)
        {
            strconcat(sizeof(full), full, full, "\\");

            sqfs_dir_iterator_t* sub;

            it->open_subdir(it, &sub);
            index_dir_sqfs(loc, full, sub);
            sqfs_drop(sub);
        }
        else
        {
            union
            {
                u64 inode;
                struct
                {
                    u32 ptr;
                    u32 size_compressed;
                };
            } attr = {
                .inode = ent->inode,
            };

            loc.Register(full, vfs_idx, attr.ptr, ent->size, attr.size_compressed, 0);
        }

        sqfs_free(ent);
    }
}

void CLocatorAPI::archive::index_sqfs(CLocatorAPI& loc, const char* fs_entry_point)
{
    sqfs_inode_generic_t* inode;
    sqfs_dir_iterator_t* it;
    sqfs_id_table_t* idt;

    idt = sqfs_id_table_create(0);
    R_ASSERT(idt);
    R_ASSERT(!sqfs_id_table_read(idt, file, &fs->super, cmp));

    auto* rd = fs->get(*this);
    R_ASSERT(!sqfs_dir_reader_get_root_inode(rd->dr, &inode));

    R_ASSERT(!sqfs_dir_iterator_create(rd->dr, idt, rd->data, nullptr, inode, &it));
    sqfs_free(inode);

    index_dir_sqfs(loc, fs_entry_point, it);
    sqfs_drop(it);

    sqfs_drop(idt);
    fs->put(rd);
}

IReader* CLocatorAPI::archive::read_sqfs(const char* fname, const struct file& desc, u32 gran)
{
    sqfs_inode_generic_t* inode;
    auto* rd = fs->get(*this);

    if (sqfs_dir_reader_get_inode(rd->dr, desc.inode, &inode))
        return nullptr;

    u8* dest = xr_alloc<u8>(desc.size_real);
    R_ASSERT(sqfs_data_reader_read(rd->data, inode, 0, dest, desc.size_real) == (s32)desc.size_real);

    sqfs_free(inode);
    fs->put(rd);

    return xr_new<CTempReader>(dest, desc.size_real, 0);
}

CStreamReader* CLocatorAPI::archive::stream_sqfs(const char* fname, const struct file& desc)
{
    xr_sqfs_stream* st = xr_new<xr_sqfs_stream>();
    st->construct(this, desc.inode, desc.size_real, fs->super.block_size);
    return st;
}

void CLocatorAPI::archive::cleanup_sqfs()
{
    // cleanup() is inverse autoload(), which does nothing
}

void CLocatorAPI::archive::close_sqfs()
{
    fs->drain();

    sqfs_drop(cmp);
    cmp = nullptr;

    xr_delete(fs);
    fs = nullptr;

    sqfs_drop(file);
    file = nullptr;
}
