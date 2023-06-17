#include "stdafx.h"

#include "fs_internal.h"
#include "trivial_encryptor.h"

#ifdef DEBUG
XRCORE_API u32 g_file_mapped_memory = 0;
u32 g_file_mapped_count = 0;
typedef xr_map<u32, std::pair<u32, shared_str>> FILE_MAPPINGS;
FILE_MAPPINGS g_file_mappings;
static std::mutex g_file_mappings_Mutex;

void register_file_mapping(void* address, const u32& size, LPCSTR file_name)
{
    std::scoped_lock<decltype(g_file_mappings_Mutex)> lock(g_file_mappings_Mutex);

    FILE_MAPPINGS::const_iterator I = g_file_mappings.find(*(u32*)&address);
    VERIFY(I == g_file_mappings.end());
    g_file_mappings.insert(std::make_pair(*(u32*)&address, std::make_pair(size, shared_str(file_name))));

    g_file_mapped_memory += size;
    ++g_file_mapped_count;
}

void unregister_file_mapping(void* address, const u32& size)
{
    std::scoped_lock<decltype(g_file_mappings_Mutex)> lock(g_file_mappings_Mutex);

    FILE_MAPPINGS::iterator I = g_file_mappings.find(*(u32*)&address);
    VERIFY(I != g_file_mappings.end());
    //	VERIFY2							((*I).second.first == size,make_string("file mapping sizes are different: %d -> %d",(*I).second.first,size));
    g_file_mapped_memory -= (*I).second.first;
    --g_file_mapped_count;

    g_file_mappings.erase(I);
}

XRCORE_API void dump_file_mappings()
{
    std::scoped_lock<decltype(g_file_mappings_Mutex)> lock(g_file_mappings_Mutex);

    Msg("* active file mappings (%d):", g_file_mappings.size());

    FILE_MAPPINGS::const_iterator I = g_file_mappings.begin();
    FILE_MAPPINGS::const_iterator E = g_file_mappings.end();
    for (; I != E; ++I)
        Msg("* [0x%08x][%d][%s]", (*I).first, (*I).second.first, (*I).second.second.c_str());
}
#endif // DEBUG

//////////////////////////////////////////////////////////////////////
// Tools
//////////////////////////////////////////////////////////////////////

#include <filesystem>
void VerifyPath(const std::string_view path) //Проверяет путь до файла. Если папки в пути отсутствуют - создаёт их.
{
    const auto lastSepPos = path.find_last_of('\\');
    const auto foldersPath = (lastSepPos != std::string_view::npos) ? path.substr(0, lastSepPos) : path;
    std::error_code e;
    namespace stdfs = std::filesystem;
    stdfs::create_directories(stdfs::path(foldersPath.begin(), foldersPath.end()), e);
    (void)e;
}

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////
//---------------------------------------------------
// memory
CMemoryWriter::~CMemoryWriter() { xr_free(data); }

void CMemoryWriter::w(const void* ptr, u32 count)
{
    if (position + count > mem_size)
    {
        // reallocate
        if (mem_size == 0)
            mem_size = 1024 * 1024;
        while (mem_size <= (position + count))
            mem_size *= 2;
        if (0 == data)
            data = (BYTE*)Memory.mem_alloc(mem_size);
        else
            data = (BYTE*)Memory.mem_realloc(data, mem_size);
    }
    CopyMemory(data + position, ptr, count);
    position += count;
    if (position > file_size)
        file_size = position;
}

void CMemoryWriter::reserve(const size_t count)
{
    mem_size = count;
    data = (BYTE*)Memory.mem_alloc(mem_size);
}

bool CMemoryWriter::save_to(LPCSTR fn)
{
    IWriter* F = FS.w_open(fn);
    if (F)
    {
        F->w(pointer(), size());
        FS.w_close(F);
        return true;
    }
    return false;
}

void IWriter::open_chunk(u32 type)
{
    w_u32(type);
    chunk_pos.push(tell());
    w_u32(0); // the place for 'size'
}
void IWriter::close_chunk()
{
    VERIFY(!chunk_pos.empty());

    int pos = tell();
    seek(chunk_pos.top());
    w_u32(pos - chunk_pos.top() - 4);
    seek(pos);
    chunk_pos.pop();
}
u32 IWriter::chunk_size() // returns size of currently opened chunk, 0 otherwise
{
    if (chunk_pos.empty())
        return 0;
    return tell() - chunk_pos.top() - 4;
}

void IWriter::w_compressed(void* ptr, u32 count, const bool encrypt, const bool is_ww)
{
    BYTE* dest = 0;
    unsigned dest_sz = 0;
    _compressLZ(&dest, &dest_sz, ptr, count);

    if (encrypt)
        g_trivial_encryptor.encode(dest, dest_sz, dest, is_ww ? trivial_encryptor::key_flag::worldwide : trivial_encryptor::key_flag::russian);

    if (dest && dest_sz)
        w(dest, dest_sz);
    xr_free(dest);
}

void IWriter::w_chunk(u32 type, void* data, u32 size, const bool encrypt, const bool is_ww)
{
    open_chunk(type);

    if (type & CFS_CompressMark)
        w_compressed(data, size, encrypt, is_ww);
    else
        w(data, size);

    close_chunk();
}
void IWriter::w_sdir(const Fvector& D)
{
    Fvector C;
    float mag = D.magnitude();
    if (mag > EPS_S)
    {
        C.div(D, mag);
    }
    else
    {
        C.set(0, 0, 1);
        mag = 0;
    }
    w_dir(C);
    w_float(mag);
}
void IWriter::w_printf(const char* format, ...)
{
    va_list mark;
    char buf[1024];
    va_start(mark, format);
    vsprintf(buf, format, mark);
    w(buf, xr_strlen(buf));
}

//---------------------------------------------------
// base stream
IReader* IReader::open_chunk(u32 ID)
{
    BOOL bCompressed;
    u32 dwSize = find_chunk(ID, &bCompressed);
    if (dwSize != 0)
    {
        if (bCompressed)
        {
            BYTE* dest;
            unsigned dest_sz;
            _decompressLZ(&dest, &dest_sz, pointer(), dwSize);
            return xr_new<CTempReader>(dest, dest_sz, tell() + dwSize);
        }
        else
        {
            return xr_new<IReader>(pointer(), dwSize, tell() + dwSize);
        }
    }
    else
        return 0;
};

void IReader::close()
{
    auto* temp = this;
    xr_delete(temp);
}

IReader* IReader::open_chunk_iterator(u32& ID, IReader* _prev)
{
    if (0 == _prev)
    {
        // first
        rewind();
    }
    else
    {
        // next
        seek(_prev->iterpos);
        _prev->close();
    }

    //	open
    if (elapsed() < static_cast<int>(sizeof u32 * 2))
        return nullptr;

    ID = r_u32();
    u32 _size = r_u32();

    //На всякий случай тут тоже так сделаем по аналогии с find_chunk()
    if (elapsed() < static_cast<int>(_size))
    {
        Msg("!![%s] chunk [%u] has invalid size [%u], return elapsed size [%d]", __FUNCTION__, ID, _size, elapsed());
        _size = elapsed();
    }

    if (ID & CFS_CompressMark)
    {
        // compressed
        u8* dest;
        unsigned dest_sz;
        _decompressLZ(&dest, &dest_sz, pointer(), _size);
        return xr_new<CTempReader>(dest, dest_sz, tell() + _size);
    }
    else
    {
        // normal
        return xr_new<IReader>(pointer(), _size, tell() + _size);
    }
}

constexpr unsigned char boms[]{0xef, 0xbb, 0xbf};

void IReader::skip_bom(const char* dbg_name)
{
    if (elapsed() < 3)
        return;

    for (const auto& bom : boms)
    {
        if (static_cast<const unsigned char>(data[Pos]) != bom)
        {
            seek(0);
            return;
        }

        Pos++;
    }

    Msg("! Skip BOM for file [%s]", dbg_name);
}

void IReader::r(void* p, int cnt)
{
    R_ASSERT(Pos + cnt <= Size);
    CopyMemory(p, pointer(), cnt);
    advance(cnt);
#ifdef DEBUG
    BOOL bShow = FALSE;
    if (dynamic_cast<CVirtualFileReader*>(this))
        bShow = TRUE;
    if (bShow)
    {
        FS.dwOpenCounter++;
    }
#endif
};

constexpr bool is_term(const char a) { return a == '\r' || a == '\n'; }

IC u32 IReader::advance_term_string()
{
    u32 sz = 0;
    char* src = (char*)data;
    while (!eof())
    {
        Pos++;
        sz++;
        if (!eof() && is_term(src[Pos]))
        {
            while (!eof() && is_term(src[Pos]))
                Pos++;

            break;
        }
    }
    return sz;
}

void IReader::r_string(char* dest, u32 tgt_sz)
{
    char* src = (char*)data + Pos;
    u32 sz = advance_term_string();
    R_ASSERT2(sz < (tgt_sz - 1), "Dest string less than needed.");
    strncpy(dest, src, sz);
    dest[sz] = 0;
}

void IReader::r_string(xr_string& dest)
{
    char* src = (char*)data + Pos;
    u32 sz = advance_term_string();
    dest.assign(src, sz);
}

void IReader::r_stringZ(char* dest, u32 tgt_sz)
{
    char* src = (char*)data;

    u32 sz = 0;

    while ((src[Pos] != 0) && (!eof()))
    {
        sz++;

        R_ASSERT2(sz < (tgt_sz - 1), "Dest string less than needed.");

        *dest++ = src[Pos++];
    }

    *dest = 0;

    if (!eof())
        Pos++;
}

void IReader::r_stringZ(shared_str& dest)
{
    char* src = (char*)(data + Pos);

    int size = 0;
    while ((src[size] != 0) && (!eof()))
    {
        size++;
        Pos++;
    }

    std::string tmp;
    tmp.assign(src, size);

    dest = tmp.c_str();

    //advance(size);

    if (!eof())
        Pos++;
}
void IReader::r_stringZ(xr_string& dest)
{
    char* src = (char*)(data + Pos);

    int size = 0;
    while ((src[size] != 0) && (!eof()))
    {
        size++;
        Pos++;
    }

    dest.assign(src, size);

    //advance(size);

    if (!eof())
        Pos++;
}

void IReader::skip_stringZ()
{
    char* src = (char*)data;
    while ((src[Pos] != 0) && (!eof()))
        Pos++;

    Pos++;
};

//---------------------------------------------------
// temp stream
CTempReader::~CTempReader() { xr_free(data); };
//---------------------------------------------------
// pack stream
CPackReader::~CPackReader()
{
#ifdef DEBUG
    unregister_file_mapping(base_address, Size);
#endif // DEBUG

    UnmapViewOfFile(base_address);
};
//---------------------------------------------------

CVirtualFileReader::CVirtualFileReader(const char* cFileName)
{
    Pos = 0;

    // Open the file
    hSrcFile = CreateFile(cFileName, GENERIC_READ, FILE_SHARE_READ | FILE_SHARE_WRITE, 0, OPEN_EXISTING, 0, 0);
    R_ASSERT3(hSrcFile != INVALID_HANDLE_VALUE, cFileName, Debug.error2string(GetLastError()));
    Size = (int)GetFileSize(hSrcFile, NULL);
    if (Size == 0)
        Msg("~~[%s] Found empty file: [%s]", __FUNCTION__, cFileName);

    hSrcMap = CreateFileMapping(hSrcFile, 0, PAGE_READONLY, 0, 0, 0);
    R_ASSERT3(hSrcMap != INVALID_HANDLE_VALUE, cFileName, Debug.error2string(GetLastError()));

    data = (char*)MapViewOfFile(hSrcMap, FILE_MAP_READ, 0, 0, 0);
    R_ASSERT3(!Size || data, cFileName, Debug.error2string(GetLastError()));

#ifdef DEBUG
    register_file_mapping(data, Size, cFileName);
#endif // DEBUG
}

CVirtualFileReader::~CVirtualFileReader()
{
#ifdef DEBUG
    unregister_file_mapping(data, Size);
#endif // DEBUG

    UnmapViewOfFile((void*)data);
    CloseHandle(hSrcMap);
    CloseHandle(hSrcFile);
}
