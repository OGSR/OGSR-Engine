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
  *   FILE:        c_regex_traits.cpp
  *   VERSION:     see <boost/version.hpp>
  *   DESCRIPTION: Implements the cpp_regex_traits<charT> traits class
  */


#define BOOST_REGEX_SOURCE

#include <boost/regex/config.hpp>

#if !defined(BOOST_NO_STD_LOCALE) && !defined(BOOST_NO_STD_WSTREAMBUF)

# ifdef BOOST_MSVC
#  pragma warning(disable:4786 4702 4127 4244)
#  endif

#include <clocale>
#include <locale>
#include <cstdio>
#include <list>
#include <cctype>
#include <iostream>
#include <map>
#include <boost/regex/regex_traits.hpp>
#include <boost/cregex.hpp>
#include <boost/scoped_array.hpp>
#include "primary_transform.hpp"

# ifdef BOOST_MSVC
#  pragma warning(disable:4786 4702 4127 4244)
#  endif

namespace{
   const unsigned int re_classes_max = 14;
   const unsigned int char_set_size = CHAR_MAX - CHAR_MIN + 1;

boost::uint_fast32_t re_char_class_id[] = {
   boost::re_detail::cpp_regex_traits_base::char_class_alnum,
   boost::re_detail::cpp_regex_traits_base::char_class_alpha,
   boost::re_detail::cpp_regex_traits_base::char_class_cntrl,
   boost::re_detail::cpp_regex_traits_base::char_class_digit,
   boost::re_detail::cpp_regex_traits_base::char_class_graph,
   boost::re_detail::cpp_regex_traits_base::char_class_lower,
   boost::re_detail::cpp_regex_traits_base::char_class_print,
   boost::re_detail::cpp_regex_traits_base::char_class_punct,
   boost::re_detail::cpp_regex_traits_base::char_class_space,
   boost::re_detail::cpp_regex_traits_base::char_class_upper,
   boost::re_detail::cpp_regex_traits_base::char_class_xdigit,
   boost::re_detail::cpp_regex_traits_base::char_class_blank,
   boost::re_detail::cpp_regex_traits_base::char_class_word,
   boost::re_detail::cpp_regex_traits_base::char_class_unicode,
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

template <class charT,
          class traits = ::std::char_traits<charT> >
class parser_buf : public ::std::basic_streambuf<charT, traits>
{
   typedef ::std::basic_streambuf<charT, traits> base_type;
   typedef typename base_type::int_type int_type;
   typedef typename base_type::char_type char_type;
   typedef typename base_type::pos_type pos_type;
   typedef ::std::streamsize streamsize;
   typedef typename base_type::off_type off_type;
public:
   parser_buf() : base_type() { setbuf(0, 0); }
   const charT* getnext() { return this->gptr(); }
protected:
   std::basic_streambuf<charT, traits>* setbuf(char_type* s, streamsize n);
   typename parser_buf<charT, traits>::pos_type seekpos(pos_type sp, ::std::ios_base::openmode which);
   typename parser_buf<charT, traits>::pos_type seekoff(off_type off, ::std::ios_base::seekdir way, ::std::ios_base::openmode which);
private:
   parser_buf& operator=(const parser_buf&);
   parser_buf(const parser_buf&);
};

template<class charT, class traits>
std::basic_streambuf<charT, traits>*
parser_buf<charT, traits>::setbuf(char_type* s, streamsize n)
{
   this->setg(s, s, s + n);
   return this;
}

template<class charT, class traits>
typename parser_buf<charT, traits>::pos_type
parser_buf<charT, traits>::seekoff(off_type off, ::std::ios_base::seekdir way, ::std::ios_base::openmode which)
{
   typedef typename parser_buf<charT, traits>::pos_type pos_type;
   if(which & ::std::ios_base::out)
      return pos_type(off_type(-1));
   std::ptrdiff_t size = this->egptr() - this->eback();
   std::ptrdiff_t pos = this->gptr() - this->eback();
   charT* g = this->eback();
   switch(way)
   {
   case ::std::ios_base::beg:
      if((off < 0) || (off > size))
         return pos_type(off_type(-1));
      else
         this->setg(g, g + off, g + size);
      break;
   case ::std::ios_base::end:
      if((off < 0) || (off > size))
         return pos_type(off_type(-1));
      else
         this->setg(g, g + size - off, g + size);
      break;
   case ::std::ios_base::cur:
   {
      std::ptrdiff_t newpos = pos + off;
      if((newpos < 0) || (newpos > size))
         return pos_type(off_type(-1));
      else
         this->setg(g, g + newpos, g + size);
      break;
   }
   default: ;
   }
   return static_cast<pos_type>(this->gptr() - this->eback());
}

template<class charT, class traits>
typename parser_buf<charT, traits>::pos_type
parser_buf<charT, traits>::seekpos(pos_type sp, ::std::ios_base::openmode which)
{
   if(which & ::std::ios_base::out)
      return pos_type(off_type(-1));
   std::ptrdiff_t size = this->egptr() - this->eback();
   charT* g = this->eback();
   if(sp <= size)
   {
      this->setg(g, g + ::std::streamsize(sp), g + size);
   }
   return pos_type(off_type(-1));
}


} // namespace

namespace boost{
   namespace re_detail{

template <>
struct message_data<char>
{
   unsigned char syntax_map[CHAR_MAX-CHAR_MIN];
   std::map<std::string, std::string, std::less<std::string> > collating_elements;
   std::map<std::string, std::size_t, std::less<std::string> > classes;
   //std::string _zero;
   //std::string _ten;
   parser_buf<char> sbuf;
   std::istream is;
   std::string error_strings[boost::REG_E_UNKNOWN+1];

   message_data(const std::locale& l, const std::string& regex_message_catalogue);
private:
   message_data(const message_data&);
   message_data& operator=(const message_data&);
};


message_data<char>::message_data(const std::locale& l, const std::string& regex_message_catalogue)
   : is(&sbuf)
{
   is.imbue(l);
#ifndef BOOST_NO_STD_MESSAGES

   const std::messages<char>* pm = 0;
#ifndef __IBMCPP__
   std::messages<char>::catalog cat = static_cast<std::messages<wchar_t>::catalog>(-1);
#else
   std::messages<char>::catalog cat = reinterpret_cast<std::messages<wchar_t>::catalog>(-1);
#endif
   if(regex_message_catalogue.size())
   {
      pm = &BOOST_USE_FACET(std::messages<char>, l);
      cat = pm->open(regex_message_catalogue, l);
#ifndef BOOST_NO_EXCEPTIONS
      if(cat < 0)
      {
         std::string m("Unable to open message catalog: ");
         throw std::runtime_error(m + regex_message_catalogue);
      }
#else
      BOOST_REGEX_NOEH_ASSERT(cat >= 0);
#endif
   } 
#endif
   std::memset(syntax_map, cpp_regex_traits<char>::syntax_char, 256);
   unsigned i;
   scoped_array<char> a;
   std::size_t array_size = 0;
   std::size_t new_size;
   for(i = 1; i < cpp_regex_traits<char>::syntax_max; ++i)
   {
      new_size = re_get_default_message(0, 0, i+100);
      if(new_size > array_size)
      {
         a.reset(new char[new_size]);
         array_size = new_size;
      }
      re_get_default_message(a.get(), array_size, i+100);
      std::string s = a.get();
#ifndef BOOST_NO_STD_MESSAGES
      if((int)cat >= 0)
         s = pm->get(cat, 0, i+100, s);
#endif
      for(std::size_t j = 0; j < s.size(); ++j)
      {
         syntax_map[s[j]] = (unsigned char)(i);
      }
   }

#ifndef BOOST_NO_STD_MESSAGES
   // load any custom collate names:
   //
   // for some reason Borland C++ Builder 6 won't let us use
   // std::isspace(char, std::locale) unless we call it
   // unqualifed - weird.  This seems to be affecting other
   // STLport users as well (gcc3.1+STLport5), so enable the
   // workaround for all STLport users...
   //
#if defined(__SGI_STL_PORT) || defined(_STLPORT_VERSION)
   using namespace std;
#  define BOOST_REGEX_STD
#else
#  define BOOST_REGEX_STD std::
#endif

   std::string c1, c2;
   i = 400;
   if((int)cat >= 0)
   {
      c2 = pm->get(cat, 0, i, c1);
      while(c2.size())
      {
         const char* p1, *p2, *p3, *p4;;
         p1 = c2.c_str();
         while(*p1 && BOOST_REGEX_STD isspace((char)*p1, l))++p1;
         p2 = p1;
         while(*p2 && !BOOST_REGEX_STD isspace((char)*p2, l))++p2;
         p3 = p2;
         while(*p3 && BOOST_REGEX_STD isspace((char)*p3, l))++p3;
         p4 = p3;
         while(*p4 && !BOOST_REGEX_STD isspace((char)*p4, l))++p4;
         collating_elements[std::string(p1, p2)] = std::string(p3, p4);

         ++i;
         c2 = pm->get(cat, 0, i, c1);
      }
   }
#endif
   std::string m;
   std::string s;
#ifndef BOOST_NO_STD_MESSAGES
   if((int)cat >= 0)
   {
      for(i = 0; i < re_classes_max; ++i)
      {
         s = pm->get(cat, 0, i+300, m);
         if(s.size())
            classes[s] = i;
      }
      for(i = 0; i <= boost::REG_E_UNKNOWN ; ++i)
      {
         s = pm->get(cat, 0, i+200, m);
         error_strings[i] = s;
      }
   }

   if((int)cat >= 0)
      pm->close(cat);
#endif
}

std::string BOOST_REGEX_CALL cpp_regex_traits_base::set_message_catalogue(const std::string& l)
{
   if(sizeof(regex_message_cat) <= l.size())
      return l;
   std::string old(regex_message_cat);
   std::strcpy(regex_message_cat, l.c_str());
   return old;
}

char cpp_regex_traits_base::regex_message_cat[BOOST_REGEX_MAX_PATH] = {0};


} // namespace re_detail


cpp_regex_traits<char>::cpp_regex_traits()
{
   pmd = new re_detail::message_data<char>(locale_inst, regex_message_cat);
   psyntax = pmd->syntax_map;
#ifndef BOOST_NO_EXCEPTIONS
   try{
#endif
      lower_map = new char[char_set_size];
      BOOST_REGEX_NOEH_ASSERT(lower_map)
#ifndef BOOST_NO_EXCEPTIONS
   }
   catch(...)
   {
      delete pmd;
      throw;
   }
#endif
   for(unsigned int i = 0; i < char_set_size; ++i)
      lower_map[i] = static_cast<char>(i);
   pctype = &BOOST_USE_FACET(std::ctype<char>, locale_inst);
   pctype->tolower(&lower_map[0], &lower_map[char_set_size]);
   pcollate = &BOOST_USE_FACET(std::collate<char>, locale_inst);
   sort_type = re_detail::find_sort_syntax(this, &(this->sort_delim));
}

cpp_regex_traits<char>::~cpp_regex_traits()
{
   delete pmd;
   delete[] lower_map;
}

int BOOST_REGEX_CALL cpp_regex_traits<char>::toi(char c)const
{
   pmd->sbuf.pubsetbuf(&c, 1);
   pmd->is.clear();
   pmd->is >> std::dec;
   int val;
   if(pmd->is >> val)
   {
      return val;
   }
   else
      return 0;
}

int BOOST_REGEX_CALL cpp_regex_traits<char>::toi(const char*& first, const char* last, int radix)const
{
   pmd->sbuf.pubsetbuf(const_cast<char*>(first), static_cast<std::streamsize>(last-first));
   pmd->is.clear();
   if(std::abs(radix) == 16) pmd->is >> std::hex;
   else if(std::abs(radix) == 8) pmd->is >> std::oct;
   else pmd->is >> std::dec;
   int val;
   if(pmd->is >> val)
   {
      first = first + ((last - first) - pmd->sbuf.in_avail());
      return val;
   }
   else
      return 0;
}

boost::uint_fast32_t BOOST_REGEX_CALL cpp_regex_traits<char>::lookup_classname(const char* first, const char* last)const
{
   BOOST_RE_GUARD_STACK
   unsigned int i;
   std::string s(first, last);

   std::map<std::string, std::size_t, std::less<std::string> >::const_iterator pos = pmd->classes.find(s);
   if(pos != pmd->classes.end())
      return re_char_class_id[(*pos).second];

   for(i = 0; i < re_classes_max; ++i)
   {
      if(s == re_char_class_names[i])
         return re_char_class_id[i];
   }
   return 0;
}

bool BOOST_REGEX_CALL cpp_regex_traits<char>::lookup_collatename(std::string& s, const char* first, const char* last)const
{
   BOOST_RE_GUARD_STACK
   std::string name(first, last);
   std::map<std::string, std::string, std::less<std::string > >::const_iterator pos = pmd->collating_elements.find(name);
   if(pos != pmd->collating_elements.end())
   {
      s = (*pos).second;
      return true;
   }
   return re_detail::re_lookup_def_collate_name(s, name.c_str());
}

void BOOST_REGEX_CALL cpp_regex_traits<char>::transform_primary(std::string& out, const std::string& in)const
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


std::string BOOST_REGEX_CALL cpp_regex_traits<char>::error_string(unsigned id)const
{
   if((id <= boost::REG_E_UNKNOWN) && (pmd->error_strings[id].size()))
      return pmd->error_strings[id];
   return boost::re_detail::re_default_error_messages[id];
}

cpp_regex_traits<char>::locale_type BOOST_REGEX_CALL cpp_regex_traits<char>::imbue(locale_type l)
{
   locale_type old_l(locale_inst);
   locale_inst = l;
   re_detail::message_data<char>* npmd = new re_detail::message_data<char>(locale_inst, regex_message_cat);
   delete pmd;
   pmd = npmd;
   psyntax = pmd->syntax_map;
   for(unsigned int i = 0; i < char_set_size; ++i)
      lower_map[i] = static_cast<char>(i);
   pctype = &BOOST_USE_FACET(std::ctype<char>, locale_inst);
   pctype->tolower(&lower_map[0], &lower_map[char_set_size]);
   pcollate = &BOOST_USE_FACET(std::collate<char>, locale_inst);
   sort_type = re_detail::find_sort_syntax(this, &(this->sort_delim));
   return old_l;
}

#ifndef BOOST_NO_WREGEX

namespace re_detail{

std::string BOOST_REGEX_CALL to_narrow(const std::basic_string<wchar_t>& is, const std::codecvt<wchar_t, char, std::mbstate_t>& cvt)
{
   BOOST_RE_GUARD_STACK
   std::basic_string<wchar_t>::size_type bufsize = is.size() * 2;
   //
   // declare buffer first as VC6 workaround for internal compiler error!
   char* pc = new char[bufsize];
   scoped_array<char> t(pc);
   #if defined(BOOST_MSVC) && !defined(DINKUMWARE_CE)
   std::mbstate_t state = 0;
   #else
   std::mbstate_t state = std::mbstate_t();
   #endif

   const wchar_t* next_in;
   char* next_out;
   while(true)
   {
      switch(cvt.out(state, is.c_str(), is.c_str() + is.size(), next_in, t.get(), t.get() + bufsize, next_out))
      {
      case std::codecvt_base::ok:
         return std::string(t.get(), next_out);
      case std::codecvt_base::partial:
         bufsize *= 2;
         t.reset(new char[bufsize]);
         continue;
      case std::codecvt_base::error:
         // not much we can do here but guess:
      case std::codecvt_base::noconv:
         std::string out;
         for(unsigned i = 0; i < is.size(); ++i)
         {
            out.append(1, (char)is[i]);
         }
         return out;
      }
   }
}

std::wstring BOOST_REGEX_CALL to_wide(const std::string& is, const std::codecvt<wchar_t, char, std::mbstate_t>& cvt)
{
   BOOST_RE_GUARD_STACK
   std::string::size_type bufsize = is.size() + 2;
   std::string::size_type maxsize = is.size() * 100;
   //
   // declare buffer first as VC6 workaround for internal compiler error!
   wchar_t* pc = new wchar_t[bufsize];
   scoped_array<wchar_t> t(pc);
   #if defined(BOOST_MSVC) && !defined(DINKUMWARE_CE)
   std::mbstate_t state = 0;
   #else
   std::mbstate_t state = std::mbstate_t();
   #endif


   wchar_t* next_out;
   const char* next_in;
   while(true)
   {
      switch(cvt.in(state, is.c_str(), is.c_str() + is.size(), next_in, t.get(), t.get() + bufsize, next_out))
      {
      case std::codecvt_base::ok:
         return std::wstring(t.get(), next_out);
      case std::codecvt_base::partial:
         bufsize *= 2;
         if(bufsize < maxsize)
         {
            t.reset(new wchar_t[bufsize]);
            continue;
         }
         //
         // error fall through:
      case std::codecvt_base::error:
         // not much we can do here but guess:
      case std::codecvt_base::noconv:
         std::wstring out;
         for(unsigned i = 0; i < is.size(); ++i)
         {
            out.append(1, is[i]);
         }
         return out;
      }
   }
}



template <>
struct message_data<wchar_t>
{
#ifndef BOOST_NO_STD_MESSAGES
   typedef std::messages<wchar_t>::string_type string_type;
#else
   typedef std::wstring string_type;
#endif

   string_type name;

   struct syntax_map
   {
      wchar_t c;
      unsigned int type;
   };

   std::list<syntax_map> syntax;
   std::map<string_type, std::size_t> classes;
   std::map<string_type, string_type> collating_elements;
   unsigned char syntax_[CHAR_MAX-CHAR_MIN+1];

   parser_buf<wchar_t> sbuf;
   std::wistream is;
   std::string error_strings[boost::REG_E_UNKNOWN+1];

   message_data(const std::locale& l, const std::string& regex_message_catalogue);
private:
   message_data(const message_data&);
   message_data& operator=(const message_data&);
};

message_data<wchar_t>::message_data(const std::locale& l, const std::string& regex_message_catalogue)
   : is(&sbuf)
{
   is.imbue(l);
   syntax_map m;
   typedef std::codecvt<wchar_t, char, std::mbstate_t> cvt_type;
   const cvt_type& cvt = BOOST_USE_FACET(cvt_type, l);
#ifndef BOOST_NO_STD_MESSAGES
   const std::messages<wchar_t>& msgs = BOOST_USE_FACET(std::messages<wchar_t>, l);
#ifndef __IBMCPP__
   std::messages<wchar_t>::catalog cat = static_cast<std::messages<wchar_t>::catalog>(-1);
#else
   std::messages<wchar_t>::catalog cat = reinterpret_cast<std::messages<wchar_t>::catalog>(-1);
#endif
   if(regex_message_catalogue.size())
   {
      cat = msgs.open(regex_message_catalogue, l);
#ifndef BOOST_NO_EXCEPTIONS
      if(cat < 0)
      {
         std::string m("Unable to open message catalog: ");
         throw std::runtime_error(m + regex_message_catalogue);
      }
#else
      BOOST_REGEX_NOEH_ASSERT(cat >= 0);
#endif
   }
#endif
   scoped_array<char> a;
   std::size_t array_size = 0;
   std::size_t new_size;
   std::size_t i;
   std::memset(syntax_, cpp_regex_traits<wchar_t>::syntax_char, sizeof(syntax_));
   for(i = 1; i < cpp_regex_traits<wchar_t>::syntax_max; ++i)
   {
      new_size = re_get_default_message(0, 0, i+100);
      if(new_size > array_size)
      {
         a.reset(new char[new_size]);
         array_size = new_size;
      }
      re_get_default_message(a.get(), array_size, i+100);
      std::string ns = a.get();
      string_type s = to_wide(ns, cvt);
#ifndef BOOST_NO_STD_MESSAGES
      if((int)cat >= 0)
         s = BOOST_USE_FACET(std::messages<wchar_t>, l).get(cat, 0, (int)i+100, s);
#endif
      for(unsigned int j = 0; j < s.size(); ++j)
      {
         if((s[j] <= UCHAR_MAX) && (s[j] >= 0))
            syntax_[s[j]] = static_cast<unsigned char>(i);
         else
         {
            m.c = s[j];
            m.type = static_cast<unsigned int>(i);
            syntax.push_back(m);
         }
      }
   }

#ifndef BOOST_NO_STD_MESSAGES
   // load any custom collate names:
   string_type c1, c2;
   i = 400;
   if((int)cat >= 0)
   {
      c2 = msgs.get(cat, 0, (int)i, c1);
      while(c2.size())
      {
         const wchar_t* p1, *p2, *p3, *p4;;
         p1 = c2.c_str();
         while(*p1 && BOOST_REGEX_STD isspace((wchar_t)*p1, l))++p1;
         p2 = p1;
         while(*p2 && !BOOST_REGEX_STD isspace((wchar_t)*p2, l))++p2;
         p3 = p2;
         while(*p3 && BOOST_REGEX_STD isspace((wchar_t)*p3, l))++p3;
         p4 = p3;
         while(*p4 && !BOOST_REGEX_STD isspace((wchar_t)*p4, l))++p4;
         collating_elements[std::basic_string<wchar_t>(p1, p2)] = std::basic_string<wchar_t>(p3, p4);

         ++i;
         c2 = msgs.get(cat, 0, (int)i, c1);
      }
   }

   if((int)cat >= 0)
   {
      c2.erase();
      for(i = 0; i < re_classes_max; ++i)
      {
         c1 = msgs.get(cat, 0, static_cast<int>(i+300), c2);
         if(c1.size())
            classes[c1] = i;
      }
      for(i = 0; i <= boost::REG_E_UNKNOWN ; ++i)
      {
         c1 = msgs.get(cat, 0, static_cast<int>(i+200), c2);
         error_strings[i] = to_narrow(c1, cvt);
      }
   }

   if((int)cat >= 0)
      msgs.close(cat);
#endif      
}

} // namespace re_detail

unsigned int BOOST_REGEX_CALL cpp_regex_traits<wchar_t>::do_syntax_type(size_type c)const
{
   std::list<re_detail::message_data<wchar_t>::syntax_map>::const_iterator i, j;
   i = pmd->syntax.begin();
   j = pmd->syntax.end();
   while(i != j)
   {
      if(((uchar_type)(*i).c) == c)
         return (*i).type;
      ++i;
   }
   return 0;
}

void BOOST_REGEX_CALL cpp_regex_traits<wchar_t>::transform_primary(std::basic_string<wchar_t>& out, const std::basic_string<wchar_t>& in)const
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

int BOOST_REGEX_CALL cpp_regex_traits<wchar_t>::toi(wchar_t c)const
{
   pmd->sbuf.pubsetbuf(&c, 1);
   pmd->is.clear();
   pmd->is >> std::dec;
   int val;
   if(pmd->is >> val)
   {
      return val;
   }
   else
      return 0;
}

int BOOST_REGEX_CALL cpp_regex_traits<wchar_t>::toi(const wchar_t*& first, const wchar_t* last, int radix)const
{
   pmd->sbuf.pubsetbuf(const_cast<wchar_t*>(first), static_cast<std::streamsize>(last-first));
   pmd->is.clear();
   if(std::abs(radix) == 16) pmd->is >> std::hex;
   else if(std::abs(radix) == 8) pmd->is >> std::oct;
   else pmd->is >> std::dec;
   int val;
   if(pmd->is >> val)
   {
      first = first + ((last - first) - pmd->sbuf.in_avail());
      return val;
   }
   else
      return 0;
}

boost::uint_fast32_t BOOST_REGEX_CALL cpp_regex_traits<wchar_t>::lookup_classname(const wchar_t* first, const wchar_t* last)const
{
   BOOST_RE_GUARD_STACK
   unsigned int i;
   std::wstring s(first, last);

   std::map<std::wstring, std::size_t>::const_iterator pos = pmd->classes.find(s);
   if(pos != pmd->classes.end())
      return re_char_class_id[(*pos).second];

   std::string ns = re_detail::to_narrow(s, *pcdv);

   for(i = 0; i < re_classes_max; ++i)
   {
      if(ns == re_char_class_names[i])
         return re_char_class_id[i];
   }
   return 0;
}

bool BOOST_REGEX_CALL cpp_regex_traits<wchar_t>::lookup_collatename(std::basic_string<wchar_t>& s, const wchar_t* first, const wchar_t* last)const
{
   BOOST_RE_GUARD_STACK
   std::wstring name(first, last);
   std::map<std::wstring, std::wstring>::const_iterator pos = pmd->collating_elements.find(name);
   if(pos != pmd->collating_elements.end())
   {
      s = (*pos).second;
      return true;
   }
   std::string ns = re_detail::to_narrow(name, *pcdv);
   std::string ns2;
   bool result = re_detail::re_lookup_def_collate_name(ns2, ns.c_str());
   s = re_detail::to_wide(ns2, *pcdv);
   return result;
}

std::string BOOST_REGEX_CALL cpp_regex_traits<wchar_t>::error_string(unsigned id)const
{
   if((id <= boost::REG_E_UNKNOWN) && (pmd->error_strings[id].size()))
      return pmd->error_strings[id];
   return boost::re_detail::re_default_error_messages[id];
}

cpp_regex_traits<wchar_t>::cpp_regex_traits()
{
   pmd = new re_detail::message_data<wchar_t>(locale_inst, std::string(regex_message_cat));
   psyntax = pmd->syntax_;
#ifndef BOOST_NO_EXCEPTIONS
   try{
#endif
      lower_map = new wchar_t[char_set_size];
      BOOST_REGEX_NOEH_ASSERT(lower_map)
#ifndef BOOST_NO_EXCEPTIONS
   }
   catch(...)
   {
      delete pmd;
      throw;
   }
#endif
   for(unsigned int i = 0; i < char_set_size; ++i)
      lower_map[i] = static_cast<wchar_t>(i);
   pctype = &BOOST_USE_FACET(std::ctype<wchar_t>, locale_inst);
   pctype->tolower(&lower_map[0], &lower_map[char_set_size]);
   pcollate = &BOOST_USE_FACET(std::collate<wchar_t>, locale_inst);
   typedef std::codecvt<wchar_t, char, std::mbstate_t> cvt_t;
   pcdv = &BOOST_USE_FACET(cvt_t, locale_inst);
   sort_type = re_detail::find_sort_syntax(this, &(this->sort_delim));
}

cpp_regex_traits<wchar_t>::~cpp_regex_traits()
{
   delete pmd;
   delete[] lower_map;
}

cpp_regex_traits<wchar_t>::locale_type BOOST_REGEX_CALL cpp_regex_traits<wchar_t>::imbue(locale_type l)
{
   locale_type old_l(locale_inst);
   locale_inst = l;
   re_detail::message_data<wchar_t>* npmd = new re_detail::message_data<wchar_t>(locale_inst, std::string(regex_message_cat));
   delete pmd;
   pmd = npmd;
   psyntax = pmd->syntax_;
   for(unsigned int i = 0; i < char_set_size; ++i)
      lower_map[i] = static_cast<wchar_t>(i);
   pctype = &BOOST_USE_FACET(std::ctype<wchar_t>, locale_inst);
   pctype->tolower(&lower_map[0], &lower_map[char_set_size]);
   pcollate = &BOOST_USE_FACET(std::collate<wchar_t>, locale_inst);
   typedef std::codecvt<wchar_t, char, std::mbstate_t> cvt_t;
   pcdv = &BOOST_USE_FACET(cvt_t, locale_inst);
   sort_type = re_detail::find_sort_syntax(this, &(this->sort_delim));
   return old_l;
}

std::size_t BOOST_REGEX_CALL cpp_regex_traits<wchar_t>::strwiden(wchar_t *s1, std::size_t len, const char *s2)const
{
   std::string s(s2);
   std::wstring ws = re_detail::to_wide(s2, *pcdv);
   if(len > ws.size())
      std::wcscpy(s1, ws.c_str());
   return ws.size()+1;
}

#endif // BOOST_NO_WREGEX


} // namespace boost

#endif


