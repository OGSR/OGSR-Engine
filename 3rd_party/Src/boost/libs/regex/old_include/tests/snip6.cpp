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

#include <string>
#include <map>
#include <regex>
#include <functional>

// purpose:
// takes the contents of a file in the form of a string
// and searches for all the C++ class definitions, storing
// their locations in a map of strings/int's

typedef std::map<std::string, int, std::less<std::string> > map_type;

class class_index
{
   jm::regex expression;
   map_type index;
   std::string::const_iterator base;

   bool grep_callback(reg_match<std::string::const_iterator, regex::alloc_type> what);
public:
   map_type& get_map() { return index; }
   void IndexClasses(const std::string& file);
   class_index()
      : index(),
        expression("^(template[[:space:]]*<[^;:{]+>[[:space:]]*)?"
                   "(class|struct)[[:space:]]*(\\<\\w+\\>([[:blank:]]*\\([^)]*\\))?"
                   "[[:space:]]*)*(\\<\\w*\\>)[[:space:]]*(<[^;:{]+>[[:space:]]*)?"
                   "(\\{|:[^;\\{()]*\\{)"
                   ){}
};

bool class_index::grep_callback(reg_match<std::string::const_iterator, regex::alloc_type> what)
{
   // what[0] contains the whole string
   // what[5] contains the class name.
   // what[6] contains the template specialisation if any.
   // add class name and position to map:
   index[std::string(what[5].first, what[5].second) + std::string(what[6].first, what[6].second)] =
               what[5].first - base;
   return true;
}

void class_index::IndexClasses(const std::string& file)
{
   std::string::const_iterator start, end;
   start = file.begin();
   end = file.end();
   base = start;
   reg_grep(std::bind1st(std::mem_fun(&class_index::grep_callback), this),
            start,
            end,
            expression,
            match_default);
}



using namespace std;

#include <fileiter.h>
#include <iostream>

int main(int argc, const char** argv)
{
   for(int i = 1; i < argc; ++i)
   {
      cout << "Processing file " << argv[i] << endl;
      mapfile f(argv[i]);
      std::string text(f.begin(), f.end());
      class_index i;
      i.IndexClasses(text);
      cout << i.get_map().size() << " matches found" << endl;
      map_type::iterator c, d;
      c = i.get_map().begin();
      d = i.get_map().end();
      while(c != d)
      {
         cout << "class \"" << (*c).first << "\" found at index: " << (*c).second << endl;
         ++c;
      }
   }
   return 0;
}


