/*
 *
 * Copyright (c) 1998-2000
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
  *   FILE     regex.h
  *   VERSION  see <boost/version.hpp>
  *   deprecated regular expression matching algorithms
  */

#ifndef BOOST_OLD_REGEX_H
#define BOOST_OLD_REGEX_H

#include <cregex.h>
#include <boost/regex.hpp>

namespace boost{

namespace deprecated{
//
// class reg_match:
// old name for match_results, this ones just a thin wrapper:
//
template <class iterator, class Allocator  = BOOST_DEFAULT_ALLOCATOR(typename boost::re_detail::def_alloc_param_traits<iterator>::type)>
class reg_match : public boost::match_results<iterator, Allocator>
{
   typedef boost::match_results<iterator, Allocator> base_type;
   typedef boost::re_detail::match_results_base<iterator, Allocator> ancestor;
public:
   reg_match(const Allocator& a = Allocator())
      : base_type(a){}

   reg_match(const ancestor& m)
      : base_type(m){}

   reg_match& operator=(const ancestor& m)
   {
      // shallow copy
      ancestor::operator=(m);
      return *this;
   }

   reg_match(const base_type& m)
        : base_type(m){}
   reg_match& operator=(const base_type& m)
   {
      // deep copy
      base_type::operator=(m);
      return *this;
   }
};
//
// proc query_match
// returns true if the specified regular expression matches
// at position first.  Fills in what matched in m.
//
template <class iterator, class Allocator, class charT, class traits, class Allocator2>
bool query_match(iterator first, iterator last, boost::match_results<iterator, Allocator>& m, const reg_expression<charT, traits, Allocator2>& e, unsigned flags = match_default)
{
   return regex_search(first, last, m, e, flags | match_continuous);
}

//
// query_match convenience interfaces:
#ifndef BOOST_NO_FUNCTION_TEMPLATE_ORDERING
//
// this isn't really a partial specialisation, but template function
// overloading - if the compiler doesn't support partial specialisation
// then it really won't support this either:
template <class charT, class Allocator, class traits, class Allocator2>
inline bool query_match(const charT* str, 
                        boost::match_results<const charT*, Allocator>& m, 
                        const reg_expression<charT, traits, Allocator2>& e, 
                        unsigned flags = match_default)
{
   return query_match(str, str + traits::length(str), m, e, flags);
}

template <class ST, class SA, class Allocator, class charT, class traits, class Allocator2>
inline bool query_match(const std::basic_string<charT, ST, SA>& s, 
                 boost::match_results<typename std::basic_string<charT, ST, SA>::const_iterator, Allocator>& m, 
                 const reg_expression<charT, traits, Allocator2>& e, 
                 unsigned flags = match_default)
{
   return query_match(s.begin(), s.end(), m, e, flags);
}
#else  // partial ordering
inline bool query_match(const char* str, 
                        cmatch& m, 
                        const regex& e, 
                        unsigned flags = match_default)
{
   return query_match(str, str + regex::traits_type::length(str), m, e, flags);
}
#ifndef BOOST_NO_WREGEX
inline bool query_match(const wchar_t* str, 
                        wcmatch& m, 
                        const wregex& e, 
                        unsigned flags = match_default)
{
   return query_match(str, str + wregex::traits_type::length(str), m, e, flags);
}
#endif
inline bool query_match(const std::string& s, 
                        boost::match_results<std::string::const_iterator, regex::allocator_type>& m,
                        const regex& e, 
                        unsigned flags = match_default)
{
   return query_match(s.begin(), s.end(), m, e, flags);
}
#if !defined(BOOST_NO_WREGEX)
inline bool query_match(const std::basic_string<wchar_t>& s, 
                        boost::match_results<std::basic_string<wchar_t>::const_iterator, wregex::allocator_type>& m,
                        const wregex& e, 
                        unsigned flags = match_default)
{
   return query_match(s.begin(), s.end(), m, e, flags);
}
#endif

#endif


template <class iterator, class Allocator, class charT, class traits, class Allocator2>
bool reg_search(iterator first, iterator last, boost::match_results<iterator, Allocator>& m, const reg_expression<charT, traits, Allocator2>& e, unsigned flags = match_default)
{
   return boost::regex_search(first, last, m, e, flags);
}

//
// reg_search convenience interfaces:
#ifndef BOOST_NO_FUNCTION_TEMPLATE_ORDERING
//
// this isn't really a partial specialisation, but template function
// overloading - if the compiler doesn't support partial specialisation
// then it really won't support this either:
template <class charT, class Allocator, class traits, class Allocator2>
inline bool reg_search(const charT* str, 
                        boost::match_results<const charT*, Allocator>& m, 
                        const reg_expression<charT, traits, Allocator2>& e, 
                        unsigned flags = match_default)
{
   return reg_search(str, str + traits::length(str), m, e, flags);
}

#ifndef BOOST_RE_NO_STRING_H
template <class ST, class SA, class Allocator, class charT, class traits, class Allocator2>
inline bool reg_search(const std::basic_string<charT, ST, SA>& s, 
                 boost::match_results<typename std::basic_string<charT, ST, SA>::const_iterator, Allocator>& m, 
                 const reg_expression<charT, traits, Allocator2>& e, 
                 unsigned flags = match_default)
{
   return reg_search(s.begin(), s.end(), m, e, flags);
}
#endif
#else  // partial specialisation
inline bool reg_search(const char* str, 
                        cmatch& m, 
                        const regex& e, 
                        unsigned flags = match_default)
{
   return reg_search(str, str + regex::traits_type::length(str), m, e, flags);
}
#ifndef BOOST_NO_WREGEX
inline bool reg_search(const wchar_t* str, 
                        wcmatch& m, 
                        const wregex& e, 
                        unsigned flags = match_default)
{
   return reg_search(str, str + wregex::traits_type::length(str), m, e, flags);
}
#endif
#ifndef BOOST_RE_NO_STRING_H
inline bool reg_search(const std::string& s, 
                        boost::match_results<std::string::const_iterator, regex::allocator_type>& m,
                        const regex& e, 
                        unsigned flags = match_default)
{
   return reg_search(s.begin(), s.end(), m, e, flags);
}
#if !defined(BOOST_NO_WREGEX)
inline bool reg_search(const std::basic_string<wchar_t>& s, 
                        boost::match_results<std::basic_string<wchar_t>::const_iterator, wregex::allocator_type>& m,
                        const wregex& e, 
                        unsigned flags = match_default)
{
   return reg_search(s.begin(), s.end(), m, e, flags);
}
#endif

#endif

#endif


//
// reg_grep:
// find all non-overlapping matches within the sequence first last:
//
template <class Predicate, class iterator, class charT, class traits, class Allocator>
inline unsigned int reg_grep(Predicate foo, iterator first, iterator last, const reg_expression<charT, traits, Allocator>& e, unsigned flags = match_default)
{
   return boost::regex_grep(foo, first, last, e, flags);
}

//
// reg_grep convenience interfaces:
#ifndef BOOST_NO_FUNCTION_TEMPLATE_ORDERING
//
// this isn't really a partial specialisation, but template function
// overloading - if the compiler doesn't support partial specialisation
// then it really won't support this either:
template <class Predicate, class charT, class Allocator, class traits>
inline unsigned int reg_grep(Predicate foo, const charT* str, 
                        const reg_expression<charT, traits, Allocator>& e, 
                        unsigned flags = match_default)
{
   return reg_grep(foo, str, str + traits::length(str), e, flags);
}

#ifndef BOOST_RE_NO_STRING_H
template <class Predicate, class ST, class SA, class Allocator, class charT, class traits>
inline unsigned int reg_grep(Predicate foo, const std::basic_string<charT, ST, SA>& s, 
                 const reg_expression<charT, traits, Allocator>& e, 
                 unsigned flags = match_default)
{
   return reg_grep(foo, s.begin(), s.end(), e, flags);
}
#endif
#else  // partial specialisation
inline unsigned int reg_grep(bool (*foo)(const cmatch&), const char* str, 
                        const regex& e, 
                        unsigned flags = match_default)
{
   return reg_grep(foo, str, str + regex::traits_type::length(str), e, flags);
}
#ifndef BOOST_NO_WREGEX
inline unsigned int reg_grep(bool (*foo)(const wcmatch&), const wchar_t* str, 
                        const wregex& e, 
                        unsigned flags = match_default)
{
   return reg_grep(foo, str, str + wregex::traits_type::length(str), e, flags);
}
#endif
#ifndef BOOST_RE_NO_STRING_H
inline unsigned int reg_grep(bool (*foo)(const boost::match_results<std::string::const_iterator, regex::allocator_type>&), const std::string& s,
                        const regex& e, 
                        unsigned flags = match_default)
{
   return reg_grep(foo, s.begin(), s.end(), e, flags);
}
#if !defined(BOOST_NO_WREGEX)
inline unsigned int reg_grep(bool (*foo)(const boost::match_results<std::basic_string<wchar_t>::const_iterator, wregex::allocator_type>&),
                     const std::basic_string<wchar_t>& s, 
                        const wregex& e, 
                        unsigned flags = match_default)
{
   return reg_grep(foo, s.begin(), s.end(), e, flags);
}
#endif

#endif

#endif

//
// finally for compatablity with version 1.x of the library
// we need a form of reg_grep that takes an output iterator
// as its first argument:
//

//
// struct grep_match:
// stores what matched during a reg_grep,
// the output iterator type passed to reg_grep must have an
// operator*() that returns a type with an
// operator=(const grep_match<iterator, Allocator>&);
//
template <class iterator, class Allocator>
struct grep_match
{
   unsigned int line;
   iterator line_start;
   reg_match<iterator, Allocator> what;

   grep_match(Allocator a = Allocator()) : what(a) {}

   grep_match(unsigned int l, iterator p1, const boost::match_results<iterator, Allocator>& m)
      : what(m) { line = l; line_start = p1; }

   bool operator == (const grep_match& )
   { return false; }

   bool operator < (const grep_match&)
   { return false; }
};

namespace re_detail{

template <class O, class I, class A>
struct grep_adaptor
{
   O oi;
   reg_match<I, A> m;
   grep_adaptor(O i, A a) : m(a), oi(i) {}
   bool operator()(const boost::re_detail::match_results_base<I, A>& w)
   {
      m.what = w;
      m.line = w.line();
      m.line_start = w.line_start();
      *oi = m;
      ++oi;
      return true;
   }
};

} // namespace re_detail

template <class Out, class iterator, class charT, class traits, class Allocator>
inline unsigned int reg_grep_old(Out oi, iterator first, iterator last, const reg_expression<charT, traits, Allocator>& e, unsigned flags = match_default)
{
   return boost::regex_grep(re_detail::grep_adaptor<Out, iterator, Allocator>(oi, e.allocator()), first, last, e, flags);
}

template <class OutputIterator, class iterator, class Allocator, class charT>
OutputIterator BOOST_REGEX_CALL reg_format(OutputIterator out,
                          const boost::match_results<iterator, Allocator>& m,
                          const charT* fmt)
{
   //
   // start by updating the locale:
   //
   return boost::re_detail::_reg_format_aux(out, m, fmt, 0);
}

template <class iterator, class Allocator, class charT>
std::basic_string<charT> BOOST_REGEX_CALL reg_format(const boost::match_results<iterator, Allocator>& m, const charT* fmt)
{
   std::basic_string<charT> result;
   boost::re_detail::string_out_iterator<std::basic_string<charT> > i(result);
   reg_format(i, m, fmt);
   return result;
}

template <class OutputIterator, class iterator, class traits, class Allocator, class charT>
OutputIterator BOOST_REGEX_CALL reg_merge(OutputIterator out, 
                         iterator first,
                         iterator last,
                         const reg_expression<charT, traits, Allocator>& e, 
                         const charT* fmt, 
                         bool copy = true, 
                         unsigned int flags = match_default)
{
   //
   // start by updating the locale:
   //
   iterator l = first;
   if(!copy) flags |= format_no_copy;
   boost::re_detail::merge_out_predicate<OutputIterator, iterator, charT, Allocator, traits> oi(out, l, fmt, flags, e.get_traits());
   regex_grep(oi, first, last, e, flags);
   return copy ? boost::re_detail::re_copy_out(out, l, last) : out;
}

template <class traits, class Allocator, class charT>
std::basic_string<charT> BOOST_REGEX_CALL reg_merge(const std::basic_string<charT>& s,
                         const reg_expression<charT, traits, Allocator>& e,
                         const charT* fmt, 
                         bool copy = true, 
                         unsigned int flags = match_default)
{
   std::basic_string<charT> result;
   boost::re_detail::string_out_iterator<std::basic_string<charT> > i(result);
   reg_merge(i, s.begin(), s.end(), e, fmt, copy, flags);
   return result;
}

} // namespace deprecated

using deprecated::query_match;
using deprecated::reg_search;
using deprecated::reg_grep;
using deprecated::reg_format;
using deprecated::reg_merge;
//using re_detail::jm_def_alloc;
using deprecated::char_regex_traits_i;
//using re_detail::re_alloc_binder;
using re_detail::padding_size;


} // namspace boost

namespace jm = boost;

#if !defined(BOOST_RE_NO_NAMESPACES) && !defined(BOOST_RE_NO_USING)

using boost::bad_expression;
template<class charT>
struct char_regex_traits : boost::regex_traits<charT>{};
using boost::deprecated::char_regex_traits_i;
using boost::regbase;
using boost::reg_expression;
using boost::deprecated::reg_match;
//using boost::reg_match_base;
using boost::sub_match;
using boost::regex;
using boost::cmatch;
#ifndef BOOST_NO_WREGEX
using boost::wregex;
using boost::wcmatch;
#endif
using boost::deprecated::reg_match;
using boost::deprecated::query_match;
using boost::deprecated::reg_search;
using boost::deprecated::reg_grep;
using boost::deprecated::reg_format;
using boost::deprecated::reg_merge;
//using boost::re_detail::jm_def_alloc;

#endif

#ifdef BOOST_RE_USING_HACK
using namespace boost;
using namespace boost::deprecated;
#endif



#endif
