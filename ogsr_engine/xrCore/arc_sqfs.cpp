#include "stdafx.h"

#include "FS_internal.h"
#include "stream_reader.h"

#include <../cpputils/cpputils/include/bitmap_object_pool.hpp>

namespace sqfs
{
#include <sqfs/compressor.h>
#include <sqfs/data_reader.h>
#include <sqfs/dir_entry.h>
#include <sqfs/dir_reader.h>
#include <sqfs/id_table.h>
#include <sqfs/inode.h>
#include <sqfs/io.h>
} // namespace sqfs

#pragma comment(lib, "libsquashfs")
#pragma comment(lib, "libzstd")
#pragma comment(lib, "liblz4")

template <>
struct std::default_delete<sqfs::sqfs_compressor_t>
{
    _CONSTEXPR23 void operator()(sqfs::sqfs_compressor_t* ptr) const noexcept { sqfs::sqfs_drop(ptr); }
};

template <>
struct std::default_delete<sqfs::sqfs_data_reader_t>
{
    _CONSTEXPR23 void operator()(sqfs::sqfs_data_reader_t* ptr) const noexcept { sqfs::sqfs_drop(ptr); }
};

template <>
struct std::default_delete<sqfs::sqfs_dir_entry_t>
{
    _CONSTEXPR23 void operator()(sqfs::sqfs_dir_entry_t* ptr) const noexcept { sqfs::sqfs_free(ptr); }
};

template <>
struct std::default_delete<sqfs::sqfs_dir_iterator_t>
{
    _CONSTEXPR23 void operator()(sqfs::sqfs_dir_iterator_t* ptr) const noexcept { sqfs::sqfs_drop(ptr); }
};

template <>
struct std::default_delete<sqfs::sqfs_dir_reader_t>
{
    _CONSTEXPR23 void operator()(sqfs::sqfs_dir_reader_t* ptr) const noexcept { sqfs::sqfs_drop(ptr); }
};

template <>
struct std::default_delete<sqfs::sqfs_file_t>
{
    _CONSTEXPR23 void operator()(sqfs::sqfs_file_t* ptr) const noexcept { sqfs::sqfs_drop(ptr); }
};

template <>
struct std::default_delete<sqfs::sqfs_id_table_t>
{
    _CONSTEXPR23 void operator()(sqfs::sqfs_id_table_t* ptr) const noexcept { sqfs::sqfs_drop(ptr); }
};

template <>
struct std::default_delete<sqfs::sqfs_inode_generic_t>
{
    _CONSTEXPR23 void operator()(sqfs::sqfs_inode_generic_t* ptr) const noexcept { sqfs::sqfs_free(ptr); }
};

class CLocatorAPI::archive::xr_sqfs
{
public:
    class reader
    {
    public:
        std::unique_ptr<sqfs::sqfs_file_t> file;
        std::unique_ptr<sqfs::sqfs_compressor_t> cmp;
        std::unique_ptr<sqfs::sqfs_dir_reader_t> dr;
        std::unique_ptr<sqfs::sqfs_data_reader_t> data;

        reader() = delete;
        explicit reader(const archive& arc);
    };

private:
    class pool_t final : public tzcnt_utils::BitmapObjectPoolImpl<reader, pool_t>
    {
        friend class tzcnt_utils::BitmapObjectPoolImpl<reader, pool_t>;

        void initialize(void* location, const archive& arc) { new (location) reader{arc}; }
    };

    pool_t pool;

public:
    sqfs::sqfs_super_t super;
    sqfs::sqfs_compressor_config_t cfg;

    xr_sqfs() = delete;
    explicit xr_sqfs(const char* path);

    [[nodiscard]] auto acquire_scoped(const archive& arc) { return pool.acquire_scoped(arc); }
};

CLocatorAPI::archive::xr_sqfs::xr_sqfs(const char* path)
{
    sqfs::sqfs_file_t* f;
    R_ASSERT(sqfs::sqfs_file_open(&f, path, sqfs::SQFS_FILE_OPEN_READ_ONLY | sqfs::SQFS_FILE_OPEN_NO_CHARSET_XFRM) == 0);
    const auto file = xr::wrap_unique(f);

    R_ASSERT(sqfs::sqfs_super_read(&super, file.get()) == 0);
    R_ASSERT(sqfs::sqfs_compressor_config_init(&cfg, static_cast<sqfs::SQFS_COMPRESSOR>(super.compression_id), super.block_size, sqfs::SQFS_COMP_FLAG_UNCOMPRESS) == 0);
}

CLocatorAPI::archive::xr_sqfs::reader::reader(const archive& arc)
{
    sqfs::sqfs_file_t* f;
    R_ASSERT(sqfs::sqfs_file_open(&f, arc.path.c_str(), sqfs::SQFS_FILE_OPEN_READ_ONLY | sqfs::SQFS_FILE_OPEN_NO_CHARSET_XFRM) == 0);
    file = xr::wrap_unique(f);

    sqfs::sqfs_compressor_t* c;
    R_ASSERT(sqfs::sqfs_compressor_create(&arc.fs->cfg, &c) == 0);
    cmp = xr::wrap_unique(c);

    dr = xr::wrap_unique(sqfs::sqfs_dir_reader_create(&arc.fs->super, cmp.get(), file.get(), 0));
    data = xr::wrap_unique(sqfs::sqfs_data_reader_create(file.get(), arc.fs->super.block_size, cmp.get(), 0));
    R_ASSERT(dr && data);

    R_ASSERT(sqfs::sqfs_data_reader_load_fragment_table(data.get(), &arc.fs->super) == 0);
}

/* Stream reader for a file inside squashfs */

class CLocatorAPI::archive::xr_sqfs_stream : public CStreamReader
{
    const decltype(std::declval<xr_sqfs>().acquire_scoped(std::declval<archive>())) obj;
    std::unique_ptr<sqfs::sqfs_inode_generic_t> inode;

    xr_vector<std::byte> buf;
    std::ptrdiff_t wnd;

    std::ptrdiff_t fbase;
    std::ptrdiff_t fsize;
    std::ptrdiff_t foff{};

    std::ptrdiff_t bsize;
    std::ptrdiff_t bread{};
    std::ptrdiff_t boff{};

    const archive& arc;
    sqfs::sqfs_u64 ref;

    [[nodiscard]] const auto& rd() const { return obj.value; }

public:
    xr_sqfs_stream() = delete;
    explicit xr_sqfs_stream(const archive& arcin, sqfs::sqfs_u64 inodein, std::ptrdiff_t base, std::ptrdiff_t sz, std::ptrdiff_t wndin);
    ~xr_sqfs_stream() override = default;

    void close() override;

    [[nodiscard]] size_t elapsed() const override { return fsize - tell(); }
    [[nodiscard]] size_t length() const override { return fsize; }
    [[nodiscard]] size_t tell() const override { return foff + boff; }
    void seek(std::ptrdiff_t offset) override { advance(offset - tell()); }

    void advance(std::ptrdiff_t offset) override;
    void r(void* buffer, size_t buffer_size) override;
    [[nodiscard]] CStreamReader* open_chunk(u32 chunk_id) override;
};

CLocatorAPI::archive::xr_sqfs_stream::xr_sqfs_stream(const archive& arcin, sqfs::sqfs_u64 inodein, std::ptrdiff_t base, std::ptrdiff_t sz, std::ptrdiff_t wndin)
    : obj{arcin.fs->acquire_scoped(arcin)}, wnd{wndin}, fbase{base}, fsize{sz}, arc{arcin}, ref{inodein}
{
    R_ASSERT(std::has_single_bit(static_cast<size_t>(wnd)));

    sqfs::sqfs_inode_generic_t* in;
    R_ASSERT(sqfs::sqfs_dir_reader_get_inode(rd().dr.get(), ref, &in) == 0);
    inode = xr::wrap_unique(in);

    bsize = std::min(wnd, fsize);
    buf.resize(static_cast<size_t>(bsize));
}

void CLocatorAPI::archive::xr_sqfs_stream::close()
{
    auto self = this;
    xr_delete(self);
}

void CLocatorAPI::archive::xr_sqfs_stream::advance(std::ptrdiff_t offset)
{
    const auto noff = boff + offset;
    if (noff >= 0 && noff < bsize)
    {
        boff = noff;
        bread = std::max(bread - offset, 0ll);

        if (offset < 0)
            R_ASSERT(sqfs::sqfs_data_reader_read(rd().data.get(), inode.get(), static_cast<sqfs::sqfs_u64>(fbase + foff + boff), &buf[static_cast<size_t>(boff)],
                                                 static_cast<sqfs::sqfs_u32>(-offset)) == -offset);
    }
    else
    {
        foff += noff;

        const auto woff = xr::rounddown(foff, wnd);
        boff = foff - woff;
        foff = woff;

        bread = 0;
    }
}

void CLocatorAPI::archive::xr_sqfs_stream::r(void* buffer, size_t buffer_size)
{
    std::byte* cbuffer = static_cast<std::byte*>(buffer);

    while (buffer_size > 0 && !eof())
    {
        if (bread == 0)
        {
            const auto precache = std::min(bsize - boff, static_cast<std::ptrdiff_t>(elapsed()));

            R_ASSERT(sqfs::sqfs_data_reader_read(rd().data.get(), inode.get(), static_cast<sqfs::sqfs_u64>(fbase + foff + boff), &buf[static_cast<size_t>(boff)],
                                                 static_cast<sqfs::sqfs_u32>(precache)) == precache);
            bread += precache;
        }

        const auto copy = std::min(static_cast<std::ptrdiff_t>(buffer_size), bread);
        std::memcpy(cbuffer, &buf[static_cast<size_t>(boff)], static_cast<size_t>(copy));

        cbuffer += copy;

        R_ASSERT(copy <= static_cast<std::ptrdiff_t>(buffer_size));
        buffer_size -= copy;
        advance(copy);
    }
}

CStreamReader* CLocatorAPI::archive::xr_sqfs_stream::open_chunk(u32 chunk_id)
{
    BOOL compressed;

    const auto size = find_chunk(chunk_id, &compressed);
    if (size == 0)
        return nullptr;

    R_ASSERT2(!compressed, "cannot use xr_sqfs_stream on compressed chunks");

    return xr_new<xr_sqfs_stream>(arc, ref, tell(), size, wnd);
}

void CLocatorAPI::archive::open_sqfs()
{
    type = container::SQFS;
    fs = xr_new<xr_sqfs>(path.c_str());
}

void CLocatorAPI::archive::index_dir_sqfs(CLocatorAPI& loc, const char* path, sqfs::sqfs_dir_iterator_t& it) const
{
    for (;;)
    {
        sqfs::sqfs_dir_entry_t* ep;
        if (it.next(&it, &ep) != 0)
            break;

        const auto ent = xr::wrap_unique(ep);
        string_path full;
        xr_strconcat(full, path, ent->name);

        if ((ent->mode & sqfs::SQFS_INODE_MODE_MASK) == sqfs::SQFS_INODE_MODE_DIR)
        {
            xr_strcat(full, "\\");

            sqfs::sqfs_dir_iterator_t* subp;
            R_ASSERT(it.open_subdir(&it, &subp) == 0);

            index_dir_sqfs(loc, full, *xr::wrap_unique(subp));
        }
        else
        {
            union
            {
                sqfs::sqfs_u64 inode;
                struct
                {
                    u32 ptr;
                    u32 size_compressed;
                };
            } attr = {
                .inode = ent->inode,
            };

            loc.Register(full, vfs_idx, attr.ptr, static_cast<s64>(ent->size), attr.size_compressed, ent->mtime);
        }
    }
}

void CLocatorAPI::archive::index_sqfs(CLocatorAPI& loc, const char* fs_entry_point) const
{
    const auto obj = fs->acquire_scoped(*this);
    const auto& rd = obj.value;

    const auto idt = xr::wrap_unique(sqfs::sqfs_id_table_create(0));
    R_ASSERT(idt && sqfs::sqfs_id_table_read(idt.get(), rd.file.get(), &fs->super, rd.cmp.get()) == 0);

    const auto it = [&idt, &rd] {
        sqfs::sqfs_inode_generic_t* inp;
        R_ASSERT(sqfs::sqfs_dir_reader_get_root_inode(rd.dr.get(), &inp) == 0);
        const auto inode = xr::wrap_unique(inp);

        sqfs::sqfs_dir_iterator_t* itp;
        R_ASSERT(sqfs::sqfs_dir_iterator_create(rd.dr.get(), idt.get(), rd.data.get(), nullptr, inode.get(), &itp) == 0);
        return xr::wrap_unique(itp);
    };

    index_dir_sqfs(loc, fs_entry_point, *it());
}

IReader* CLocatorAPI::archive::read_sqfs(const char*, const struct file& desc, u32) const
{
    const auto obj = fs->acquire_scoped(*this);
    const auto& rd = obj.value;

    sqfs::sqfs_inode_generic_t* inp;
    if (sqfs::sqfs_dir_reader_get_inode(rd.dr.get(), desc.inode, &inp) != 0)
        return nullptr;

    const auto inode = xr::wrap_unique(inp);
    std::byte* dest = xr_alloc<std::byte>(desc.size_real);
    R_ASSERT(sqfs::sqfs_data_reader_read(rd.data.get(), inode.get(), 0, dest, static_cast<sqfs::sqfs_u32>(desc.size_real)) == desc.size_real);

    return xr_new<CTempReader>(dest, desc.size_real, 0);
}

CStreamReader* CLocatorAPI::archive::stream_sqfs(const char*, const struct file& desc) const
{
    return xr_new<xr_sqfs_stream>(*this, desc.inode, 0, desc.size_real, fs->super.block_size);
}

void CLocatorAPI::archive::cleanup_sqfs()
{
    // cleanup() is inverse autoload(), which does nothing
}

void CLocatorAPI::archive::close_sqfs() { xr_delete(fs); }
