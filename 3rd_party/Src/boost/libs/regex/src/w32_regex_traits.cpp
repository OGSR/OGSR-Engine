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
  *   LOCATION:    see http://www.boost.org for most recent version.
  *   FILE:        w32_regex_traits.cpp
  *   VERSION:     see <boost/version.hpp>
  *   DESCRIPTION: Implements the w32_regex_traits<charT> traits class
  */

#define BOOST_REGEX_SOURCE

#include <clocale>
#include <cstdio>
#include <list>
#include <cctype>
#include <cstdio>
#include <boost/regex/regex_traits.hpp>
#if defined(_WIN32) && !defined(BOOST_REGEX_NO_W32)
#include <boost/cregex.hpp>
#include <boost/regex/v3/regex_synch.hpp>
#include <boost/regex/v3/regex_cstring.hpp>
#include <boost/scoped_array.hpp>


//
// VC6 needs to link to user32.lib, as do all compilers that
// claim to be VC6/7 compatible:
//
#if defined(_MSC_VER) && !defined(__BORLANDC__)
#pragma comment(lib, "user32.lib")
#endif

namespace{

//
// character classes:

boost::uint_fast32_t re_char_class_id[] = {
   boost::re_detail::w32_traits_base::char_class_alnum,
   boost::re_detail::w32_traits_base::char_class_alpha,
   boost::re_detail::w32_traits_base::char_class_cntrl,
   boost::re_detail::w32_traits_base::char_class_digit,
   boost::re_detail::w32_traits_base::char_class_graph,
   boost::re_detail::w32_traits_base::char_class_lower,
   boost::re_detail::w32_traits_base::char_class_print,
   boost::re_detail::w32_traits_base::char_class_punct,
   boost::re_detail::w32_traits_base::char_class_space,
   boost::re_detail::w32_traits_base::char_class_upper,
   boost::re_detail::w32_traits_base::char_class_xdigit,
   boost::re_detail::w32_traits_base::char_class_blank,
   boost::re_detail::w32_traits_base::char_class_word,
   boost::re_detail::w32_traits_base::char_class_unicode,
};

const char* re_char_class_names[] = {
"alnum",
"alpha",
"cntrl",
"digit",
"graph",
"lower",
"print",
"punct",
"space",
"upper",
"xdigit",
"blank",
"word",
"unicode",
};

std::string* pclasses = 0;
const unsigned int re_classes_max = 14;

//
// collate names:

struct collate_name_t
{
   std::string name;
   std::string value;
   collate_name_t(){}
   collate_name_t(const char* p1, const char* p2, const char* p3, const char* p4)
      : name(p1, p2), value(p3, p4) {}
};

std::list<collate_name_t>* pcoll_names = 0;

//
// message handling:
#ifndef BOOST_RE_MESSAGE_BASE
#define BOOST_RE_MESSAGE_BASE 0
#endif

HINSTANCE hresmod = 0;

BOOST_REGEX_DECL char* re_custom_error_messages[] = {
   0,
   0,
   0,
   0,
   0,
   0,
   0,
   0,
   0,
   0,
   0,
   0,
   0,
   0,
   0,
   0,
   0,
   0,
   0,
   0,
   0,
   0,
   0,
};

char re_zero;
char re_ten;

unsigned int entry_count = 0;
bool is_init = false;

enum syntax_map_size
{
   map_size = UCHAR_MAX + 1
};

#ifndef BOOST_NO_WREGEX

BOOST_REGEX_DECL wchar_t re_zero_w;
BOOST_REGEX_DECL wchar_t re_ten_w;

bool isPlatformNT = false;

struct syntax_map_t
{
   wchar_t c;
   unsigned int type;
};

std::list<syntax_map_t>* syntax;

#endif

std::size_t BOOST_REGEX_CALL _re_get_message(char* buf, std::size_t len, unsigned id);

std::size_t BOOST_REGEX_CALL get_message(wchar_t* buf, std::size_t len, unsigned id)
{
   std::size_t size = _re_get_message(static_cast<char*>(0), 0, id);
   if(len < size)
      return size;
   boost::scoped_array<char> cb(new char[size]);
   _re_get_message(cb.get(), size, id);
   size = boost::w32_regex_traits<wchar_t>::strwiden(buf, len, cb.get());
   return size;
}

inline std::size_t BOOST_REGEX_CALL get_message(char* buf, std::size_t len, unsigned id)
{
   return _re_get_message(buf, len, id);
}

std::size_t BOOST_REGEX_CALL _re_get_message(char* buf, std::size_t len, unsigned id)
{
   BOOST_RE_GUARD_STACK
   // get the customised message if any:
   if(len < 255)
      return 255;
   std::size_t size = 0;
   if(hresmod)
      size = LoadStringA(hresmod, BOOST_RE_MESSAGE_BASE + id, buf, 255);
   if(size)
      return size;

   //
   // now get the default message if any:
   return boost::re_detail::re_get_default_message(buf, len, id);
}

const char* BOOST_REGEX_CALL re_get_error_str(unsigned int id)
{
   BOOST_RE_GUARD_STACK
#ifdef BOOST_HAS_THREADS
   boost::re_detail::cs_guard g(*boost::re_detail::p_re_lock);
#endif
   if(re_custom_error_messages[id] == 0)
   {
      char buf[256];
      _re_get_message(buf, 256, id + 200);
      if(*buf)
      {
         re_custom_error_messages[id] = boost::re_detail::re_strdup(buf);
         return re_custom_error_messages[id];
      }
      return boost::re_detail::re_default_error_messages[id];
   }
   return re_custom_error_messages[id];
}

} // namespace

namespace boost{

namespace re_detail{

char w32_traits_base::regex_message_catalogue[BOOST_REGEX_MAX_PATH] = {0};

void BOOST_REGEX_CALL w32_traits_base::do_init()
{
   BOOST_RE_GUARD_STACK
   if(is_init == 0)
   {
      //
      // update the messages first:
      is_init = true;
      if(*regex_message_catalogue)
      {
         hresmod = LoadLibraryA(regex_message_catalogue);
#ifndef BOOST_NO_EXCEPTIONS
         if(hresmod == NULL)
         {
            std::string s("Unable to open dll: ");
            throw std::runtime_error(s + regex_message_catalogue);
         }
#else
         BOOST_REGEX_NOEH_ASSERT(hresmod != NULL);
#endif
      }
      unsigned int i;
      for(i = 0; i < REG_E_UNKNOWN; ++i)
      {
         if(re_custom_error_messages[i])
         {
            re_detail::re_strfree(re_custom_error_messages[i]);
            re_custom_error_messages[i] = 0;
         }
      }
#ifndef BOOST_NO_WREGEX
      //
      // wide character strings:
      syntax = new std::list<syntax_map_t>();
      OSVERSIONINFO VersionInformation = {0};
      VersionInformation.dwOSVersionInfoSize = sizeof(OSVERSIONINFO);
      GetVersionEx(&VersionInformation);
      if(VersionInformation.dwPlatformId == VER_PLATFORM_WIN32_NT)
         isPlatformNT = true;
#endif
      //
      // now the character classes:
      pclasses = new std::string[re_classes_max];
      char buf[map_size+2];
      for(i = 0; i < re_classes_max; ++i)
      {
         get_message(buf, 256, i+300);
         pclasses[i] = buf;
      }
      // start by updating the syntax map:
      std::memset(syntax_map, syntax_char, map_size);
      for(i = 1; i < syntax_max; ++i)
      {
         char* ptr = buf;
         get_message(buf, map_size, i+100);
         for(; *ptr; ++ptr)
         {
            syntax_map[(unsigned char)*ptr] = (unsigned char)i;
         }
      }

      // now update the character class map,
      // and lower case map:
      for(i = 0; i < map_size; ++i)
      {
         buf[i] = (char)i;
      }
      buf[map_size] = (char)0;
      GetStringTypeA(GetUserDefaultLCID(), CT_CTYPE1, buf, map_size, class_map);
      for(i = 0; i < map_size; ++i)
      {
         class_map[i] &= char_class_win;
      }
      class_map['_'] |= char_class_underscore;
      LCMapStringA(GetUserDefaultLCID(), LCMAP_LOWERCASE, buf, map_size, lower_case_map, map_size);
      //
      // update our collating elements:
      pcoll_names = new std::list<collate_name_t>();
      i = 400;
      get_message(buf, 256, i);
      while(*buf)
      {
         char* p1, *p2, *p3, *p4;;
         p1 = buf;
         while(*p1 && isspace(*p1))++p1;
         p2 = p1;
         while(*p2 && !isspace(*p2))++p2;
         p3 = p2;
         while(*p3 && isspace(*p3))++p3;
         p4 = p3;
         while(*p4 && !isspace(*p4))++p4;
         pcoll_names->push_back(collate_name_t(p1, p2, p3, p4));
         ++i;
         get_message(buf, 256, i);
      }
      std::string s;
      const char* p = "zero";
      if(w32_regex_traits<char>::lookup_collatename(s, p, p+4))
      {
         jm_assert(s.size() == 1);
         re_zero = *s.c_str();
      }
      else
         re_zero = '0';

      p = "ten";
      if(w32_regex_traits<char>::lookup_collatename(s, p, p+3))
      {
         jm_assert(s.size() == 1);
         re_ten = *s.c_str();
      }
      else
         re_ten = 'a';
#ifndef BOOST_NO_WREGEX
      //
      // wide string data:
      std::basic_string<wchar_t> ws;
      const wchar_t* wp = L"zero";
      if(w32_regex_traits<wchar_t>::lookup_collatename(ws, wp, wp+4))
      {
         jm_assert(ws.size() == 1);
         re_zero_w = *ws.c_str();
      }
      else
         re_zero_w = L'0';

      wp = L"ten";
      if(w32_regex_traits<wchar_t>::lookup_collatename(ws, wp, wp+3))
      {
         jm_assert(ws.size() == 1);
         re_ten_w = *ws.c_str();
      }
      else
         re_ten_w = L'a';

      wchar_t wbuf[256];
      syntax_map_t sm;
      syntax->clear();
      for(i = 1; i < syntax_max; ++i)
      {
         wchar_t* ptr = wbuf;
         get_message(wbuf, 256, i+100);
         for(; *ptr; ++ptr)
         {
            sm.c = *ptr;
            sm.type = i;
            syntax->push_back(sm);
         }
      }
#endif // BOOST_NO_WREGEX
   }
}

void BOOST_REGEX_CALL w32_traits_base::do_free()
{
   BOOST_RE_GUARD_STACK
   delete[] pclasses;
   pclasses = 0;
   delete pcoll_names;
   pcoll_names = 0;
#ifndef BOOST_NO_WREGEX
   delete syntax;
   syntax = 0;
#endif
   if(hresmod)
   {
      FreeLibrary(hresmod);
      hresmod = 0;
   }
   for(int i = 0; i < REG_E_UNKNOWN; ++i)
   {
      if(re_custom_error_messages[i])
      {
         re_detail::re_strfree(re_custom_error_messages[i]);
         re_custom_error_messages[i] = 0;
      }
   }
   is_init = false;
}

std::string BOOST_REGEX_CALL w32_traits_base::error_string(unsigned id)
{
   return re_get_error_str(id);
}

boost::uint_fast32_t BOOST_REGEX_CALL w32_traits_base::do_lookup_class(const char* p)
{
   BOOST_RE_GUARD_STACK
   unsigned int i;
   for(i = 0; i < re_classes_max; ++i)
   {
      if(pclasses[i] == p)
      {
         return re_char_class_id[i];
      }
   }
   for(i = 0; i < re_classes_max; ++i)
   {
      if(std::strcmp(re_char_class_names[i], p) == 0)
      {
         return re_char_class_id[i];
      }
   }
   return 0;
}

bool BOOST_REGEX_CALL w32_traits_base::do_lookup_collate(std::string& buf, const char* p)
{
   BOOST_RE_GUARD_STACK
   std::list<collate_name_t>::iterator first, last;
   first = pcoll_names->begin();
   last = pcoll_names->end();
   while(first != last)
   {
      if((*first).name == p)
      {
         buf = (*first).value;
         return true;
      }
      ++first;
   }

   bool result = re_detail::re_lookup_def_collate_name(buf, p);
   if((result == 0) && (std::strlen(p) == 1))
   {
      result = true;
      buf = *p;
   }
   return result;
}

std::string BOOST_REGEX_CALL w32_traits_base::set_message_catalogue(const std::string& l)
{
   BOOST_RE_GUARD_STACK
   #ifdef BOOST_HAS_THREADS
   re_detail::cs_guard g(*re_detail::p_re_lock);
   #endif
   if(sizeof(regex_message_catalogue) <= l.size())
      return l;
   std::string old(regex_message_catalogue);
   std::strcpy(regex_message_catalogue, l.c_str());
   return old;
}

unsigned char w32_traits_base::syntax_map[map_size];
unsigned short w32_traits_base::class_map[map_size];
char w32_traits_base::lower_case_map[map_size];

} // namespace re_detail

w32_regex_traits<char> w32_regex_traits<char>::i;

void BOOST_REGEX_CALL w32_regex_traits<char>::update()
{
   BOOST_RE_GUARD_STACK
   #ifdef BOOST_HAS_THREADS
   re_detail::cs_guard g(*re_detail::p_re_lock);
   #endif
   do_init();
}

w32_regex_traits<char>::w32_regex_traits()
{
   BOOST_RE_GUARD_STACK
   #ifdef BOOST_HAS_THREADS
   re_detail::re_init_threads();
   re_detail::cs_guard g(*re_detail::p_re_lock);
   #endif
   ++entry_count;
}

w32_regex_traits<char>::~w32_regex_traits()
{
   BOOST_RE_GUARD_STACK
#ifdef BOOST_HAS_THREADS
   re_detail::cs_guard g(*re_detail::p_re_lock);
#endif
   // add reference to static member here to ensure
   // that the linker includes it in the .exe:
   if((--entry_count == 0) && (0 != &w32_regex_traits<char>::i))
      do_free();
#ifdef BOOST_HAS_THREADS
   g.acquire(false);
   re_detail::re_free_threads();
#endif
}

void BOOST_REGEX_CALL w32_regex_traits<char>::transform(std::string& out, const std::string& in)
{
   BOOST_RE_GUARD_STACK
   size_t n = LCMapStringA(GetUserDefaultLCID(), LCMAP_SORTKEY, in.c_str(), -1, 0, 0);
   if(n == (size_t)(-1))
   {
      out = in;
      return;
   }
   scoped_array<char> buf(new char[n+1]);
   n = LCMapStringA(GetUserDefaultLCID(), LCMAP_SORTKEY, in.c_str(), -1, buf.get(), (int)n);
   if(n == (size_t)(-1))
   {
      out = in;
      return;
   }
   out = buf.get();
}

void BOOST_REGEX_CALL w32_regex_traits<char>::transform_primary(std::string& out, const std::string& in)
{
   transform(out, in);
   for(unsigned int i = 0; i < out.size(); ++i)
   {
      if((out[i] == 1) && (i+1 < out.size()))
      {
         out.erase(i+1);
         break;
      }
   }
}


int BOOST_REGEX_CALL w32_regex_traits<char>::toi(char c)
{
   if(is_class(c, char_class_digit))
      return c - re_zero;
   if(is_class(c, char_class_xdigit))
      return 10 + translate(c, true) - translate(re_ten, true);
   return -1; // error!!
}

int BOOST_REGEX_CALL w32_regex_traits<char>::toi(const char*& first, const char* last, int radix)
{
   unsigned int maxval;
   if(radix < 0)
   {
      // if radix is less than zero, then restrict
      // return value to charT. NB assumes sizeof(charT) <= sizeof(int)
      radix *= -1;
      maxval = 1 << (sizeof(*first) * CHAR_BIT - 1);
      maxval /= radix;
      maxval *= 2;
      maxval -= 1;
   }
   else
   {
      maxval = (unsigned int)-1;
      maxval /= radix;
   }

   unsigned int result = 0;
   unsigned int type = (radix > 10) ? char_class_xdigit : char_class_digit;
   while((first != last) && is_class(*first, type) && (result <= maxval))
   {
      result *= radix;
      result += toi(*first);
      ++first;
   }
   return result;
}

#ifndef BOOST_NO_WREGEX

bool BOOST_REGEX_CALL w32_regex_traits<wchar_t>::lookup_collatename(std::basic_string<wchar_t>& out, const wchar_t* first, const wchar_t* last)
{
   BOOST_RE_GUARD_STACK
   std::basic_string<wchar_t> s(first, last);
   std::size_t len = strnarrow(static_cast<char*>(0), 0, s.c_str());
   scoped_array<char> buf(new char[len]);
   strnarrow(buf.get(), len, s.c_str());
   std::string t_out;
   bool result = base_type::do_lookup_collate(t_out, buf.get());
   if(t_out.size() == 0) result = false;
   if(result)
   {
      if(t_out[0])
      {
         len = strwiden(static_cast<wchar_t*>(0), 0, t_out.c_str());
         scoped_array<wchar_t> wb(new wchar_t[len]);
         strwiden(wb.get(), len, t_out.c_str());
         out = wb.get();
      }
      else
         out.append(1,(wchar_t)0);
   }
   return result;
}

unsigned int BOOST_REGEX_CALL w32_regex_traits<wchar_t>::syntax_type(size_type c)
{
   BOOST_RE_GUARD_STACK
   std::list<syntax_map_t>::const_iterator first, last;
   first = syntax->begin();
   last = syntax->end();
   while(first != last)
   {
      if((size_type)(uchar_type)((*first).c) == c)
         return (*first).type;
      ++first;
   }
   return 0;
}

bool BOOST_REGEX_CALL w32_regex_traits<wchar_t>::do_lookup_collate(std::basic_string<wchar_t>& out, const wchar_t* first, const wchar_t* last)
{
   BOOST_RE_GUARD_STACK
   std::basic_string<wchar_t> s(first, last);
   std::size_t len = strnarrow(static_cast<char*>(0), 0, s.c_str());
   scoped_array<char> buf(new char[len]);
   strnarrow(buf.get(), len, s.c_str());
   std::string t_out;
   bool result =  base_type::do_lookup_collate(t_out, buf.get());
   if(result)
   {
      len = strwiden(static_cast<wchar_t*>(0), 0, t_out.c_str());
      scoped_array<wchar_t> wb(new wchar_t[len]);
      strwiden(wb.get(), len, t_out.c_str());
      out = wb.get();
   }
   return result;
}


void BOOST_REGEX_CALL w32_regex_traits<wchar_t>::update()
{
   BOOST_RE_GUARD_STACK
#ifdef BOOST_HAS_THREADS
   re_detail::cs_guard g(*re_detail::p_re_lock);
#endif
   do_init();
}

w32_regex_traits<wchar_t>::w32_regex_traits()
{
   BOOST_RE_GUARD_STACK
#ifdef BOOST_HAS_THREADS
   re_detail::re_init_threads();
   re_detail::cs_guard g(*re_detail::p_re_lock);
#endif
   ++entry_count;
}

w32_regex_traits<wchar_t>::~w32_regex_traits()
{
   BOOST_RE_GUARD_STACK
#ifdef BOOST_HAS_THREADS
   re_detail::cs_guard g(*re_detail::p_re_lock);
#endif
   // add reference to static member here to ensure
   // that the linker includes it in the .exe:
   if((--entry_count == 0) && (0 != &w32_regex_traits<wchar_t>::init_))
      do_free();
#ifdef BOOST_HAS_THREADS
   g.acquire(false);
   re_detail::re_free_threads();
#endif
}

bool BOOST_REGEX_CALL w32_regex_traits<wchar_t>::do_iswclass(wchar_t c, boost::uint_fast32_t f)
{
   BOOST_RE_GUARD_STACK
   if((c & ~0xFF) == 0)
      return BOOST_REGEX_MAKE_BOOL(re_detail::wide_unicode_classes[(uchar_type)c] & f & char_class_win);
   WORD mask;
   if(f & char_class_unicode)
      return true;
   else if(isPlatformNT && GetStringTypeW(CT_CTYPE1, &c, 1, &mask))
      return BOOST_REGEX_MAKE_BOOL(mask & f & char_class_win);
   else if((f & char_class_graph) == char_class_graph)
      return true;  // all wide characters are considered "graphics"
   return false;
}

void BOOST_REGEX_CALL w32_regex_traits<wchar_t>::transform(std::basic_string<wchar_t>& out, const std::basic_string<wchar_t>& in)
{
   BOOST_RE_GUARD_STACK
   scoped_array<char> alt;
   size_t n;
   if(isPlatformNT)
      n = LCMapStringW(GetUserDefaultLCID(), LCMAP_SORTKEY, in.c_str(), -1, 0, 0);
   else
   {
      n = strnarrow(static_cast<char*>(0), 0, in.c_str());
      alt.reset(new char[n+1]);
      strnarrow(alt.get(), n+1, in.c_str());
      n = LCMapStringA(GetUserDefaultLCID(), LCMAP_SORTKEY, alt.get(), -1, 0, 0);
   }
   if((n == (size_t)(-1)) || (n == 0))
   {
      out = in;
      return;
   }
   scoped_array<wchar_t> buf(new wchar_t[n+1]);
   // under win32 we get mapped to an array of bytes
   // not characters; since the underlying engine has to
   // deal with chars we widen the bytes to wchar_t to ensure
   // the sort order remains unchanged when we compare.
   scoped_array<char> t(new char[n+1]);
   if(isPlatformNT)
      n = LCMapStringW(GetUserDefaultLCID(), LCMAP_SORTKEY, in.c_str(), -1, reinterpret_cast<wchar_t*>(t.get()), (int)n);
   else
      n = LCMapStringA(GetUserDefaultLCID(), LCMAP_SORTKEY, alt.get(), -1, t.get(), (int)n);
   int i = -1;
   do
   {
      ++i;
      buf[i] = (wchar_t)(unsigned char)t[i];
   } while(t[i]);
   if(n == (size_t)(-1))
   {
      out = in;
      return;
   }
   out = buf.get();
}

void BOOST_REGEX_CALL w32_regex_traits<wchar_t>::transform_primary(std::basic_string<wchar_t>& out, const std::basic_string<wchar_t>& in)
{
   transform(out, in);
   for(unsigned int i = 0; i < out.size(); ++i)
   {
      if((out[i] == 1) && ((i + 1) < out.size()))
      {
         out.erase(i+1);
         break;
      }
   }
}


int BOOST_REGEX_CALL w32_regex_traits<wchar_t>::toi(wchar_t c)
{
   if(is_class(c, char_class_digit))
      return c - re_zero_w;
   if(is_class(c, char_class_xdigit))
      return 10 + translate(c, true) - translate(re_ten_w, true);
   return -1; // error!!
}

int BOOST_REGEX_CALL w32_regex_traits<wchar_t>::toi(const wchar_t*& first, const wchar_t* last, int radix)
{
   unsigned int maxval;
   if(radix < 0)
   {
      // if radix is less than zero, then restrict
      // return value to charT. NB assumes sizeof(charT) <= sizeof(int)
      radix *= -1;
      maxval = 1 << (sizeof(*first) * CHAR_BIT - 1);
      maxval /= radix;
      maxval *= 2;
      maxval -= 1;
   }
   else
   {
      maxval = (unsigned int)-1;
      maxval /= radix;
   }

   unsigned int result = 0;
   unsigned int type = (radix > 10) ? char_class_xdigit : char_class_digit;
   while((first != last) && is_class(*first, type) && (result <= maxval))
   {
      result *= radix;
      result += toi(*first);
      ++first;
   }
   return result;
}

boost::uint_fast32_t BOOST_REGEX_CALL w32_regex_traits<wchar_t>::lookup_classname(const wchar_t* first, const wchar_t* last)
{
   std::basic_string<wchar_t> s(first, last);
   std::size_t len = strnarrow(static_cast<char*>(0), 0, s.c_str());
   scoped_array<char> buf(new char[len]);
   strnarrow(buf.get(), len, s.c_str());
   boost::uint_fast32_t result =  do_lookup_class(buf.get());
   return result;
}

wchar_t BOOST_REGEX_CALL w32_regex_traits<wchar_t>::wtolower(wchar_t c)
{
   BOOST_RE_GUARD_STACK
   if(isPlatformNT)
      return LOWORD(CharLowerW(reinterpret_cast<wchar_t*>(static_cast<unsigned short>(c))));
   return c;
}


w32_regex_traits<wchar_t> w32_regex_traits<wchar_t>::init_;

std::size_t BOOST_REGEX_CALL w32_regex_traits<wchar_t>::strnarrow(char *s1, std::size_t len, const wchar_t *s2)
{
   BOOST_RE_GUARD_STACK
   std::size_t size = WideCharToMultiByte(CP_ACP, 0, s2, -1, s1, 0, 0, 0);
   if(size > len)
      return size;
   return WideCharToMultiByte(CP_ACP, 0, s2, -1, s1, (int)len, 0, 0);
}

std::size_t BOOST_REGEX_CALL w32_regex_traits<wchar_t>::strwiden(wchar_t *s1, std::size_t len, const char *s2)
{
   BOOST_RE_GUARD_STACK
   std::size_t size = MultiByteToWideChar(CP_ACP, MB_PRECOMPOSED,   s2, -1, s1, 0);
   if(size > len)
      return size;
   return MultiByteToWideChar(CP_ACP, MB_PRECOMPOSED, s2, -1, s1, (int)len);
}

unsigned short w32_regex_traits<wchar_t>::wide_unicode_classes[] = {
   re_detail::w32_traits_base::char_class_cntrl,        // ''  0
   re_detail::w32_traits_base::char_class_cntrl,        // ''  1
   re_detail::w32_traits_base::char_class_cntrl,        // ''  2
   re_detail::w32_traits_base::char_class_cntrl,        // ''  3
   re_detail::w32_traits_base::char_class_cntrl,        // ''  4
   re_detail::w32_traits_base::char_class_cntrl,        // ''  5
   re_detail::w32_traits_base::char_class_cntrl,        // ''  6
   re_detail::w32_traits_base::char_class_cntrl,        // ''  7
   re_detail::w32_traits_base::char_class_cntrl,        // ''  8
   re_detail::w32_traits_base::char_class_cntrl | re_detail::w32_traits_base::char_class_space | re_detail::w32_traits_base::char_class_blank,        // ''  9
   re_detail::w32_traits_base::char_class_cntrl | re_detail::w32_traits_base::char_class_space,        // ''  10
   re_detail::w32_traits_base::char_class_cntrl | re_detail::w32_traits_base::char_class_space,        // ''  11
   re_detail::w32_traits_base::char_class_cntrl | re_detail::w32_traits_base::char_class_space,        // ''  12
   re_detail::w32_traits_base::char_class_cntrl | re_detail::w32_traits_base::char_class_space,        // ''  13
   re_detail::w32_traits_base::char_class_cntrl,        // '.'  14
   re_detail::w32_traits_base::char_class_cntrl,        // '.'  15
   re_detail::w32_traits_base::char_class_cntrl,        // '.'  16
   re_detail::w32_traits_base::char_class_cntrl,        // '.'  17
   re_detail::w32_traits_base::char_class_cntrl,        // '.'  18
   re_detail::w32_traits_base::char_class_cntrl,        // '.'  19
   re_detail::w32_traits_base::char_class_cntrl,        // '.'  20
   re_detail::w32_traits_base::char_class_cntrl,        // '.'  21
   re_detail::w32_traits_base::char_class_cntrl,        // '.'  22
   re_detail::w32_traits_base::char_class_cntrl,        // '.'  23
   re_detail::w32_traits_base::char_class_cntrl,        // '.'  24
   re_detail::w32_traits_base::char_class_cntrl,        // ''  25
   re_detail::w32_traits_base::char_class_cntrl,        // ''  26
   re_detail::w32_traits_base::char_class_cntrl,        // ''  27
   re_detail::w32_traits_base::char_class_cntrl,        // '.'  28
   re_detail::w32_traits_base::char_class_cntrl,        // '.'  29
   re_detail::w32_traits_base::char_class_cntrl,        // '.'  30
   re_detail::w32_traits_base::char_class_cntrl,        // '.'  31
   re_detail::w32_traits_base::char_class_space | re_detail::w32_traits_base::char_class_blank,        // ' '  32
   re_detail::w32_traits_base::char_class_punct,        // '!'  33
   re_detail::w32_traits_base::char_class_punct,        // '"'  34
   re_detail::w32_traits_base::char_class_punct,        // '#'  35
   re_detail::w32_traits_base::char_class_punct,        // '$'  36
   re_detail::w32_traits_base::char_class_punct,        // '%'  37
   re_detail::w32_traits_base::char_class_punct,        // '&'  38
   re_detail::w32_traits_base::char_class_punct,        // '''  39
   re_detail::w32_traits_base::char_class_punct,        // '('  40
   re_detail::w32_traits_base::char_class_punct,        // ')'  41
   re_detail::w32_traits_base::char_class_punct,        // '*'  42
   re_detail::w32_traits_base::char_class_punct,        // '+'  43
   re_detail::w32_traits_base::char_class_punct,        // ','  44
   re_detail::w32_traits_base::char_class_punct,        // '-'  45
   re_detail::w32_traits_base::char_class_punct,        // '.'  46
   re_detail::w32_traits_base::char_class_punct,        // '/'  47
   re_detail::w32_traits_base::char_class_digit |   re_detail::w32_traits_base::char_class_xdigit,        // '0'  48
   re_detail::w32_traits_base::char_class_digit |   re_detail::w32_traits_base::char_class_xdigit,        // '1'  49
   re_detail::w32_traits_base::char_class_digit |   re_detail::w32_traits_base::char_class_xdigit,        // '2'  50
   re_detail::w32_traits_base::char_class_digit |   re_detail::w32_traits_base::char_class_xdigit,        // '3'  51
   re_detail::w32_traits_base::char_class_digit |   re_detail::w32_traits_base::char_class_xdigit,        // '4'  52
   re_detail::w32_traits_base::char_class_digit |   re_detail::w32_traits_base::char_class_xdigit,        // '5'  53
   re_detail::w32_traits_base::char_class_digit |   re_detail::w32_traits_base::char_class_xdigit,        // '6'  54
   re_detail::w32_traits_base::char_class_digit |   re_detail::w32_traits_base::char_class_xdigit,        // '7'  55
   re_detail::w32_traits_base::char_class_digit |   re_detail::w32_traits_base::char_class_xdigit,        // '8'  56
   re_detail::w32_traits_base::char_class_digit |   re_detail::w32_traits_base::char_class_xdigit,        // '9'  57
   re_detail::w32_traits_base::char_class_punct,        // ':'  58
   re_detail::w32_traits_base::char_class_punct,        // ';'  59
   re_detail::w32_traits_base::char_class_punct,        // '<'  60
   re_detail::w32_traits_base::char_class_punct,        // '='  61
   re_detail::w32_traits_base::char_class_punct,        // '>'  62
   re_detail::w32_traits_base::char_class_punct,        // '?'  63
   re_detail::w32_traits_base::char_class_punct,        // '@'  64
   re_detail::w32_traits_base::char_class_alpha |   re_detail::w32_traits_base::char_class_upper | re_detail::w32_traits_base::char_class_xdigit,        // 'A'  65
   re_detail::w32_traits_base::char_class_alpha |   re_detail::w32_traits_base::char_class_upper | re_detail::w32_traits_base::char_class_xdigit,        // 'B'  66
   re_detail::w32_traits_base::char_class_alpha |   re_detail::w32_traits_base::char_class_upper | re_detail::w32_traits_base::char_class_xdigit,        // 'C'  67
   re_detail::w32_traits_base::char_class_alpha |   re_detail::w32_traits_base::char_class_upper | re_detail::w32_traits_base::char_class_xdigit,        // 'D'  68
   re_detail::w32_traits_base::char_class_alpha |   re_detail::w32_traits_base::char_class_upper | re_detail::w32_traits_base::char_class_xdigit,        // 'E'  69
   re_detail::w32_traits_base::char_class_alpha |   re_detail::w32_traits_base::char_class_upper | re_detail::w32_traits_base::char_class_xdigit,        // 'F'  70
   re_detail::w32_traits_base::char_class_alpha |   re_detail::w32_traits_base::char_class_upper,        // 'G'  71
   re_detail::w32_traits_base::char_class_alpha |   re_detail::w32_traits_base::char_class_upper,        // 'H'  72
   re_detail::w32_traits_base::char_class_alpha |   re_detail::w32_traits_base::char_class_upper,        // 'I'  73
   re_detail::w32_traits_base::char_class_alpha |   re_detail::w32_traits_base::char_class_upper,        // 'J'  74
   re_detail::w32_traits_base::char_class_alpha |   re_detail::w32_traits_base::char_class_upper,        // 'K'  75
   re_detail::w32_traits_base::char_class_alpha |   re_detail::w32_traits_base::char_class_upper,        // 'L'  76
   re_detail::w32_traits_base::char_class_alpha |   re_detail::w32_traits_base::char_class_upper,        // 'M'  77
   re_detail::w32_traits_base::char_class_alpha |   re_detail::w32_traits_base::char_class_upper,        // 'N'  78
   re_detail::w32_traits_base::char_class_alpha |   re_detail::w32_traits_base::char_class_upper,        // 'O'  79
   re_detail::w32_traits_base::char_class_alpha |   re_detail::w32_traits_base::char_class_upper,        // 'P'  80
   re_detail::w32_traits_base::char_class_alpha |   re_detail::w32_traits_base::char_class_upper,        // 'Q'  81
   re_detail::w32_traits_base::char_class_alpha |   re_detail::w32_traits_base::char_class_upper,        // 'R'  82
   re_detail::w32_traits_base::char_class_alpha |   re_detail::w32_traits_base::char_class_upper,        // 'S'  83
   re_detail::w32_traits_base::char_class_alpha |   re_detail::w32_traits_base::char_class_upper,        // 'T'  84
   re_detail::w32_traits_base::char_class_alpha |   re_detail::w32_traits_base::char_class_upper,        // 'U'  85
   re_detail::w32_traits_base::char_class_alpha |   re_detail::w32_traits_base::char_class_upper,        // 'V'  86
   re_detail::w32_traits_base::char_class_alpha |   re_detail::w32_traits_base::char_class_upper,        // 'W'  87
   re_detail::w32_traits_base::char_class_alpha |   re_detail::w32_traits_base::char_class_upper,        // 'X'  88
   re_detail::w32_traits_base::char_class_alpha |   re_detail::w32_traits_base::char_class_upper,        // 'Y'  89
   re_detail::w32_traits_base::char_class_alpha |   re_detail::w32_traits_base::char_class_upper,        // 'Z'  90
   re_detail::w32_traits_base::char_class_punct,        // '['  91
   re_detail::w32_traits_base::char_class_punct,        // '\'  92
   re_detail::w32_traits_base::char_class_punct,        // ']'  93
   re_detail::w32_traits_base::char_class_punct,        // '^'  94
   re_detail::w32_traits_base::char_class_punct | re_detail::w32_traits_base::char_class_underscore,        // '_'  95
   re_detail::w32_traits_base::char_class_punct,        // '`'  96
   re_detail::w32_traits_base::char_class_alpha |  re_detail::w32_traits_base::char_class_lower |  re_detail::w32_traits_base::char_class_xdigit,        // 'a'  97
   re_detail::w32_traits_base::char_class_alpha |  re_detail::w32_traits_base::char_class_lower |  re_detail::w32_traits_base::char_class_xdigit,        // 'b'  98
   re_detail::w32_traits_base::char_class_alpha |  re_detail::w32_traits_base::char_class_lower |  re_detail::w32_traits_base::char_class_xdigit,        // 'c'  99
   re_detail::w32_traits_base::char_class_alpha |  re_detail::w32_traits_base::char_class_lower |  re_detail::w32_traits_base::char_class_xdigit,        // 'd'  100
   re_detail::w32_traits_base::char_class_alpha |  re_detail::w32_traits_base::char_class_lower |  re_detail::w32_traits_base::char_class_xdigit,        // 'e'  101
   re_detail::w32_traits_base::char_class_alpha |  re_detail::w32_traits_base::char_class_lower |  re_detail::w32_traits_base::char_class_xdigit,        // 'f'  102
   re_detail::w32_traits_base::char_class_alpha |  re_detail::w32_traits_base::char_class_lower,        // 'g'  103
   re_detail::w32_traits_base::char_class_alpha |  re_detail::w32_traits_base::char_class_lower,        // 'h'  104
   re_detail::w32_traits_base::char_class_alpha |  re_detail::w32_traits_base::char_class_lower,        // 'i'  105
   re_detail::w32_traits_base::char_class_alpha |  re_detail::w32_traits_base::char_class_lower,        // 'j'  106
   re_detail::w32_traits_base::char_class_alpha |  re_detail::w32_traits_base::char_class_lower,        // 'k'  107
   re_detail::w32_traits_base::char_class_alpha |  re_detail::w32_traits_base::char_class_lower,        // 'l'  108
   re_detail::w32_traits_base::char_class_alpha |  re_detail::w32_traits_base::char_class_lower,        // 'm'  109
   re_detail::w32_traits_base::char_class_alpha |  re_detail::w32_traits_base::char_class_lower,        // 'n'  110
   re_detail::w32_traits_base::char_class_alpha |  re_detail::w32_traits_base::char_class_lower,        // 'o'  111
   re_detail::w32_traits_base::char_class_alpha |  re_detail::w32_traits_base::char_class_lower,        // 'p'  112
   re_detail::w32_traits_base::char_class_alpha |  re_detail::w32_traits_base::char_class_lower,        // 'q'  113
   re_detail::w32_traits_base::char_class_alpha |  re_detail::w32_traits_base::char_class_lower,        // 'r'  114
   re_detail::w32_traits_base::char_class_alpha |  re_detail::w32_traits_base::char_class_lower,        // 's'  115
   re_detail::w32_traits_base::char_class_alpha |  re_detail::w32_traits_base::char_class_lower,        // 't'  116
   re_detail::w32_traits_base::char_class_alpha |  re_detail::w32_traits_base::char_class_lower,        // 'u'  117
   re_detail::w32_traits_base::char_class_alpha |  re_detail::w32_traits_base::char_class_lower,        // 'v'  118
   re_detail::w32_traits_base::char_class_alpha |  re_detail::w32_traits_base::char_class_lower,        // 'w'  119
   re_detail::w32_traits_base::char_class_alpha |  re_detail::w32_traits_base::char_class_lower,        // 'x'  120
   re_detail::w32_traits_base::char_class_alpha |  re_detail::w32_traits_base::char_class_lower,        // 'y'  121
   re_detail::w32_traits_base::char_class_alpha |  re_detail::w32_traits_base::char_class_lower,        // 'z'  122
   re_detail::w32_traits_base::char_class_punct,        // '{'  123
   re_detail::w32_traits_base::char_class_punct,        // '|'  124
   re_detail::w32_traits_base::char_class_punct,        // '}'  125
   re_detail::w32_traits_base::char_class_punct,        // '~'  126

   re_detail::w32_traits_base::char_class_cntrl,        // ''  127
   re_detail::w32_traits_base::char_class_cntrl,        // '€'  128
   re_detail::w32_traits_base::char_class_cntrl,        // ''  129
   re_detail::w32_traits_base::char_class_cntrl,        // '‚'  130
   re_detail::w32_traits_base::char_class_cntrl,        // 'ƒ'  131
   re_detail::w32_traits_base::char_class_cntrl,        // '„'  132
   re_detail::w32_traits_base::char_class_cntrl,        // '…'  133
   re_detail::w32_traits_base::char_class_cntrl,        // '†'  134
   re_detail::w32_traits_base::char_class_cntrl,        // '‡'  135
   re_detail::w32_traits_base::char_class_cntrl,        // 'ˆ'  136
   re_detail::w32_traits_base::char_class_cntrl,        // '‰'  137
   re_detail::w32_traits_base::char_class_cntrl,        // 'Š'  138
   re_detail::w32_traits_base::char_class_cntrl,        // '‹'  139
   re_detail::w32_traits_base::char_class_cntrl,        // 'Œ'  140
   re_detail::w32_traits_base::char_class_cntrl,        // ''  141
   re_detail::w32_traits_base::char_class_cntrl,        // ''  142
   re_detail::w32_traits_base::char_class_cntrl,        // ''  143
   re_detail::w32_traits_base::char_class_cntrl,        // ''  144
   re_detail::w32_traits_base::char_class_cntrl,        // '‘'  145
   re_detail::w32_traits_base::char_class_cntrl,        // '’'  146
   re_detail::w32_traits_base::char_class_cntrl,        // '“'  147
   re_detail::w32_traits_base::char_class_cntrl,        // '”'  148
   re_detail::w32_traits_base::char_class_cntrl,        // '•'  149
   re_detail::w32_traits_base::char_class_cntrl,        // '–'  150
   re_detail::w32_traits_base::char_class_cntrl,        // '—'  151
   re_detail::w32_traits_base::char_class_cntrl,        // '˜'  152
   re_detail::w32_traits_base::char_class_cntrl,        // '™'  153
   re_detail::w32_traits_base::char_class_cntrl,        // 'š'  154
   re_detail::w32_traits_base::char_class_cntrl,        // '›'  155
   re_detail::w32_traits_base::char_class_cntrl,        // 'œ'  156
   re_detail::w32_traits_base::char_class_cntrl,        // ''  157
   re_detail::w32_traits_base::char_class_cntrl,        // ''  158
   re_detail::w32_traits_base::char_class_cntrl,        // 'Ÿ'  159
   re_detail::w32_traits_base::char_class_space | re_detail::w32_traits_base::char_class_blank,        // ' '  160
   re_detail::w32_traits_base::char_class_punct,        // '¡'  161
   re_detail::w32_traits_base::char_class_punct,        // '¢'  162
   re_detail::w32_traits_base::char_class_punct,        // '£'  163
   re_detail::w32_traits_base::char_class_punct,        // '¤'  164
   re_detail::w32_traits_base::char_class_punct,        // '¥'  165
   re_detail::w32_traits_base::char_class_punct,        // '¦'  166
   re_detail::w32_traits_base::char_class_punct,        // '§'  167
   re_detail::w32_traits_base::char_class_punct,        // '¨'  168
   re_detail::w32_traits_base::char_class_punct,        // '©'  169
   re_detail::w32_traits_base::char_class_punct,        // 'ª'  170
   re_detail::w32_traits_base::char_class_punct,        // '«'  171
   re_detail::w32_traits_base::char_class_punct,        // '¬'  172
   re_detail::w32_traits_base::char_class_punct,        // '­'  173
   re_detail::w32_traits_base::char_class_punct,        // '®'  174
   re_detail::w32_traits_base::char_class_punct,        // '¯'  175
   re_detail::w32_traits_base::char_class_punct,        // '°'  176
   re_detail::w32_traits_base::char_class_punct,        // '±'  177
   re_detail::w32_traits_base::char_class_punct,        // '²'  178
   re_detail::w32_traits_base::char_class_punct,        // '³'  179
   re_detail::w32_traits_base::char_class_punct,        // '´'  180
   re_detail::w32_traits_base::char_class_punct,        // 'µ'  181
   re_detail::w32_traits_base::char_class_punct,        // '¶'  182
   re_detail::w32_traits_base::char_class_punct,        // '·'  183
   re_detail::w32_traits_base::char_class_punct,        // '¸'  184
   re_detail::w32_traits_base::char_class_punct,        // '¹'  185
   re_detail::w32_traits_base::char_class_punct,        // 'º'  186
   re_detail::w32_traits_base::char_class_punct,        // '»'  187
   re_detail::w32_traits_base::char_class_punct,        // '¼'  188
   re_detail::w32_traits_base::char_class_punct,        // '½'  189
   re_detail::w32_traits_base::char_class_punct,        // '¾'  190
   re_detail::w32_traits_base::char_class_punct,        // '¿'  191
   re_detail::w32_traits_base::char_class_alpha |   re_detail::w32_traits_base::char_class_upper,        // 'À'  192
   re_detail::w32_traits_base::char_class_alpha |   re_detail::w32_traits_base::char_class_upper,        // 'Á'  193
   re_detail::w32_traits_base::char_class_alpha |   re_detail::w32_traits_base::char_class_upper,        // 'Â'  194
   re_detail::w32_traits_base::char_class_alpha |   re_detail::w32_traits_base::char_class_upper,        // 'Ã'  195
   re_detail::w32_traits_base::char_class_alpha |   re_detail::w32_traits_base::char_class_upper,        // 'Ä'  196
   re_detail::w32_traits_base::char_class_alpha |   re_detail::w32_traits_base::char_class_upper,        // 'Å'  197
   re_detail::w32_traits_base::char_class_alpha |   re_detail::w32_traits_base::char_class_upper,        // 'Æ'  198
   re_detail::w32_traits_base::char_class_alpha |   re_detail::w32_traits_base::char_class_upper,        // 'Ç'  199
   re_detail::w32_traits_base::char_class_alpha |   re_detail::w32_traits_base::char_class_upper,        // 'È'  200
   re_detail::w32_traits_base::char_class_alpha |   re_detail::w32_traits_base::char_class_upper,        // 'É'  201
   re_detail::w32_traits_base::char_class_alpha |   re_detail::w32_traits_base::char_class_upper,        // 'Ê'  202
   re_detail::w32_traits_base::char_class_alpha |   re_detail::w32_traits_base::char_class_upper,        // 'Ë'  203
   re_detail::w32_traits_base::char_class_alpha |   re_detail::w32_traits_base::char_class_upper,        // 'Ì'  204
   re_detail::w32_traits_base::char_class_alpha |   re_detail::w32_traits_base::char_class_upper,        // 'Í'  205
   re_detail::w32_traits_base::char_class_alpha |   re_detail::w32_traits_base::char_class_upper,        // 'Î'  206
   re_detail::w32_traits_base::char_class_alpha |   re_detail::w32_traits_base::char_class_upper,        // 'Ï'  207
   re_detail::w32_traits_base::char_class_alpha |   re_detail::w32_traits_base::char_class_upper,        // 'Ğ'  208
   re_detail::w32_traits_base::char_class_alpha |   re_detail::w32_traits_base::char_class_upper,        // 'Ñ'  209
   re_detail::w32_traits_base::char_class_alpha |   re_detail::w32_traits_base::char_class_upper,        // 'Ò'  210
   re_detail::w32_traits_base::char_class_alpha |   re_detail::w32_traits_base::char_class_upper,        // 'Ó'  211
   re_detail::w32_traits_base::char_class_alpha |   re_detail::w32_traits_base::char_class_upper,        // 'Ô'  212
   re_detail::w32_traits_base::char_class_alpha |   re_detail::w32_traits_base::char_class_upper,        // 'Õ'  213
   re_detail::w32_traits_base::char_class_alpha |   re_detail::w32_traits_base::char_class_upper,        // 'Ö'  214
   re_detail::w32_traits_base::char_class_punct,        // '×'  215
   re_detail::w32_traits_base::char_class_alpha |   re_detail::w32_traits_base::char_class_upper,        // 'Ø'  216
   re_detail::w32_traits_base::char_class_alpha |   re_detail::w32_traits_base::char_class_upper,        // 'Ù'  217
   re_detail::w32_traits_base::char_class_alpha |   re_detail::w32_traits_base::char_class_upper,        // 'Ú'  218
   re_detail::w32_traits_base::char_class_alpha |   re_detail::w32_traits_base::char_class_upper,        // 'Û'  219
   re_detail::w32_traits_base::char_class_alpha |   re_detail::w32_traits_base::char_class_upper,        // 'Ü'  220
   re_detail::w32_traits_base::char_class_alpha |   re_detail::w32_traits_base::char_class_upper,        // 'İ'  221
   re_detail::w32_traits_base::char_class_alpha |  re_detail::w32_traits_base::char_class_upper,        // 'Ş'  222
   re_detail::w32_traits_base::char_class_alpha |  re_detail::w32_traits_base::char_class_lower,        // 'ß'  223
   re_detail::w32_traits_base::char_class_alpha |  re_detail::w32_traits_base::char_class_lower,        // 'à'  224
   re_detail::w32_traits_base::char_class_alpha |  re_detail::w32_traits_base::char_class_lower,        // 'á'  225
   re_detail::w32_traits_base::char_class_alpha |  re_detail::w32_traits_base::char_class_lower,        // 'â'  226
   re_detail::w32_traits_base::char_class_alpha |  re_detail::w32_traits_base::char_class_lower,        // 'ã'  227
   re_detail::w32_traits_base::char_class_alpha |  re_detail::w32_traits_base::char_class_lower,        // 'ä'  228
   re_detail::w32_traits_base::char_class_alpha |  re_detail::w32_traits_base::char_class_lower,        // 'å'  229
   re_detail::w32_traits_base::char_class_alpha |  re_detail::w32_traits_base::char_class_lower,        // 'æ'  230
   re_detail::w32_traits_base::char_class_alpha |  re_detail::w32_traits_base::char_class_lower,        // 'ç'  231
   re_detail::w32_traits_base::char_class_alpha |  re_detail::w32_traits_base::char_class_lower,        // 'è'  232
   re_detail::w32_traits_base::char_class_alpha |  re_detail::w32_traits_base::char_class_lower,        // 'é'  233
   re_detail::w32_traits_base::char_class_alpha |  re_detail::w32_traits_base::char_class_lower,        // 'ê'  234
   re_detail::w32_traits_base::char_class_alpha |  re_detail::w32_traits_base::char_class_lower,        // 'ë'  235
   re_detail::w32_traits_base::char_class_alpha |  re_detail::w32_traits_base::char_class_lower,        // 'ì'  236
   re_detail::w32_traits_base::char_class_alpha |  re_detail::w32_traits_base::char_class_lower,        // 'í'  237
   re_detail::w32_traits_base::char_class_alpha |  re_detail::w32_traits_base::char_class_lower,        // 'î'  238
   re_detail::w32_traits_base::char_class_alpha |  re_detail::w32_traits_base::char_class_lower,        // 'ï'  239
   re_detail::w32_traits_base::char_class_alpha |  re_detail::w32_traits_base::char_class_lower,        // 'ğ'  240
   re_detail::w32_traits_base::char_class_alpha |  re_detail::w32_traits_base::char_class_lower,        // 'ñ'  241
   re_detail::w32_traits_base::char_class_alpha |  re_detail::w32_traits_base::char_class_lower,        // 'ò'  242
   re_detail::w32_traits_base::char_class_alpha |  re_detail::w32_traits_base::char_class_lower,        // 'ó'  243
   re_detail::w32_traits_base::char_class_alpha |  re_detail::w32_traits_base::char_class_lower,        // 'ô'  244
   re_detail::w32_traits_base::char_class_alpha |  re_detail::w32_traits_base::char_class_lower,        // 'õ'  245
   re_detail::w32_traits_base::char_class_alpha |  re_detail::w32_traits_base::char_class_lower,        // 'ö'  246
   re_detail::w32_traits_base::char_class_punct,        // '÷'  247
   re_detail::w32_traits_base::char_class_alpha |  re_detail::w32_traits_base::char_class_lower,        // 'ø'  248
   re_detail::w32_traits_base::char_class_alpha |  re_detail::w32_traits_base::char_class_lower,        // 'ù'  249
   re_detail::w32_traits_base::char_class_alpha |  re_detail::w32_traits_base::char_class_lower,        // 'ú'  250
   re_detail::w32_traits_base::char_class_alpha |  re_detail::w32_traits_base::char_class_lower,        // 'û'  251
   re_detail::w32_traits_base::char_class_alpha |  re_detail::w32_traits_base::char_class_lower,        // 'ü'  252
   re_detail::w32_traits_base::char_class_alpha |  re_detail::w32_traits_base::char_class_lower,        // 'ı'  253
   re_detail::w32_traits_base::char_class_alpha |  re_detail::w32_traits_base::char_class_lower,        // 'ş'  254
   re_detail::w32_traits_base::char_class_alpha |  re_detail::w32_traits_base::char_class_lower,        // 'ÿ'  255
};



#endif // BOOST_NO_WREGEX


} // namespace boost

#endif // #if defined(_WIN32) && !defined(BOOST_REGEX_NO_W32)




