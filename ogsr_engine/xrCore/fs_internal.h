#pragma once

#include "lzhuf.h"
#include <io.h>
#include <fcntl.h>
#include <sys\stat.h>
#include <share.h>

class CFileWriter : public IWriter
{
private:
    FILE* hf;

public:
    CFileWriter(const char* name, bool exclusive)
    {
        R_ASSERT(name && name[0]);
        fName = name;
        VerifyPath(*fName);
        if (exclusive)
        {
            int handle = _sopen(*fName, _O_WRONLY | _O_TRUNC | _O_CREAT | _O_BINARY, SH_DENYWR);
            hf = _fdopen(handle, "wb");
        }
        else
        {
            hf = fopen(*fName, "wb");
            if (hf == 0)
                Msg("!Can't write file: '%s'. Error: '%s'.", *fName, _sys_errlist[errno]);
        }
    }

    virtual ~CFileWriter()
    {
        if (0 != hf)
        {
            fclose(hf);
            // release RO attrib
            DWORD dwAttr = GetFileAttributes(*fName);
            if ((dwAttr != u32(-1)) && (dwAttr & FILE_ATTRIBUTE_READONLY))
            {
                dwAttr &= ~FILE_ATTRIBUTE_READONLY;
                SetFileAttributes(*fName, dwAttr);
            }
        }
    }
    // kernel
    virtual void w(const void* _ptr, size_t count)
    {
        if ((0 != hf) && (0 != count))
        {
            const size_t W = fwrite(_ptr, sizeof(char), count, hf);
            R_ASSERT3(W == count, "Can't write mem block to file. Disk maybe full.", _sys_errlist[errno]);
        }
    };
    virtual void seek(size_t pos)
    {
        if (0 != hf)
            fseek(hf, pos, SEEK_SET);
    };
    virtual size_t tell() { return (0 != hf) ? ftell(hf) : 0; };
    virtual bool valid() { return (0 != hf); }
    virtual size_t flush() { return fflush(hf); }
};

// It automatically frees memory after destruction
class CTempReader : public IReader
{
public:
    CTempReader(void* _data, size_t _size, size_t _iterpos) : IReader(_data, _size, _iterpos) {}
    virtual ~CTempReader();
};

class CPackReader : public IReader
{
    void* base_address;

public:
    CPackReader(void* _base, void* _data, size_t _size) : IReader(_data, _size) { base_address = _base; }
    virtual ~CPackReader();
};

class CVirtualFileReader : public IReader
{
private:
    void *hSrcFile, *hSrcMap;

public:
    CVirtualFileReader(const char* cFileName);
    virtual ~CVirtualFileReader();
};
