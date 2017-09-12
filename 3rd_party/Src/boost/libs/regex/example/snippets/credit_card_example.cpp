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
  *   FILE         credit_card_example.cpp
  *   VERSION      see <boost/version.hpp>
  *   DESCRIPTION: Credit card number formatting code.
  */

#include <string>
#include <boost/regex.hpp>

bool validate_card_format(const std::string& s)
{
   static const boost::regex e("(\\d{4}[- ]){3}\\d{4}");
   return boost::regex_match(s, e);
}

const boost::regex e("\\A(\\d{3,4})[- ]?(\\d{4})[- ]?(\\d{4})[- ]?(\\d{4})\\z");
const std::string machine_format("\\1\\2\\3\\4");
const std::string human_format("\\1-\\2-\\3-\\4");

std::string machine_readable_card_number(const std::string& s)
{
   return boost::regex_merge(s, e, machine_format, boost::match_default | boost::format_sed);
}

std::string human_readable_card_number(const std::string& s)
{
   return boost::regex_merge(s, e, human_format, boost::match_default | boost::format_sed);
}

#include <iostream>
using namespace std;

int main()
{
   string s[4] = { "0000111122223333", "0000 1111 2222 3333",
                   "0000-1111-2222-3333", "000-1111-2222-3333", };
   int i;
   for(i = 0; i < 4; ++i)
   {
      cout << "validate_card_format(\"" << s[i] << "\") returned " << validate_card_format(s[i]) << endl;
   }
   for(i = 0; i < 4; ++i)
   {
      cout << "machine_readable_card_number(\"" << s[i] << "\") returned " << machine_readable_card_number(s[i]) << endl;
   }
   for(i = 0; i < 4; ++i)
   {
      cout << "human_readable_card_number(\"" << s[i] << "\") returned " << human_readable_card_number(s[i]) << endl;
   }
   return 0;
}



