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
  *   FILE:        primary_transform.hpp
  *   VERSION:     see <boost/version.hpp>
  *   DESCRIPTION: Heuristically determines the sort string format in use
  *                by the current locale.
  */

namespace boost{
   namespace re_detail{


enum{
   sort_C,
   sort_fixed,
   sort_delim,
   sort_unknown
};

template <class S, class charT>
unsigned count_chars(const S& s, charT c)
{
   unsigned int count = 0;
   for(unsigned pos = 0; pos < s.size(); ++pos)
   {
      if(s[pos] == c) ++count;
   }
   return count;
}


template <class traits, class charT>
unsigned find_sort_syntax(const traits* pt, charT* delim)
{
   //
   // compare 'a' with 'A' to see how similar they are,
   // should really use a-accute but we can't portably do that,
   //
   typedef typename traits::string_type string_type;
   typedef typename traits::char_type char_type;

   // Suppress incorrect warning for MSVC
   (void)pt;

   string_type a(1, (char_type)'a');
   string_type sa;
   pt->transform(sa, a);
   if(sa == a)
   {
      *delim = 0;
      return sort_C;
   }
   string_type A(1, (char_type)'A');
   string_type sA;
   pt->transform(sA, A);
   string_type c(1, (char_type)';');
   string_type sc;
   pt->transform(sc, c);

   int pos = 0;
   while((pos <= static_cast<int>(sa.size())) && (pos <= static_cast<int>(sA.size())) && (sa[pos] == sA[pos])) ++pos;
   --pos;
   if(pos < 0)
   {
      *delim = 0;
      return sort_unknown;
   }
   //
   // at this point sa[pos] is either the end of a fixed with field
   // or the character that acts as a delimiter:
   //
   charT maybe_delim = sa[pos];
   if((pos != 0) && (count_chars(sa, maybe_delim) == count_chars(sA, maybe_delim)) && (count_chars(sa, maybe_delim) == count_chars(c, maybe_delim)))
   {
      *delim = maybe_delim;
      return sort_delim;
   }
   //
   // OK doen't look like a delimiter, try for fixed width field:
   //
   if((sa.size() == sA.size()) && (sa.size() == c.size()))
   {
      // note assumes that the fixed width field is less than
      // numeric_limits<charT>::max(), should be true for all types
      // I can't imagine 127 character fields...
      *delim = static_cast<charT>(++pos);
      return sort_fixed;
   }
   //
   // don't know what it is:
   //
   *delim = 0;
   return sort_unknown;
}


   } // namespace re_detail
} // namespace boost







