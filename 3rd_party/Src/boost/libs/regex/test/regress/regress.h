/*
 *
 * Copyright (c) 1998-2002
 * Dr John Maddock
 *
 * Permission to use, copy, modify, distribute and sell this software
 * and its documentation for any purpose is hereby granted without fee,
 * provided that the above copyright notice appear in all copies and
 * that both that copyright notice and this permission notice appear
 * in supporting documentation.  Dr John Maddock makes no representations
 * about the suitability of this software for any purpose.  
 * It is provided "as is" without express or implied warranty.
 *
 */
 
 /*
  *
  *   FILE     regress.h
  *   VERSION  see <boost/version.hpp>
  *
  * Function and data declarations for regress.
  *
  */

#ifndef _REGRESS_H
#define _REGRESS_H

#include <boost/regex/config.hpp>

#ifdef BOOST_RE_OLD_IOSTREAM
#include <iostream.h>
#else
#include <iostream>
using std::cout;
using std::cerr;
using std::cin;
using std::istream;
using std::ostream;
using std::endl;
#endif

#include <string>
#include <iostream>
#include <boost/regex.hpp>

#if defined(TEST_UNICODE)

#ifdef BOOST_NO_WREGEX
#error "Wide character support is not enabled in regex lib - can't build wide character test program"
#endif

#ifdef __GNUC__
#define char_t wchar_t
#else
typedef wchar_t char_t;
#endif

typedef std::basic_string<char_t> string_type;
typedef std::basic_string<char> nstring_type;
inline istream& get_line(istream& is, nstring_type& s, char delim = '\n');
istream& get_line(istream& is, string_type& s, char delim = L'\n');
#define BOOST_RE_STR(x) L##x
std::string make_narrow(const wchar_t* ptr);
inline std::string make_narrow(const std::wstring& s)
{ return make_narrow(s.c_str()); }

#else // TEST_UNICODE

#ifdef __GNUC__
#define char_t char
#else
typedef char char_t;
#endif

typedef std::basic_string<char_t> string_type;
inline istream& get_line(istream& is, string_type& s, char delim = '\n');
#define BOOST_RE_STR(x) x
inline std::string make_narrow(const std::string& s)
{ return s; }

#endif // TEST_UNICODE

void parse_input_line(const string_type& s);
void expand_escapes(string_type& s);
void run_tests();
void begin_error();
void reset_error();
int to_int(string_type::const_iterator i, string_type::const_iterator j);

#define MAX_MATCHES 30
extern string_type expression;
extern string_type search_text;
extern string_type format_string;
extern string_type merge_string;
extern int matches[MAX_MATCHES];

#ifndef MAX_PATH
#define MAX_PATH 260
#endif

extern char file[MAX_PATH];
extern int line;
extern bool do_test;
extern unsigned int flags[5];
extern unsigned error_count;

struct flag_info
{
   const char_t* name; // printable flag name
   unsigned int len;   // length of name
   unsigned int value; // value of the flag
   unsigned int id;    // index to assign value to
};

extern flag_info flag_data[];

//
// class jm_debug_alloc
// NB this is a byte based allocator
//
class jm_debug_alloc;

template <class T, class Base>
class allocator_binder : public Base
{
public:

   typedef T              value_type;
   typedef value_type *   pointer;
   typedef const T*       const_pointer;
   typedef T&             reference;
   typedef const T&       const_reference;
   typedef std::size_t    size_type;
   typedef std::ptrdiff_t difference_type;
   typedef Base           base_type;

   allocator_binder(){}
   allocator_binder(const base_type& x) : Base(x){}
   allocator_binder& operator=(const base_type& x)
   {
      *(static_cast<base_type*>(this)) = x;
      return *this;
   }

   ~allocator_binder(){}

   pointer address(reference x) { return &x; }

   const_pointer address(const_reference x) const { return &x; }

   pointer allocate(size_type n, const void* = 0) 
   {
      return n != 0 ?
         reinterpret_cast<pointer>(base_type::allocate(n * sizeof(value_type)))
         : 0;
   }

   void deallocate(pointer p, size_type n)
   {
      assert( (p == 0) == (n == 0) );
      if (p != 0)
         base_type::deallocate((void*)p, n * sizeof(value_type));
   }

   size_type max_size() const
   { return size_t(-1) / sizeof(value_type); }

   void construct(pointer p, const T& val) const
   { boost::detail::allocator_construct(p, val); }

   void destroy(pointer p) const
   { boost::detail::allocator_destroy(p); }

#ifndef BOOST_NO_MEMBER_TEMPLATES
   template <class U>
   struct rebind
   {
      typedef allocator_binder<U, jm_debug_alloc> other;
   };
#endif

};

class jm_debug_alloc
{
private:
   int* blocks, *count;
   jm_debug_alloc* guard;
public:
   typedef std::size_t    size_type;
   typedef std::ptrdiff_t difference_type;
   typedef char*          pointer;
   typedef const char*    const_pointer;
   typedef char&          reference;
   typedef const char&    const_reference;
   typedef char           value_type;
   typedef jm_debug_alloc base_type;


#ifndef BOOST_NO_MEMBER_TEMPLATES
   template <class U>
   struct rebind
   {
      typedef allocator_binder<U, jm_debug_alloc> other;
   };
#endif

   jm_debug_alloc();

   jm_debug_alloc(const jm_debug_alloc& d);
   jm_debug_alloc& operator=(const jm_debug_alloc& d);

   ~jm_debug_alloc();
   void free_();
   static pointer address(reference x)
   {
      return &x;
   }
   static const_pointer address(const_reference x)
   {
      return &x;
   }
   unsigned maxi(unsigned i, unsigned j)
   {
      return i < j ? j : i;
   }
   void* allocate(size_type n, void* hint = 0);
   void deallocate(void* p, size_type n);

   static size_type max_size()
   {
      return UINT_MAX;
   }

   static void construct(pointer p, const char& val)
   { boost::detail::allocator_construct(p, val); }

   static void destroy(pointer p)
   { boost::detail::allocator_destroy(p); }
};

#ifdef __BORLANDC__
#if __BORLANDC__ < 0x520

#define BOOST_RE_BORLAND_FIX

#endif
#endif



//
// class debug_iterator
//
template <class T>
struct debug_iterator
#if !defined(BOOST_NO_STD_ITERATOR) || defined(BOOST_MSVC_STD_ITERATOR)
: public std::iterator<std::random_access_iterator_tag, char_t>
#endif
{
   typedef std::ptrdiff_t                    difference_type;
   typedef char_t                            value_type;
   typedef char_t*                           pointer;
   typedef char_t&                           reference;
   typedef std::random_access_iterator_tag   iterator_category;
   typedef debug_iterator                    self;
   typedef std::size_t                       size_type;

  T cur;
  T first;
  T last;

  debug_iterator(T c, T f, T l)
    : cur(c), first(f), last(l) {}
  debug_iterator() : cur(), first(), last() {}
  debug_iterator(const debug_iterator& x)
    : cur(x.cur), first(x.first), last(x.last) {}
  debug_iterator& operator=(const debug_iterator& x)
  {
     cur = x.cur;
     first = x.first;
     last = x.last;
     return *this;
  }

  reference operator*() const
  {
      if(cur == last)
      {
         begin_error();
         cout << "Error: attempt to dereference iterator past range" << endl;
      }
      return *cur;
  }

  operator T()const { return cur; }

  difference_type operator-(const self& x) const {
    return (difference_type)(cur - x.cur);
  }

  self& operator++()
  {
      if(cur == last)
      {
         begin_error();
         cout << "Error: attempt to increment iterator past range" << endl;
         return *this;
      }
    ++cur;
    return *this;
  }
  self operator++(int)
  {
    self tmp = *this;
    ++*this;
    return tmp;
  }

  self& operator--()
  {
      if(cur == first)
      {
         begin_error();
         cout << "Error: attempt to decrement iterator past range" << endl;
         return *this;
      }
    --cur;
    return *this;
  }
  self operator--(int)
  {
    self tmp = *this;
    --*this;
    return tmp;
  }

  self& operator+=(difference_type n)
  {
      if((last - cur) < n)
      {
         begin_error();
         cout << "Error: attempt to increment iterator past range" << endl;
         return *this;
      }
    cur += n;
    return *this;
  }

  self operator+(difference_type n) const
  {
    self tmp = *this;
    return tmp += n;
  }

  self& operator-=(difference_type n) { return *this += -n; }
 
  self operator-(difference_type n) const
  {
    self tmp = *this;
    return tmp -= n;
  }

  reference operator[](difference_type n) const { return *(cur + n); }

  bool operator==(const self& x) const { return cur == x.cur; }
  bool operator!=(const self& x) const { return !(*this == x); }
  bool operator<(const self& x) const {
    return (cur < x.cur);
  }
  friend self operator+(difference_type n, const self& i)
  {
    self tmp = i;
    return tmp += n;
  }
};

#if defined(__SGI_STL_PORT) 
namespace std{
template <class T>
#if __SGI_STL_PORT <= 0x400
inline random_access_iterator_tag __STL_CALL __ITERATOR_CATEGORY(const debug_iterator<T>&) {
#else
inline random_access_iterator_tag __iterator_category(const debug_iterator<T>&) {
#endif
  return random_access_iterator_tag();
}
}
#elif defined(__STL_CONFIG_H)
namespace std{
template <class T>
inline random_access_iterator_tag 
iterator_category(const debug_iterator<T>&) {
  return random_access_iterator_tag();
}
}
#endif
#if (defined(BOOST_MSVC) || defined(__ICL)) && (defined(_YVALS) || defined(_CPPLIB_VER))
namespace std{
template <class T>
inline random_access_iterator_tag __cdecl
_Iter_cat(const debug_iterator<T>&) {
  return random_access_iterator_tag();
}
}
#endif
#ifdef BOOST_RWSTD_VER
namespace std{
template <class T>
inline random_access_iterator_tag
__iterator_category(const debug_iterator<T>&) {
  return random_access_iterator_tag();
}
}
#endif

#if defined(_WIN32) && !defined(BOOST_RE_TEST_LOCALE_CPP) && !defined(BOOST_RE_TEST_LOCALE_C) && !defined(BOOST_RE_TEST_LOCALE_W32)&& !defined(BOOST_REGEX_NO_W32)
#define BOOST_RE_TEST_LOCALE_W32
#endif


#ifdef BOOST_RE_TEST_LOCALE_W32
typedef boost::reg_expression<char_t, boost::w32_regex_traits<char_t>, jm_debug_alloc> re_type;
#elif defined(BOOST_RE_TEST_LOCALE_CPP)
typedef boost::reg_expression<char_t, boost::cpp_regex_traits<char_t>, jm_debug_alloc> re_type;
#else
typedef boost::reg_expression<char_t, boost::c_regex_traits<char_t>, jm_debug_alloc> re_type;
#endif

#define REG_NO_POSIX_TEST 1
#define REG_UNICODE_ONLY 2
#define REG_GREP 4
#define REG_MERGE 8
#define REG_MERGE_COPY 16

#endif












