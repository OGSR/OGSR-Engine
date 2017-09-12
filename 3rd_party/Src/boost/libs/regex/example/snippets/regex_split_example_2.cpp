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
  *   FILE         regex_split_example_2.cpp
  *   VERSION      see <boost/version.hpp>
  *   DESCRIPTION: regex_split example: spit out linked URL's.
  */


#include <list>
#include <fstream>
#include <iostream>
#include <iterator>
#include <boost/regex.hpp>

boost::regex e("<\\s*A\\s+[^>]*href\\s*=\\s*\"([^\"]*)\"",
               boost::regbase::normal | boost::regbase::icase);

void load_file(std::string& s, std::istream& is)
{
   s.erase();
   //
   // attempt to grow string buffer to match file size,
   // this doesn't always work...
   s.reserve(is.rdbuf()->in_avail());
   char c;
   while(is.get(c))
   {
      // use logarithmic growth stategy, in case
      // in_avail (above) returned zero:
      if(s.capacity() == s.size())
         s.reserve(s.capacity() * 3);
      s.append(1, c);
   }
}

int main(int argc, char** argv)
{
   std::string s;
   std::list<std::string> l;
   int i;
   for(i = 1; i < argc; ++i)
   {
      std::cout << "Findings URL's in " << argv[i] << ":" << std::endl;
      s.erase();
      std::ifstream is(argv[i]);
      load_file(s, is);
      boost::regex_split(std::back_inserter(l), s, e);
      while(l.size())
      {
         s = *(l.begin());
         l.pop_front();
         std::cout << s << std::endl;
      }
   }
   //
   // alternative method:
   // split one match at a time and output direct to
   // cout via ostream_iterator<std::string>....
   //
   for(i = 1; i < argc; ++i)
   {
      std::cout << "Findings URL's in " << argv[i] << ":" << std::endl;
      s.erase();
      std::ifstream is(argv[i]);
      load_file(s, is);
      while(boost::regex_split(std::ostream_iterator<std::string>(std::cout), s, e, boost::match_default, 1)) std::cout << std::endl;
   }

   return 0;
}


