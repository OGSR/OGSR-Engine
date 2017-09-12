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
  *   FILE         partial_regex_grep.cpp
  *   VERSION      see <boost/version.hpp>
  *   DESCRIPTION: Search example using partial matches.
  */

#include <iostream>
#include <fstream>
#include <sstream>
#include <string>
#include <boost/regex.hpp>

// match some kind of html tag:
boost::regex e("<[^>]*>");
// count how many:
unsigned int tags = 0;
// saved position of partial match:
const char* next_pos = 0;

bool grep_callback(const boost::match_results<const char*>& m)
{
   if(m[0].matched == false)
   {
      // save position and return:
      next_pos = m[0].first;
   }
   else
      ++tags;
   return true;
}

void search(std::istream& is)
{
   char buf[4096];
   next_pos = buf + sizeof(buf);
   bool have_more = true;
   while(have_more)
   {
      // how much do we copy forward from last try:
      unsigned leftover = (buf + sizeof(buf)) - next_pos;
      // and how much is left to fill:
      unsigned size = next_pos - buf;
      // copy forward whatever we have left:
      memmove(buf, next_pos, leftover);
      // fill the rest from the stream:
      is.read(buf + leftover, size);
      unsigned read = is.gcount();
      // check to see if we've run out of text:
      have_more = read == size;
      // reset next_pos:
      next_pos = buf + sizeof(buf);
      // and then grep:
      boost::regex_grep<bool(*)(const boost::cmatch&), const char*>(grep_callback,
                        static_cast<const char*>(buf),
                        static_cast<const char*>(buf + read + leftover),
                        e,
                        boost::match_default | boost::match_partial);
   }
}

int main(int argc, char* argv[])
{
   if(argc > 1)
   {
      for(unsigned int i = 1; i < argc; ++i)
      {
         std::ifstream fs(argv[i]);
         if(fs.bad()) continue;
         search(fs);
      }
   }
   else
   {
      std::string one("<META NAME=\"keywords\" CONTENT=\"regex++, regular expressions, regular expression library, C++\">");
      std::string what;
      while(what.size() < 10000)
      {
         what.append(one);
         what.append(13, ' ');
      }
      std::stringstream ss;
      ss.str(what);
      search(ss);
   }
   std::cout << "total tag count was " << tags << std::endl;
   return 0;
}



