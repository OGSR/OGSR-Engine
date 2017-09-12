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
  *   FILE         regex_match_example.cpp
  *   VERSION      see <boost/version.hpp>
  *   DESCRIPTION: ftp based regex_match example.
  */
  
#include <cstdlib>
#include <boost/regex.hpp>
#include <string>
#include <iostream>

using namespace std;
using namespace boost;

regex expression("^([0-9]+)(\\-| |$)(.*)$");

// process_ftp:
// on success returns the ftp response code, and fills
// msg with the ftp response message.
int process_ftp(const char* response, std::string* msg)
{
   cmatch what;
   if(regex_match(response, what, expression))
   {
      // what[0] contains the whole string
      // what[1] contains the response code
      // what[2] contains the separator character
      // what[3] contains the text message.
      if(msg)
         msg->assign(what[3].first, what[3].second);
      return atoi(what[1].first);
   }
   // failure did not match
   if(msg)
      msg->erase();
   return -1;
}

#if defined(BOOST_MSVC) || (defined(__BORLANDC__) && (__BORLANDC__ == 0x550))
//
// problem with std::getline under MSVC6sp3
istream& getline(istream& is, std::string& s)
{
   s.erase();
   char c = is.get();
   while(c != '\n')
   {
      s.append(1, c);
      c = is.get();
   }
   return is;
}
#endif

int main(int argc)
{
   std::string in, out;
   do
   {
      if(argc == 1)
      {
         cout << "enter test string" << endl;
         getline(cin, in);
         if(in == "quit")
            break;
      }
      else
         in = "100 this is an ftp message text";
      int result;
      result = process_ftp(in.c_str(), &out);
      if(result != -1)
      {
         cout << "Match found:" << endl;
         cout << "Response code: " << result << endl;
         cout << "Message text: " << out << endl;
      }
      else
      {
         cout << "Match not found" << endl;
      }
      cout << endl;
   } while(argc == 1);
   return 0;
}






