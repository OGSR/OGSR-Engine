#pragma once

IC CMapStreamReader::CMapStreamReader() {}

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

IC size_t CMapStreamReader::length() const { return m_file_size; }

IC void CMapStreamReader::seek(std::ptrdiff_t offset) { advance(offset - tell()); }

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