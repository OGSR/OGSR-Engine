#pragma once

#include "stream_reader.h"

class CFileStreamReader : public CMapStreamReader
{
private:
    typedef CMapStreamReader inherited;

private:
    HANDLE m_file_handle;

public:
    virtual void construct(LPCSTR file_name, const size_t& window_size);
    virtual void destroy();
};