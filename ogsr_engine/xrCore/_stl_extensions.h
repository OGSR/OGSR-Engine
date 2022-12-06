#pragma once

using std::swap; // TODO: Убрать!


struct xr_allocator
{
    template <typename T>
    struct helper
    {
        typedef std::allocator<T> result;
    };

    static void* alloc(const u32& n) { return Memory.mem_alloc(n); }
    template <typename T>
    static void dealloc(T*& p)
    {
        xr_free(p);
    }
};

// string(char)
using xr_string = std::basic_string<char, std::char_traits<char>, std::allocator<char>>;

template <typename T, typename allocator = std::allocator<T>>
using xr_vector = std::vector<T, allocator>;

template <typename T>
void clear_and_reserve(xr_vector<T>& vec)
{
    if (vec.capacity() <= (vec.size() + vec.size() / 4))
        vec.clear();
    else
    {
        const size_t old = vec.size();
        vec.clear();
        vec.reserve(old);
    }
}

template <typename T, typename allocator = std::allocator<T>>
using xr_deque = std::deque<T, allocator>;

template <typename T, class C = xr_deque<T>>
using xr_stack = std::stack<T, C>;

template <typename T, typename allocator = std::allocator<T>>
using xr_list = std::list<T, allocator>;

template <typename K, class P = std::less<K>, typename allocator = std::allocator<K>>
using xr_set = std::set<K, P, allocator>;

template <typename K, class P = std::less<K>, typename allocator = std::allocator<K>>
using xr_multiset = std::multiset<K, P, allocator>;

template <typename K, class V, class P = std::less<K>, typename allocator = std::allocator<std::pair<const K, V>>>
using xr_map = std::map<K, V, P, allocator>;

template <typename K, class V, class P = std::less<K>, typename allocator = std::allocator<std::pair<const K, V>>>
using xr_multimap = std::multimap<K, V, P, allocator>;

template <typename K, typename V, class _Hasher = std::hash<K>, class _Keyeq = std::equal_to<K>, class _Alloc = std::allocator<std::pair<const K, V>>>
using xr_unordered_map = std::unordered_map<K, V, _Hasher, _Keyeq, _Alloc>;

#define mk_pair std::make_pair // TODO: Везде заменить, а это убрать.

struct pred_str
{
    bool operator()(const char* x, const char* y) const { return std::strcmp(x, y) < 0; }
};
struct pred_stri
{
    bool operator()(const char* x, const char* y) const { return _stricmp(x, y) < 0; }
};

// STL extensions
#define DEF_VECTOR(N, T) \
    typedef xr_vector<T> N; \
    typedef N::iterator N##_it;
#define DEF_LIST(N, T) \
    typedef xr_list<T> N; \
    typedef N::iterator N##_it;
#define DEF_DEQUE(N, T) \
    typedef xr_deque<T> N; \
    typedef N::iterator N##_it;
#define DEF_MAP(N, K, T) \
    typedef xr_map<K, T> N; \
    typedef N::iterator N##_it;

#define DEFINE_DEQUE(T, N, I) \
    typedef xr_deque<T> N; \
    typedef N::iterator I;
#define DEFINE_LIST(T, N, I) \
    typedef xr_list<T> N; \
    typedef N::iterator I;
#define DEFINE_VECTOR(T, N, I) \
    typedef xr_vector<T> N; \
    typedef N::iterator I;
#define DEFINE_MAP(K, T, N, I) \
    typedef xr_map<K, T> N; \
    typedef N::iterator I;
#define DEFINE_MAP_PRED(K, T, N, I, P) \
    typedef xr_map<K, T, P> N; \
    typedef N::iterator I;
#define DEFINE_MMAP(K, T, N, I) \
    typedef xr_multimap<K, T> N; \
    typedef N::iterator I;
#define DEFINE_SVECTOR(T, C, N, I) \
    typedef svector<T, C> N; \
    typedef N::iterator I;
#define DEFINE_SET(T, N, I) \
    typedef xr_set<T> N; \
    typedef N::iterator I;
#define DEFINE_SET_PRED(T, N, I, P) \
    typedef xr_set<T, P> N; \
    typedef N::iterator I;
#define DEFINE_STACK(T, N) typedef xr_stack<T> N;

#include "FixedVector.h"

// auxilary definition
DEFINE_VECTOR(bool, boolVec, boolIt);
DEFINE_VECTOR(BOOL, BOOLVec, BOOLIt);
DEFINE_VECTOR(BOOL*, LPBOOLVec, LPBOOLIt);
DEFINE_VECTOR(Frect, FrectVec, FrectIt);
DEFINE_VECTOR(Irect, IrectVec, IrectIt);
DEFINE_VECTOR(Fplane, PlaneVec, PlaneIt);
DEFINE_VECTOR(Fvector2, Fvector2Vec, Fvector2It);
DEFINE_VECTOR(Fvector, FvectorVec, FvectorIt);
DEFINE_VECTOR(Fvector*, LPFvectorVec, LPFvectorIt);
DEFINE_VECTOR(Fcolor, FcolorVec, FcolorIt);
DEFINE_VECTOR(Fcolor*, LPFcolorVec, LPFcolorIt);
DEFINE_VECTOR(LPSTR, LPSTRVec, LPSTRIt);
DEFINE_VECTOR(LPCSTR, LPCSTRVec, LPCSTRIt);
DEFINE_VECTOR(xr_string, SStringVec, SStringVecIt);

DEFINE_VECTOR(s8, S8Vec, S8It);
DEFINE_VECTOR(s8*, LPS8Vec, LPS8It);
DEFINE_VECTOR(s16, S16Vec, S16It);
DEFINE_VECTOR(s16*, LPS16Vec, LPS16It);
DEFINE_VECTOR(s32, S32Vec, S32It);
DEFINE_VECTOR(s32*, LPS32Vec, LPS32It);
DEFINE_VECTOR(u8, U8Vec, U8It);
DEFINE_VECTOR(u8*, LPU8Vec, LPU8It);
DEFINE_VECTOR(u16, U16Vec, U16It);
DEFINE_VECTOR(u16*, LPU16Vec, LPU16It);
DEFINE_VECTOR(u32, U32Vec, U32It);
DEFINE_VECTOR(u32*, LPU32Vec, LPU32It);
DEFINE_VECTOR(float, FloatVec, FloatIt);
DEFINE_VECTOR(float*, LPFloatVec, LPFloatIt);
DEFINE_VECTOR(int, IntVec, IntIt);
DEFINE_VECTOR(int*, LPIntVec, LPIntIt);
