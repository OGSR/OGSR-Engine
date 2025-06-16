#pragma once

IC CMapStreamReader::CMapStreamReader() {}

IC CMapStreamReader::CMapStreamReader(const CMapStreamReader& object)
    : m_start_offset(object.m_start_offset), m_file_size(object.m_file_size), m_archive_size(object.m_archive_size), m_window_size(object.m_window_size)
{
    // should be never called
}

IC CMapStreamReader& CMapStreamReader::operator=(const CMapStreamReader&)
{
    // should be never called
    return (*this);
}

IC const HANDLE& CMapStreamReader::file_mapping_handle() const { return (m_file_mapping_handle); }

IC void CMapStreamReader::unmap() { UnmapViewOfFile(m_current_map_view_of_file); }

IC void CMapStreamReader::remap(const size_t& new_offset)
{
    unmap();
    map(new_offset);
}

IC size_t CMapStreamReader::elapsed() const
{
    size_t offset_from_file_start = tell();
    VERIFY(m_file_size >= offset_from_file_start);
    return (m_file_size - offset_from_file_start);
}

IC const size_t& CMapStreamReader::length() const { return (m_file_size); }

IC void CMapStreamReader::seek(const int& offset) { advance(offset - tell()); }

IC size_t CMapStreamReader::tell() const
{
    VERIFY(m_current_pointer >= m_start_pointer);
    VERIFY(size_t(m_current_pointer - m_start_pointer) <= m_current_window_size);
    return (m_current_offset_from_start + (m_current_pointer - m_start_pointer));
}

IC void CMapStreamReader::close()
{
    destroy();
    CMapStreamReader* self = this;
    xr_delete(self);
}