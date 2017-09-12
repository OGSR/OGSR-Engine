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
  *   FILE         partial_regex_match.cpp
  *   VERSION      see <boost/version.hpp>
  *   DESCRIPTION: regex_match example using partial matches.
  */

#include <string>
#include <iostream>
#include <boost/regex.hpp>

boost::regex e("(\\d{3,4})[- ]?(\\d{4})[- ]?(\\d{4})[- ]?(\\d{4})");

bool is_possible_card_number(const std::string& input)
{
   //
   // return false for partial match, true for full match, or throw for
   // impossible match based on what we have so far...
   boost::match_results<std::string::const_iterator> what;
   if(0 == boost::regex_match(input, what, e, boost::match_default | boost::match_partial))
   {
      // the input so far could not possibly be valid so reject it:
      throw std::runtime_error("Invalid data entered - this could not possibly be a valid card number");
   }
   // OK so far so good, but have we finished?
   if(what[0].matched)
   {
      // excellent, we have a result:
      return true;
   }
   // what we have so far is only a partial match...
   return false;
}

int main(int argc, char* argv[])
{
   try{
      std::string input;
      if(argc > 1)
         input = argv[1];
      else
         std::cin >> input;
      if(is_possible_card_number(input))
      {
         std::cout << "Matched OK..." << std::endl;
      }
      else
         std::cout << "Got a partial match..." << std::endl;
   }
   catch(const std::exception& e)
   {
      std::cout << e.what() << std::endl;
   }
   return 0;
}


