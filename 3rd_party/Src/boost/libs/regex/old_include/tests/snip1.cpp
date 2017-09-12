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

#include <stdlib.h>
#include <string>
#include <iostream>
#include <regex>

using namespace std;

regex expression("^([0-9]+)(\\-| |$)(.*)$");

// process_ftp:
// on success returns the ftp response code, and fills
// msg with the ftp response message.
int process_ftp(const char* response, std::string* msg)
{
   cmatch what;
   if(query_match(response, response + strlen(response), what, expression))
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

int main()
{
  std::string in, out;
  while(true)
  {
  cout << "enter test string" << endl;
  std::getline(cin, in);
  if(in == "quit")
     break;
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
  }
  return 0;
}






