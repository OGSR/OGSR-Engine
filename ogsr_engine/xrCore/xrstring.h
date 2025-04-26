#pragma once

#pragma pack(push, 4)

//////////////////////////////////////////////////////////////////////////
#pragma warning(disable : 4200)
struct XRCORE_API str_value
{
    std::atomic_uint32_t dwReference;
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

XRCORE_API extern  str_container* g_pStringContainer;

//////////////////////////////////////////////////////////////////////////
class shared_str
{
private:
    str_value* p_;

private:
    // ref-counting
    void _dec()
    {
        if (!p_)
            return;
        p_->dwReference--;
        if (0 == p_->dwReference)
            p_ = nullptr;
    }

private:
    void _set(const char* rhs)
    {
        str_value* v = g_pStringContainer->dock(rhs);
        if (v)
            v->dwReference++;
        _dec();
        p_ = v;
    }
    void _set(shared_str const& rhs)
    {
        str_value* v = rhs.p_;
        if (v)
            v->dwReference++;
        _dec();
        p_ = v;
    }
    const str_value* _get() const { return p_; }

public:
    // construction
    shared_str() { p_ = nullptr; }
    shared_str(const char* rhs)
    {
        p_ = nullptr;
        _set(rhs);
    }
    shared_str(shared_str const& rhs)
    {
        p_ = nullptr;
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

// string(char)
using xr_string = std::basic_string<char, std::char_traits<char>, xr_allocator<char>>;

DEFINE_VECTOR(xr_string, SStringVec, SStringVecIt);

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
    for (char& it : src)
        it = xr_string::value_type(tolower(it));
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

template <typename Key, typename Value, class _Alloc = xr_allocator<std::pair<const Key, Value>>>
using string_unordered_map = std::unordered_map<Key, Value, transparent_string_hash, transparent_string_equal, _Alloc>;

namespace xr_string_utils
{

template <typename T>
inline bool SplitFilename(T& str)
{
    size_t found = str.find_last_of("/\\");
    if (found != T::npos)
    {
        str.erase(found);
        return true;
    }
    return false;
}

//////////////////////////////////////////////////////////////////////////////////////////
// https://stackoverflow.com/questions/216823/how-to-trim-an-stdstring
// trim from start (in place)
template <typename T>
inline void ltrim(T& s)
{
    s.erase(s.begin(), std::find_if(s.begin(), s.end(), [](unsigned char ch) { return !std::isspace(ch); }));
}

// trim from end (in place)
template <typename T>
inline void rtrim(T& s)
{
    s.erase(std::find_if(s.rbegin(), s.rend(), [](unsigned char ch) { return !std::isspace(ch); }).base(), s.end());
}

// trim from both ends (in place)
template <typename T>
inline void trim(T& s)
{
    rtrim(s);
    ltrim(s);
}
////////////////////////////////////////////////////////////////////////////////////////////

template <typename T>
inline void strlwr(T& data)
{
    std::transform(data.begin(), data.end(), data.begin(), [](unsigned char c) { return std::tolower(c); });
}

} // namespace xr_string_utils

struct pred_shared_str
{
    // bool operator()(const char* x, const char* y) const { return std::strcmp(x, y) < 0; }
    bool operator()(const shared_str& x, const shared_str& y) const { return xr_strcmp(x, y) < 0; }
};
