#pragma once

class XRCORE_API CStreamReader : public IReaderBase<CStreamReader>
{
public:
    virtual size_t elapsed() const = 0;
    virtual const size_t& length() const = 0;
    virtual void seek(const int& offset) = 0;
    virtual size_t tell() const = 0;
    virtual void close() = 0;

    virtual void advance(const int& offset) = 0;
    virtual void r(void* buffer, size_t buffer_size) = 0;
    virtual CStreamReader* open_chunk(const u32& chunk_id) = 0;
};

class CMapStreamReader : public CStreamReader
{
private:
    HANDLE m_file_mapping_handle{};
    size_t m_start_offset{};
    size_t m_file_size{};
    size_t m_archive_size{};
    size_t m_window_size{};

private:
    size_t m_current_offset_from_start{};
    size_t m_current_window_size{};
    u8* m_current_map_view_of_file{};
    u8* m_start_pointer{};
    u8* m_current_pointer{};

private:
    void map(const size_t& new_offset);
    IC void unmap();
    IC void remap(const size_t& new_offset);

private:
    // should not be called
    IC CMapStreamReader(const CMapStreamReader& object);
    IC CMapStreamReader& operator=(const CMapStreamReader&);

public:
    IC CMapStreamReader();

public:
    virtual void construct(const HANDLE& file_mapping_handle, const size_t& start_offset, const size_t& file_size, const size_t& archive_size, const size_t& window_size);
    virtual void destroy();

public:
    IC const HANDLE& file_mapping_handle() const;
    IC size_t elapsed() const;
    IC const size_t& length() const;
    IC void seek(const int& offset);
    IC size_t tell() const;
    IC void close();

public:
    void advance(const int& offset);
    void r(void* buffer, size_t buffer_size);
    CStreamReader* open_chunk(const u32& chunk_id);
};

#include "stream_reader_inline.h"
