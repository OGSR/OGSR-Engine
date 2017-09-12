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
  *   FILE     jgrep.cpp
  *   VERSION  see <boost/version.hpp>
  */

#include <stdio.h>
#include <boost/regex.hpp>
#ifdef JM_OLD_IOSTREAM
#include <iostream.h>
#else
#include <iostream>
using std::cout;
using std::cin;
using std::cerr;
using std::endl;
#endif
#ifdef __BORLANDC__
#  pragma hrdstop
#endif

#include <boost/regex/v3/fileiter.hpp>

#include "jgrep.h"


//
// class ogrep_predicate
// outputs the results of regex_grep to screen:
template <class iterator, class Allocator >
class ogrep_predicate
{
   unsigned int& lines;
   const char* filename;
   unsigned int last_line;
   iterator end_of_storage;
public:
   ogrep_predicate(unsigned int& i, const char* p, iterator e) : lines(i), filename(p), last_line(-1), end_of_storage(e) {}
   ogrep_predicate(const ogrep_predicate& o) : lines(o.lines), filename(o.filename), last_line(o.last_line), end_of_storage(o.end_of_storage) {}
   bool operator () (const boost::match_results<iterator, Allocator>& i);
};

// ideally we'd ignor the allocator type and use a template member function
// to deel with the allocator type passed to regex_grep, unfortunately most
// compilers don't support this feature yet, so we'll have to be sure that
// the allocator passed to instances of this class match that used in our
// regular expression classes.

template <class iterator, class Allocator>
bool ogrep_predicate<iterator, Allocator>::operator()(const boost::match_results<iterator, Allocator>& i)
{
   if(last_line == (unsigned int)-1)
      cout << "File " << filename << ":" << endl;
   if(last_line != i.line())
   {
      ++lines;
      last_line = i.line();
      if(count_only == 0)
      {
         if(show_lines)
            cout << i.line() << "\t";
         iterator ptr = i.line_start();
         while((ptr != end_of_storage) && (*ptr != '\n'))++ptr;
         iterator pos = i.line_start();
         while(pos != ptr)
         {
            cout.put(*pos);
            ++pos;
         }
         cout << endl;
      }
   }
   return true;
}


void process_grep(const char* file)
{
   try{
   using namespace boost;
   mapfile f(file);
   unsigned int count = 0;
   ogrep_predicate<mapfile::iterator, allocator_type> oi(count, file, f.end());
   if(files_only)
   {
      bool ok;
      boost::match_results<mapfile::iterator, allocator_type> m;
         ok = regex_search(f.begin(), f.end(), m, e, match_not_dot_newline | match_not_dot_null);
      if(ok)
         cout << "File " << file << endl;
   }
   else
   {
      regex_grep(oi, f.begin(), f.end(), e, match_not_dot_newline | match_not_dot_null);
      if(count)
      {
         if(verbose || count_only)
         {
            cout << count << " lines match" << endl;
            return;
         }
      }
      else if(verbose)
      {
         cout << "File " << file << "(" << f.size() << "bytes):" << endl << "0 lines match" << endl;
      }
   }
   }
   catch(const std::exception& e)
   {
      std::cerr << std::endl << e.what() << std::endl;
   }
   catch(...)
   {
   }
}










