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
  *   FILE         c_regex_traits.cpp
  *   VERSION      see <boost/version.hpp>
  *   DESCRIPTION: Implements the c_regex_traits<charT> traits class
  */

#define BOOST_REGEX_SOURCE

#include <boost/config.hpp>

# ifdef BOOST_MSVC
#  pragma warning(disable: 4702)
#  endif

#include <clocale>
#include <cstdio>
#include <list>
#include <cctype>
#include <cstdio>
#include <boost/cregex.hpp>
#include <boost/regex/regex_traits.hpp>
#include <boost/regex/v3/regex_synch.hpp>
#include <boost/regex/v3/regex_cstring.hpp>
#include <boost/scoped_array.hpp>

#include "primary_transform.hpp"


#if defined(BOOST_HAS_NL_TYPES_H)
#include <nl_types.h>
#endif

// Fixes a very strange bug in Comeau 4.2.45.2 that would otherwise result in
// an instantiation loop
#if defined(__COMO__) && __COMO_VERSION__ <= 4245
void c_regex_adopted_no_longer_needed_loop_shutter_upper() { }
#endif

namespace{

//
// helper function to get the locale name,
// works around possibly broken setlocale implementations:
//
const char* getlocale(int id)
{
   static const char* def = "Unknown";
   const char* pl = std::setlocale(id, 0);
   return pl ? pl : def;
}

//
// character classes:
//
boost::uint_fast32_t re_char_class_id[] = {
   boost::re_detail::c_traits_base::char_class_alnum,
   boost::re_detail::c_traits_base::char_class_alpha,
   boost::re_detail::c_traits_base::char_class_cntrl,
   boost::re_detail::c_traits_base::char_class_digit,
   boost::re_detail::c_traits_base::char_class_graph,
   boost::re_detail::c_traits_base::char_class_lower,
   boost::re_detail::c_traits_base::char_class_print,
   boost::re_detail::c_traits_base::char_class_punct,
   boost::re_detail::c_traits_base::char_class_space,
   boost::re_detail::c_traits_base::char_class_upper,
   boost::re_detail::c_traits_base::char_class_xdigit,
   boost::re_detail::c_traits_base::char_class_blank,
   boost::re_detail::c_traits_base::char_class_word,
   boost::re_detail::c_traits_base::char_class_unicode,
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

std::string* re_cls_name;
std::string* pclasses;
unsigned int classes_count = 0;
const unsigned int re_classes_max = 14;

//
// collate names:

struct collate_name_t
{
   std::string name;
   std::string value;
   collate_name_t(const char* p1, const char* p2, const char* p3, const char* p4)
      : name(p1, p2), value(p3, p4) {}
};

std::string* re_coll_name;
std::list<collate_name_t>* pcoll_names;
unsigned int collate_count = 0;

//
// message handling:
#ifndef BOOST_RE_MESSAGE_BASE
#define BOOST_RE_MESSAGE_BASE 0
#endif

#if defined(BOOST_HAS_NL_TYPES_H)
nl_catd message_cat = (nl_catd)-1;
#endif

unsigned int message_count = 0;
std::string* mess_locale;

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

#if !defined(LC_MESSAGES)
#define LC_MESSAGES LC_CTYPE
#endif

char re_zero;
char re_ten;

unsigned int entry_count = 0;

std::string* ctype_name;
std::string* collate_name;
enum syntax_map_size
{
   map_size = UCHAR_MAX + 1
};

std::size_t BOOST_REGEX_CALL _re_get_message(char* buf, std::size_t len, std::size_t id);

#ifndef BOOST_NO_WREGEX

BOOST_REGEX_DECL wchar_t re_zero_w;
BOOST_REGEX_DECL wchar_t re_ten_w;

unsigned int nlsw_count = 0;
std::string* wlocale_name = 0;

struct syntax_map_t
{
   wchar_t c;
   unsigned int type;
};

std::list<syntax_map_t>* syntax;

std::size_t BOOST_REGEX_CALL re_get_message(wchar_t* buf, std::size_t len, std::size_t id)
{
   std::size_t size = _re_get_message(static_cast<char*>(0), 0, id);
   if(len < size)
      return size;
   boost::scoped_array<char> cb(new char[size]);
   _re_get_message(cb.get(), size, id);
   size = boost::c_regex_traits<wchar_t>::strwiden(buf, len, cb.get());
   return size;
}
#endif

inline std::size_t BOOST_REGEX_CALL re_get_message(char* buf, std::size_t len, std::size_t id)
{
   return _re_get_message(buf, len, id);
}

void BOOST_REGEX_CALL re_init_classes()
{
   BOOST_RE_GUARD_STACK
   if(classes_count == 0)
   {
      re_cls_name = new std::string("xxxxxxxx");
#ifndef BOOST_NO_EXCEPTIONS
      try{
#endif
         pclasses = new std::string[re_classes_max];
         BOOST_REGEX_NOEH_ASSERT(pclasses)
#ifndef BOOST_NO_EXCEPTIONS
      }
      catch(...)
      {
         delete re_cls_name;
         throw;
      }
#endif
   }
   ++classes_count;
}

void BOOST_REGEX_CALL re_free_classes()
{
   BOOST_RE_GUARD_STACK
   if(--classes_count == 0)
   {
      delete re_cls_name;
      delete[] pclasses;
   }
}

void BOOST_REGEX_CALL re_update_classes()
{
   BOOST_RE_GUARD_STACK
   if(*re_cls_name != getlocale(LC_CTYPE))
   {
      *re_cls_name = getlocale(LC_CTYPE);
      char buf[256];
      unsigned int i;
      for(i = 0; i < re_classes_max; ++i)
      {
         re_get_message(buf, 256, i+300);
         pclasses[i] = buf;
      }
   }
}

void BOOST_REGEX_CALL re_init_collate()
{
   BOOST_RE_GUARD_STACK
   if(collate_count == 0)
   {
      re_coll_name = new std::string("xxxxxxxx");
#ifndef BOOST_NO_EXCEPTIONS
      try{
#endif
         pcoll_names = new std::list<collate_name_t>();
         BOOST_REGEX_NOEH_ASSERT(pcoll_names)
#ifndef BOOST_NO_EXCEPTIONS
      }
      catch(...)
      {
         delete re_coll_name;
         throw;
      }
#endif
   }
   ++collate_count;
}

void BOOST_REGEX_CALL re_free_collate()
{
   BOOST_RE_GUARD_STACK
   if(--collate_count == 0)
   {
      delete re_coll_name;
      delete pcoll_names;
   }
}

void BOOST_REGEX_CALL re_update_collate()
{
   BOOST_RE_GUARD_STACK
   if(*re_coll_name != getlocale(LC_COLLATE))
   {
      *re_coll_name = getlocale(LC_COLLATE);
      char buf[256];
      unsigned int i = 400;
      re_get_message(buf, 256, i);
      while(*buf)
      {
         char* p1, *p2, *p3, *p4;;
         p1 = buf;
         while(*p1 && std::isspace((unsigned char)*p1))++p1;
         p2 = p1;
         while(*p2 && !std::isspace((unsigned char)*p2))++p2;
         p3 = p2;
         while(*p3 && std::isspace((unsigned char)*p3))++p3;
         p4 = p3;
         while(*p4 && !std::isspace((unsigned char)*p4))++p4;
         pcoll_names->push_back(collate_name_t(p1, p2, p3, p4));
         ++i;
         re_get_message(buf, 256, i);
      }
   }
}

std::size_t BOOST_REGEX_CALL _re_get_message(char* buf, std::size_t len, std::size_t id)
{
   BOOST_RE_GUARD_STACK
   // get the customised message if any:
   #if defined(BOOST_HAS_NL_TYPES_H)
   if(message_cat != (nl_catd)-1)
   {
      const char* m = catgets(message_cat, 0, id, 0);
      if(m)
      {
         std::size_t size = std::strlen(m) + 1;
         if(size > len)
            return size;
         std::strcpy(buf, m);
         return size;
      }
   }
   #endif

   //
   // now get the default message if any:
   return boost::re_detail::re_get_default_message(buf, len, id);
}

void BOOST_REGEX_CALL re_message_init()
{
   BOOST_RE_GUARD_STACK
   if(message_count == 0)
   {
      mess_locale = new std::string("xxxxxxxxxxxxxxxx");
   }
   ++message_count;
}

void BOOST_REGEX_CALL re_message_update()
{
   BOOST_RE_GUARD_STACK
   //
   // called whenever the global locale changes:
   //
   std::string l(getlocale(LC_MESSAGES));
   if(*mess_locale != l)
   {
      *mess_locale = l;
#if defined(BOOST_HAS_NL_TYPES_H)
      if(message_cat != (nl_catd)-1)
      {
         catclose(message_cat);
         message_cat = (nl_catd)-1;
      }
      if(*boost::re_detail::c_traits_base::get_catalogue())
      {
         message_cat = catopen(boost::re_detail::c_traits_base::get_catalogue(), 0);
#ifndef BOOST_NO_EXCEPTIONS
         if(message_cat == (nl_catd)-1)
         {
            std::string m("Unable to open message catalog: ");
            throw std::runtime_error(m + boost::re_detail::c_traits_base::get_catalogue());
         }
#else
         BOOST_REGEX_NOEH_ASSERT(message_cat != (nl_catd)-1);
#endif
      }
#endif
      for(int i = 0; i < boost::REG_E_UNKNOWN; ++i)
      {
         if(re_custom_error_messages[i])
         {
            boost::re_detail::re_strfree(re_custom_error_messages[i]);
            re_custom_error_messages[i] = 0;
         }
      }
   }
}

void BOOST_REGEX_CALL re_message_free()
{
   BOOST_RE_GUARD_STACK
   --message_count;
   if(message_count == 0)
   {
#if defined(BOOST_HAS_NL_TYPES_H)
      if(message_cat != (nl_catd)-1)
         catclose(message_cat);
#endif
      delete mess_locale;
      for(int i = 0; i < boost::REG_E_UNKNOWN; ++i)
      {
         if(re_custom_error_messages[i])
         {
            boost::re_detail::re_strfree(re_custom_error_messages[i]);
            re_custom_error_messages[i] = 0;
         }
      }
   }
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

char c_traits_base::regex_message_catalogue[BOOST_REGEX_MAX_PATH] = {0};

std::string BOOST_REGEX_CALL c_traits_base::error_string(unsigned id)
{
   return re_get_error_str(id);
}

void BOOST_REGEX_CALL c_traits_base::do_update_collate()
{
   BOOST_RE_GUARD_STACK
   re_update_collate();
   std::string s;
   const char* p = "zero";
   if(c_regex_traits<char>::lookup_collatename(s, p, p+4))
   {
      jm_assert(s.size() == 1);
      re_zero = *s.c_str();
   }
   else
      re_zero = '0';

   p = "ten";
   if(c_regex_traits<char>::lookup_collatename(s, p, p+3))
   {
      jm_assert(s.size() == 1);
      re_ten = *s.c_str();
   }
   else
      re_ten = 'a';
}

void BOOST_REGEX_CALL c_traits_base::do_update_ctype()
{
   BOOST_RE_GUARD_STACK
   // start by updating the syntax map:
   unsigned int i;
   char buf[map_size+2];
   std::memset(syntax_map, syntax_char, map_size);
   for(i = 1; i < syntax_max; ++i)
   {
      char* ptr = buf;
      re_get_message(static_cast<char*>(buf), map_size, i+100);
      for(; *ptr; ++ptr)
      {
         syntax_map[(unsigned char)*ptr] = (unsigned char)i;
      }
   }

   // now update the character class map,
   // and lower case map:
   std::memset(class_map, 0, map_size);
   for(i = 0; i < map_size; ++i)
   {
      if(std::isalpha(i))
         class_map[i] |= char_class_alpha;
      if(std::iscntrl(i))
         class_map[i] |= char_class_cntrl;
      if(std::isdigit(i))
         class_map[i] |= char_class_digit;
      if(std::islower(i))
         class_map[i] |= char_class_lower;
      if(std::isupper(i))
         class_map[i] |= char_class_upper;
      if(std::ispunct(i))
         class_map[i] |= char_class_punct;
      if(std::isspace(i))
         class_map[i] |= char_class_space;
      if(std::isxdigit(i))
         class_map[i] |= char_class_xdigit;
   }
   class_map['_'] |= char_class_underscore;
   class_map[' '] |= char_class_blank;
   class_map['\t'] |= char_class_blank;
   for(i = 0; i < map_size; ++i)
   {
      lower_case_map[i] = (char)std::tolower(i);
   }
   re_update_classes();
}

boost::uint_fast32_t BOOST_REGEX_CALL c_traits_base::do_lookup_class(const char* p)
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

bool BOOST_REGEX_CALL c_traits_base::do_lookup_collate(std::string& buf, const char* p)
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

std::string BOOST_REGEX_CALL c_traits_base::set_message_catalogue(const std::string& l)
{
   if(sizeof(regex_message_catalogue) <= l.size())
      return l;
   std::string old(regex_message_catalogue);
   std::strcpy(regex_message_catalogue, l.c_str());
   return old;
}

unsigned char c_traits_base::syntax_map[map_size];
unsigned short c_traits_base::class_map[map_size];
char c_traits_base::lower_case_map[map_size];

} // namespace re_detail

#ifndef BOOST_NO_WREGEX
bool BOOST_REGEX_CALL c_regex_traits<wchar_t>::lookup_collatename(std::basic_string<wchar_t>& out, const wchar_t* first, const wchar_t* last)
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
         out.append(1, (wchar_t)0);
   }
   return result;
}
#endif

c_regex_traits<char> c_regex_traits<char>::i;

void BOOST_REGEX_CALL c_regex_traits<char>::init()
{
   BOOST_RE_GUARD_STACK
#ifdef BOOST_HAS_THREADS
   re_detail::re_init_threads();
   re_detail::cs_guard g(*re_detail::p_re_lock);
#endif
   // just keep track of entry_count
   if(entry_count == 0)
   {
      ctype_name = new std::string("xxxxxxxxxxxxxxxx");
#ifndef BOOST_NO_EXCEPTIONS
      try{
#endif
         collate_name = new std::string("xxxxxxxxxxxxxxxx");
         BOOST_REGEX_NOEH_ASSERT(collate_name)
#ifndef BOOST_NO_EXCEPTIONS
      }
      catch(...)
      {
         delete ctype_name;
         throw;
      }
#endif
   }
   re_message_init();
   re_init_classes();
   re_init_collate();
   ++entry_count;
}

void BOOST_REGEX_CALL c_regex_traits<char>::update()
{
   BOOST_RE_GUARD_STACK
   #ifdef BOOST_HAS_THREADS
   re_detail::cs_guard g(*re_detail::p_re_lock);
   #endif
   re_message_update();
   if(*collate_name != getlocale(LC_COLLATE))
   {
      do_update_collate();
      *collate_name = getlocale(LC_COLLATE);
   }
   if(*ctype_name != getlocale(LC_CTYPE))
   {
      do_update_ctype();
      *ctype_name = getlocale(LC_CTYPE);
   }
   sort_type = re_detail::find_sort_syntax(&i, &sort_delim);
}

void BOOST_REGEX_CALL c_regex_traits<char>::m_free()
{
   BOOST_RE_GUARD_STACK
   #ifdef BOOST_HAS_THREADS
   re_detail::cs_guard g(*re_detail::p_re_lock);
   #endif
   re_message_free();
   re_free_classes();
   re_free_collate();
   --entry_count;
   // add reference to static member here to ensure
   // that the linker includes it in the .exe:
   if((entry_count == 0) && (0 != &c_regex_traits<char>::i))
   {
      delete ctype_name;
      delete collate_name;
   }
#ifdef BOOST_HAS_THREADS
   g.acquire(false);
   re_detail::re_free_threads();
#endif
}

void BOOST_REGEX_CALL c_regex_traits<char>::transform(std::string& out, const std::string& in)
{
   BOOST_RE_GUARD_STACK
   std::size_t n = std::strxfrm(0, in.c_str(), 0);
   if(n == (std::size_t)(-1))
   {
      out = in;
      return;
   }
   scoped_array<char> buf(new char[n+1]);
   n = std::strxfrm(buf.get(), in.c_str(), n+1);
   if(n == (std::size_t)(-1))
   {
      out = in;
      return;
   }
   out = buf.get();
}

void BOOST_REGEX_CALL c_regex_traits<char>::transform_primary(std::string& out, const std::string& in)
{
   transform(out, in);
   switch(sort_type)
   {
   case re_detail::sort_C:
   case re_detail::sort_unknown:
      break;
   case re_detail::sort_fixed:
      out.erase((int)sort_delim);
      break;
   case re_detail::sort_delim:
      for(unsigned int i = 0; i < out.size(); ++i)
      {
         if((out[i] == sort_delim) && (i+1 < out.size()))
         {
            out.erase(i+1);
            break;
         }
      }
   }
}

unsigned c_regex_traits<char>::sort_type;
char c_regex_traits<char>::sort_delim;


int BOOST_REGEX_CALL c_regex_traits<char>::toi(char c)
{
   if(is_class(c, char_class_digit))
      return c - re_zero;
   if(is_class(c, char_class_xdigit))
      return 10 + translate(c, true) - translate(re_ten, true);
   return -1; // error!!
}

int BOOST_REGEX_CALL c_regex_traits<char>::toi(const char*& first, const char* last, int radix)
{
   unsigned int maxval;
   if(radix < 0)
   {
      // if radix is less than zero, then restrict
      // return value to charT. NB assumes sizeof(charT) <= sizeof(int)
      radix *= -1;
      maxval = 1u << (sizeof(*first) * CHAR_BIT - 1);
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

unsigned int BOOST_REGEX_CALL c_regex_traits<wchar_t>::syntax_type(size_type c)
{
   BOOST_RE_GUARD_STACK
   std::list<syntax_map_t>::const_iterator first, last;
   first = syntax->begin();
   last = syntax->end();
   while(first != last)
   {
      if((uchar_type)(*first).c == c)
         return (*first).type;
      ++first;
   }
   return 0;
}

void BOOST_REGEX_CALL c_regex_traits<wchar_t>::init()
{
   BOOST_RE_GUARD_STACK
   re_detail::re_init_threads();
#ifdef BOOST_HAS_THREADS
   re_detail::cs_guard g(*re_detail::p_re_lock);
#endif
   re_message_init();
   re_init_classes();
   re_init_collate();
   if(nlsw_count == 0)
   {
      wlocale_name = new std::string("xxxxxxxxxxxxxxxx");
#ifndef BOOST_NO_EXCEPTIONS
      try{
#endif
         syntax = new std::list<syntax_map_t>();
         BOOST_REGEX_NOEH_ASSERT(syntax)
#ifndef BOOST_NO_EXCEPTIONS
      }
      catch(...)
      {
         delete wlocale_name;
         throw;
      }
#endif
   }
   ++nlsw_count;
}

bool BOOST_REGEX_CALL c_regex_traits<wchar_t>::do_lookup_collate(std::basic_string<wchar_t>& out, const wchar_t* first, const wchar_t* last)
{
   BOOST_RE_GUARD_STACK
   std::basic_string<wchar_t> s(first, last);
   std::size_t len = strnarrow(static_cast<char*>(0), 0, s.c_str());
   scoped_array<char> buf(new char[len]);
   strnarrow(buf.get(), len, s.c_str());
   std::string t_out;
   bool result = base_type::do_lookup_collate(t_out, buf.get());
   if(result)
   {
      len = strwiden(static_cast<wchar_t*>(0), 0, t_out.c_str());
      scoped_array<wchar_t> wb(new wchar_t[len]);
      strwiden(wb.get(), len, t_out.c_str());
      out = wb.get();
   }
   return result;
}


void BOOST_REGEX_CALL c_regex_traits<wchar_t>::update()
{
   BOOST_RE_GUARD_STACK
#ifdef BOOST_HAS_THREADS
   re_detail::cs_guard g(*re_detail::p_re_lock);
#endif
   re_message_update();
   re_update_classes();
   re_update_collate();
   std::string l(getlocale(LC_CTYPE));
   if(*wlocale_name != l)
   {
      *wlocale_name = l;
      std::basic_string<wchar_t> s;
      const wchar_t* p = L"zero";
      if(do_lookup_collate(s, p, p+4))
      {
         jm_assert(s.size() == 1);
         re_zero_w = *s.c_str();
      }
      else
         re_zero_w = L'0';

      p = L"ten";
      if(do_lookup_collate(s, p, p+3))
      {
         jm_assert(s.size() == 1);
         re_ten_w = *s.c_str();
      }
      else
         re_ten_w = L'a';

      unsigned int i;
      wchar_t buf[256];
      syntax_map_t sm;
      syntax->clear();
      for(i = 1; i < syntax_max; ++i)
      {
         wchar_t* ptr = buf;
         re_get_message(static_cast<wchar_t*>(buf), 256, i+100);
         for(; *ptr; ++ptr)
         {
            sm.c = *ptr;
            sm.type = i;
            syntax->push_back(sm);
         }
      }
      sort_type = re_detail::find_sort_syntax(&init_, &sort_delim);
   }
}

void BOOST_REGEX_CALL c_regex_traits<wchar_t>::m_free()
{
   BOOST_RE_GUARD_STACK
#ifdef BOOST_HAS_THREADS
   re_detail::cs_guard g(*re_detail::p_re_lock);
#endif
   --nlsw_count;
   re_message_free();
   re_free_classes();
   re_free_collate();
   // add reference to static member here to ensure
   // that the linker includes it in the .exe:
   if((nlsw_count == 0) && (0 != &c_regex_traits<wchar_t>::init_))
   {
      // cleanup:
      delete wlocale_name;
      delete syntax;
   }
#ifdef BOOST_HAS_THREADS
   g.acquire(false);
   re_detail::re_free_threads();
#endif
}

bool BOOST_REGEX_CALL c_regex_traits<wchar_t>::do_iswclass(wchar_t c, boost::uint_fast32_t f)
{
   BOOST_RE_GUARD_STACK
   if((c & ~0xFF) == 0)
      return BOOST_REGEX_MAKE_BOOL(re_detail::wide_unicode_classes[(uchar_type)c] & f);
   if((f & char_class_alpha) && std::iswalpha(c))
      return true;
   if((f & char_class_cntrl) && std::iswcntrl(c))
      return true;
   if((f & char_class_digit) && std::iswdigit(c))
      return true;
   if((f & char_class_lower) && std::iswlower(c))
      return true;
   if((f & char_class_punct) && std::iswpunct(c))
      return true;
   if((f & char_class_space) && std::iswspace(c))
      return true;
   if((f & char_class_upper) && std::iswupper(c))
      return true;
   if((f & char_class_xdigit) && std::iswxdigit(c))
      return true;
   if(f & char_class_unicode)
      return true;
   return false;
}

void BOOST_REGEX_CALL c_regex_traits<wchar_t>::transform(std::basic_string<wchar_t>& out, const std::basic_string<wchar_t>& in)
{
   BOOST_RE_GUARD_STACK
#ifndef BOOST_MSVC
   std::size_t n = std::wcsxfrm(0, in.c_str(), 0);
#else
   // broken wcsxfrm under VC6 doesn't check size of
   // output buffer, we have no choice but to guess!
   std::size_t n = 100 * in.size();
#endif
   if((n == (std::size_t)(-1)) || (n == 0))
   {
      out = in;
      return;
   }
   scoped_array<wchar_t> buf(new wchar_t[n+1]);
   n = std::wcsxfrm(buf.get(), in.c_str(), n+1);
   if(n == (std::size_t)(-1))
   {
      out = in;
      return;
   }
   out = buf.get();
}

void BOOST_REGEX_CALL c_regex_traits<wchar_t>::transform_primary(std::basic_string<wchar_t>& out, const std::basic_string<wchar_t>& in)
{
   transform(out, in);
   switch(sort_type)
   {
   case re_detail::sort_C:
   case re_detail::sort_unknown:
      break;
   case re_detail::sort_fixed:
      if((unsigned)sort_delim < out.size())
         out.erase((int)sort_delim);
      break;
   case re_detail::sort_delim:
      for(unsigned int i = 0; i < out.size(); ++i)
      {
         if((out[i] == sort_delim) && (i+1 < out.size()))
         {
            out.erase(i+1);
            break;
         }
      }
   }
}

unsigned c_regex_traits<wchar_t>::sort_type;
wchar_t c_regex_traits<wchar_t>::sort_delim;


int BOOST_REGEX_CALL c_regex_traits<wchar_t>::toi(wchar_t c)
{
   if(is_class(c, char_class_digit))
      return c - re_zero_w;
   if(is_class(c, char_class_xdigit))
      return 10 + translate(c, true) - translate(re_ten_w, true);
   return -1; // error!!
}

int BOOST_REGEX_CALL c_regex_traits<wchar_t>::toi(const wchar_t*& first, const wchar_t* last, int radix)
{
   unsigned int maxval;
   if(radix < 0)
   {
      // if radix is less than zero, then restrict
      // return value to charT. NB assumes sizeof(charT) <= sizeof(int)
      radix *= -1;
      maxval = 1u << (sizeof(*first) * CHAR_BIT - 1);
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

boost::uint_fast32_t BOOST_REGEX_CALL c_regex_traits<wchar_t>::lookup_classname(const wchar_t* first, const wchar_t* last)
{
   std::basic_string<wchar_t> s(first, last);
   std::size_t len = strnarrow(static_cast<char*>(0), 0, s.c_str());
   scoped_array<char> buf(new char[len]);
   strnarrow(buf.get(), len, s.c_str());
   boost::uint_fast32_t result =  do_lookup_class(buf.get());
   return result;
}

c_regex_traits<wchar_t> c_regex_traits<wchar_t>::init_;

std::size_t BOOST_REGEX_CALL c_regex_traits<wchar_t>::strnarrow(char *s1, std::size_t len, const wchar_t *s2)
{
   BOOST_RE_GUARD_STACK
   std::size_t size = std::wcslen(s2) + 1;
   if(size > len)
      return size;
   return std::wcstombs(s1, s2, len);
}

std::size_t BOOST_REGEX_CALL c_regex_traits<wchar_t>::strwiden(wchar_t *s1, std::size_t len, const char *s2)
{
   BOOST_RE_GUARD_STACK
   std::size_t size = std::strlen(s2) + 1;
   if(size > len)
      return size;
   size = std::mbstowcs(s1, s2, len);
   s1[size] = 0;
   return size + 1;
}

#endif // BOOST_NO_WREGEX

} // namespace boost



