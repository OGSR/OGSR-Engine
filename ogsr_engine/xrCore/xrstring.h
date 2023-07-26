#pragma once

#pragma pack(push, 4)

//////////////////////////////////////////////////////////////////////////
#pragma warning(disable : 4200)
struct XRCORE_API str_value
{
    u32 dwReference;
    u32 dwLength;
    u32 dwCRC;
    str_value* next;
    char value[];
};

#pragma warning(default : 4200)

struct str_container_impl;

//////////////////////////////////////////////////////////////////////////
class XRCORE_API str_container
{
public:
    str_container();
    ~str_container();

    str_value* dock(pcstr value) const;
    void clean() const;
    void dump() const;
    void verify() const;

    [[nodiscard]] size_t stat_economy() const;

private:
    str_container_impl* impl;
};

XRCORE_API extern str_container* g_pStringContainer;

//////////////////////////////////////////////////////////////////////////
class shared_str
{
private:
    str_value* p_;

protected:
    // ref-counting
    void _dec()
    {
        if (0 == p_)
            return;
        p_->dwReference--;
        if (0 == p_->dwReference)
            p_ = 0;
    }

public:
    void _set(const char* rhs)
    {
        str_value* v = g_pStringContainer->dock(rhs);
        if (0 != v)
            v->dwReference++;
        _dec();
        p_ = v;
    }
    void _set(shared_str const& rhs)
    {
        str_value* v = rhs.p_;
        if (0 != v)
            v->dwReference++;
        _dec();
        p_ = v;
    }
    const str_value* _get() const { return p_; }

public:
    // construction
    shared_str() { p_ = 0; }
    shared_str(const char* rhs)
    {
        p_ = 0;
        _set(rhs);
    }
    shared_str(shared_str const& rhs)
    {
        p_ = 0;
        _set(rhs);
    }
    ~shared_str() { _dec(); }

    // assignment & accessors
    shared_str& operator=(const char* rhs)
    {
        _set(rhs);
        return *this;
    }
    shared_str& operator=(shared_str const& rhs)
    {
        _set(rhs);
        return *this;
    }

    bool operator==(shared_str const& rhs) const { return _get() == rhs._get(); }
    bool operator!=(shared_str const& rhs) const { return _get() != rhs._get(); }
    bool operator<(shared_str const& rhs) const
    {
        if (!p_)
            return true;
        else
            return strcmp(p_->value, rhs.c_str()) < 0;
    }
    bool operator>(shared_str const&) const = delete;
    char operator[](size_t id) const { return p_->value[id]; }
    bool operator!() const { return !p_; }
    const char* operator*() const { return p_ ? p_->value : nullptr; }

    // Чтобы можно было легко кастить в std::string_view как и все остальные строки
    operator std::string_view() const { return std::string_view{p_ ? p_->value : ""}; }

    const char* c_str() const { return p_ ? p_->value : nullptr; }

    //Используется в погодном редакторе.
    const char* data() const { return p_ ? p_->value : ""; }

    u32 size() const { return p_ ? p_->dwLength : 0; }

    bool empty() const { return size() == 0; }

    void swap(shared_str& rhs)
    {
        str_value* tmp = p_;
        p_ = rhs.p_;
        rhs.p_ = tmp;

    }
    bool equal(const shared_str& rhs) const { return (p_ == rhs.p_); }

    shared_str& __cdecl sprintf(const char* format, ...)
    {
        string4096 buf;
        va_list p;
        va_start(p, format);
        int vs_sz = _vsnprintf(buf, sizeof(buf) - 1, format, p);
        buf[sizeof(buf) - 1] = 0;
        va_end(p);
        if (vs_sz)
            _set(buf);
        return (shared_str&)*this;
    }
};

// externally visible standart functionality
IC void swap(shared_str& lhs, shared_str& rhs) { lhs.swap(rhs); }

IC u32 xr_strlen(shared_str& a) { return a.size(); }
IC int xr_strcmp(const shared_str& a, const char* b) { return xr_strcmp(*a, b); }
IC int xr_strcmp(const char* a, const shared_str& b) { return xr_strcmp(a, *b); }
IC int xr_strcmp(const shared_str& a, const shared_str& b)
{
    if (a.equal(b))
        return 0;
    else
        return xr_strcmp(*a, *b);
}

IC void xr_strlwr(xr_string& src)
{
    for (xr_string::iterator it = src.begin(); it != src.end(); it++)
        *it = xr_string::value_type(tolower(*it));
}
IC void xr_strlwr(shared_str& src)
{
    if (*src)
    {
        LPSTR lp = xr_strdup(*src);
        xr_strlwr(lp);
        src = lp;
        xr_free(lp);
    }
}

#pragma pack(pop)

struct transparent_string_hash
{
    using is_transparent = void; // https://www.cppstories.com/2021/heterogeneous-access-cpp20/
    using hash_type = std::hash<std::string_view>;
    [[nodiscard]] size_t operator()(const std::string_view txt) const noexcept { return hash_type{}(txt); }
    [[nodiscard]] size_t operator()(const std::string& txt) const noexcept { return hash_type{}(txt); }
    [[nodiscard]] size_t operator()(const char* txt) const noexcept { return hash_type{}(txt); }
    [[nodiscard]] size_t operator()(const shared_str& txt) const noexcept { return hash_type{}(txt); }
};

struct transparent_string_equal
{
    using is_transparent = void;
    [[nodiscard]] bool operator()(const std::string_view lhs, const std::string_view rhs) const { return lhs == rhs; }
    [[nodiscard]] bool operator()(const shared_str& lhs, const shared_str& rhs) const { return lhs == rhs; }
    [[nodiscard]] bool operator()(const char* lhs, const char* rhs) const { return !strcmp(lhs, rhs); }
};

template <typename Key, typename Value, class _Alloc = std::allocator<std::pair<const Key, Value>>>
using string_unordered_map = std::unordered_map<Key, Value, transparent_string_hash, transparent_string_equal, _Alloc>;

inline bool SplitFilename(std::string& str)
{
    size_t found = str.find_last_of("/\\");
    if (found != std::string::npos)
    {
        str.erase(found);
        return true;
    }
    return false;
}
