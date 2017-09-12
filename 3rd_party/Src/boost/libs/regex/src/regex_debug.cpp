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
  *   FILE:        regex_debug.cpp
  *   VERSION:     see <boost/version.hpp>
  *   DESCRIPTION: Misc. debugging helpers.
  */


#define BOOST_REGEX_SOURCE

#include <boost/regex/config.hpp>

#ifdef BOOST_REGEX_DEBUG

#ifdef BOOST_MSVC
#include <crtdbg.h>
#endif

#include <boost/regex/v3/regex_raw_buffer.hpp>
#include <boost/regex.hpp>

#ifndef BOOST_RE_OLD_IOSTREAM
#include <ostream>
#else 
#include <ostream.h>
#endif

namespace boost { namespace re_detail {
std::ostream& operator<<(std::ostream& s, syntax_element_type x)
{
    return s << static_cast<unsigned long>(x);
}
}} // namespace boost::re_detail


namespace {

char b1[32] = {0,};
char guard_pattern[32] 
= { 0, 1, 2, 3, 4, 5, 6, 7, 8, 9, 10, 11, 12, 13, 14, 15,
16, 17, 18, 19, 20, 21, 22, 23, 24, 25, 26, 27, 28, 29, 30, 31, };
char b2[32] = {0,};

static const int guard_size = 32;

bool check_pattern(void* p)
{
   return p ? memcmp(p, guard_pattern, guard_size) : 0;
}

inline unsigned maxi(unsigned i, unsigned j)
{
   return i < j ? j : i;
}

unsigned int allocated = 0;

struct init
{
   init();
   ~init();
};

init::init()
{ 
#ifdef BOOST_MSVC
   _CrtSetDbgFlag(_CRTDBG_ALLOC_MEM_DF|_CRTDBG_DELAY_FREE_MEM_DF|_CRTDBG_LEAK_CHECK_DF);
#endif
}

init::~init() 
{ 
}


init i;

void* get_mem(size_t n)
{
   ++allocated;
   char* p = (char*)malloc(n + guard_size * 2 + maxi(sizeof(int) + sizeof(void*), boost::re_detail::padding_size) + boost::re_detail::padding_size);
   char* base = p;
   p = (char*)((std::ptrdiff_t)(p + boost::re_detail::padding_mask) & ~boost::re_detail::padding_mask);
   std::memcpy(p + maxi(sizeof(int) + sizeof(void*), boost::re_detail::padding_size), guard_pattern, guard_size);
   std::memcpy(p + maxi(sizeof(int) + sizeof(void*), boost::re_detail::padding_size) + guard_size + n, guard_pattern, guard_size);
   *(int*)p = n;
   *(void**)(p + sizeof(int)) = base;
   return p + guard_size + maxi(sizeof(int) + sizeof(void*), boost::re_detail::padding_size);
}

void free_mem(void* b)
{
   if(b)
   {
      char* p = (char*)b;
      p -= (guard_size + maxi(sizeof(int) + sizeof(void*), boost::re_detail::padding_size));
      if(check_pattern(p + maxi(sizeof(int) + sizeof(void*), boost::re_detail::padding_size)) || check_pattern(p + maxi(sizeof(int) + sizeof(void*), boost::re_detail::padding_size) + guard_size + *(int*)p))
      {
         cerr << "Error: freed memory has been written past end..." << endl;
      }
      free(*(void**)(p + sizeof(int)));
      --allocated;
   }
}

} // namespace

void* operator new(size_t n)
{
   return get_mem(n);
}

void* operator new[](size_t n)
{
   return get_mem(n);
}

void operator delete(void* p)
{
   free_mem(p);
}

void operator delete[](void* p)
{
   free_mem(p);
}

#include <set>

namespace boost{
   namespace re_detail{

std::set<debug_guard*, std::less<debug_guard*> >* patterns = 0;

int pattern_count = 0;

void check_patterns(const char* f, int l)
{
   if(pattern_count)
   {
      std::set<debug_guard*, std::less<debug_guard*> >::iterator i, j;
      i = patterns->begin();
      j = patterns->end();
      while(i != j)
      {
         if(check_pattern((void*)(*i)->g1) || check_pattern((*i)->g2) || check_pattern((void*)(*i)->pc) || check_pattern((*i)->pnc))
         {
            cerr << "Error: static memory corruption at " << hex << *i << dec << " in " << (*i)->file << "@" << (*i)->line << " called from " << f << "@" << l << endl;
         }
         ++i;
      }
   }
}

debug_guard::debug_guard(const char* f, int l, const char* p1, char* p2)
{
   if(++pattern_count == 1)
      patterns = new std::set<debug_guard*, std::less<debug_guard*> >;
   file = f;
   line = l;
   std::memcpy(g1, guard_pattern, guard_size);
   std::memcpy(g2, guard_pattern, guard_size);
   if(p1)
   {
      pc = p1;
   }
   else
      pc = 0;
   if(p2)
   {
      pnc = p2;
      std::memcpy(pnc, guard_pattern, guard_size);
   }
   else
      pnc = 0;
   patterns->insert(this);
}

debug_guard::~debug_guard()
{
   check_patterns(file, line);
   if(check_pattern(g1) || check_pattern(g2))
   {
      cerr << "Error: memory corruption " << file << "@" << line << endl;
   }
   patterns->erase(this);
   if(--pattern_count == 0)
   {
      delete patterns;
      patterns = 0;
   }
}

   } // namespace re_detail

} // namespace boost



#endif




