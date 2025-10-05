#include "stdafx.h"
#include "file_stream_reader.h"

void CFileStreamReader::construct(LPCSTR file_name, const size_t& window_size)
{
    m_file_handle = CreateFile(file_name, GENERIC_READ, FILE_SHARE_READ, nullptr, OPEN_EXISTING, 0, nullptr);

    VERIFY(m_file_handle != INVALID_HANDLE_VALUE);

    LARGE_INTEGER sz;
    GetFileSizeEx(m_file_handle, &sz);
    size_t file_size = sz.QuadPart;

    HANDLE file_mapping_handle = CreateFileMapping(m_file_handle, nullptr, PAGE_READONLY, 0, 0, nullptr);
    VERIFY(file_mapping_handle != INVALID_HANDLE_VALUE);

    inherited::construct(file_mapping_handle, 0, file_size, file_size, window_size);
}

void CFileStreamReader::destroy()
{
    HANDLE file_mapping_handle = this->file_mapping_handle();
    inherited::destroy();
    CloseHandle(file_mapping_handle);
    CloseHandle(m_file_handle);
}
