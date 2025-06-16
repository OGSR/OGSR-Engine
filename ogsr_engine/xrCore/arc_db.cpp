#include "stdafx.h"

#include "FS_internal.h"
#include "stream_reader.h"
#include "trivial_encryptor.h"

static IReader* open_archive_chunk(void* ptr, u32 ID, const char* archiveName, size_t archiveSize)
{
    const bool shouldDecrypt = !strstr(archiveName, ".xdb");

    u32 dwType = INVALID_SET_FILE_POINTER;
    DWORD read_byte;
    u32 dwPtr = SetFilePointer(ptr, 0, nullptr, FILE_BEGIN);
    R_ASSERT(dwPtr != INVALID_SET_FILE_POINTER, archiveName, Debug.error2string(GetLastError()));
    while (true)
    {
        bool res = ReadFile(ptr, &dwType, 4, &read_byte, nullptr);
        R_ASSERT(res && read_byte == 4, archiveName, Debug.error2string(GetLastError()));

        u32 tempSize = 0;
        res = ReadFile(ptr, &tempSize, 4, &read_byte, nullptr);
        size_t dwSize = tempSize;
        R_ASSERT(res && read_byte == 4, archiveName, Debug.error2string(GetLastError()));

        if ((dwType & ~CFS_CompressMark) == ID)
        {
            u8* src_data = xr_alloc<u8>(dwSize);
            res = ReadFile(ptr, src_data, dwSize, &read_byte, nullptr);
            R_ASSERT(res && read_byte == dwSize, archiveName, Debug.error2string(GetLastError()));
            if (dwType & CFS_CompressMark)
            {
                BYTE* dest{};
                size_t dest_sz{};

                if (shouldDecrypt)
                    g_trivial_encryptor.decode(src_data, dwSize, src_data); // Try WW key first

                bool result = _decompressLZ(&dest, &dest_sz, src_data, dwSize, archiveSize);

                if (!result && shouldDecrypt) // Let's try to decode with Rus key
                {
                    MsgDbg("[%s]: decoding of %s with WW key failed, trying RU key...", __FUNCTION__, archiveName);
                    g_trivial_encryptor.encode(src_data, dwSize, src_data); // rollback
                    g_trivial_encryptor.decode(src_data, dwSize, src_data, trivial_encryptor::key_flag::russian);
                    result = _decompressLZ(&dest, &dest_sz, src_data, dwSize, archiveSize);
                }

                CHECK_OR_EXIT(result, make_string("[%s] Can't decompress archive [%s]", __FUNCTION__, archiveName));

                xr_free(src_data);
                return xr_new<CTempReader>(dest, dest_sz, 0);
            }
            return xr_new<CTempReader>(src_data, dwSize, 0);
        }

        dwPtr = SetFilePointer(ptr, dwSize, nullptr, FILE_CURRENT);
        R_ASSERT(dwPtr != INVALID_SET_FILE_POINTER, archiveName, Debug.error2string(GetLastError()));
    }
};

void CLocatorAPI::archive::open_db()
{
    type = container::DB;

    hSrcMap = CreateFileMapping(hSrcFile, nullptr, PAGE_READONLY, 0, 0, nullptr);
    R_ASSERT(hSrcMap != INVALID_HANDLE_VALUE);
}

void CLocatorAPI::archive::index_db(CLocatorAPI& loc, const char* fs_entry_point) const
{
    IReader* hdr = open_archive_chunk(hSrcFile, 1, path.c_str(), size);
    R_ASSERT(hdr);
    RStringVec fv;
    while (!hdr->eof())
    {
        string_path name, full;
        string1024 buffer_start;
        size_t buffer_size = hdr->r_u16();
        VERIFY(buffer_size < sizeof(name) + 4 * sizeof(u32));
        VERIFY(buffer_size < sizeof(buffer_start));
        u8* buffer = (u8*)&*buffer_start;
        hdr->r(buffer, buffer_size);

        u32 size_real = *(u32*)buffer;
        buffer += sizeof(size_real);

        u32 size_compr = *(u32*)buffer;
        buffer += sizeof(size_compr);

        // Skip unused checksum
        buffer += sizeof(u32);

        size_t name_length = buffer_size - 4 * sizeof(u32);
        Memory.mem_copy(name, buffer, name_length);
        name[name_length] = 0;
        buffer += buffer_size - 4 * sizeof(u32);

        u32 ptr = *(u32*)buffer;
        buffer += sizeof(ptr);

        strconcat(sizeof(full), full, fs_entry_point, name);

        loc.Register(full, vfs_idx, ptr, size_real, size_compr, 0);
    }
    hdr->close();
}

IReader* CLocatorAPI::archive::read_db(const char* fname, const struct file& desc, u32 gran) const
{
    size_t start = (desc.ptr / gran) * gran;
    size_t end = (desc.ptr + desc.size_compressed) / gran;
    if ((desc.ptr + desc.size_compressed) % gran)
        end += 1;
    end *= gran;
    if (end > size)
        end = size;
    size_t sz = end - start;

    u8* ptr = (u8*)MapViewOfFile(hSrcMap, FILE_MAP_READ, 0, start, sz);
    if (!ptr)
    {
        VERIFY(ptr, "cannot create file mapping on file", fname);
        return nullptr;
    }

#ifdef DEBUG
    string512 temp;
    sprintf_s(temp, "%s:%s", *path, fname);

    register_file_mapping(ptr, sz, temp);
#endif // DEBUG

    size_t ptr_offs = desc.ptr - start;
    if (desc.size_real == desc.size_compressed)
        return xr_new<CPackReader>(ptr, ptr + ptr_offs, desc.size_real);

    // Compressed
    u8* dest = xr_alloc<u8>(desc.size_real);
    rtc_decompress(dest, desc.size_real, ptr + ptr_offs, desc.size_compressed);

    auto* R = xr_new<CTempReader>(dest, desc.size_real, 0);
    UnmapViewOfFile(ptr);

#ifdef DEBUG
    unregister_file_mapping(ptr, sz);
#endif // DEBUG

    return R;
}

CStreamReader* CLocatorAPI::archive::stream_db(const char* fname, const struct file& desc) const
{
    R_ASSERT(desc.size_compressed == desc.size_real, make_string("cannot use stream reading for compressed data %s, do not compress data to be streamed", fname).c_str());

    CMapStreamReader* R = xr_new<CMapStreamReader>();
    R->construct(hSrcMap, desc.ptr, desc.size_compressed, size, BIG_FILE_READER_WINDOW_SIZE);
    return R;
}

void CLocatorAPI::archive::cleanup_db()
{

}

void CLocatorAPI::archive::close_db()
{
    CloseHandle(hSrcMap);
    hSrcMap = nullptr;
}
