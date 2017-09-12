// (C) Copyright Jeremy Siek 2001. Permission to copy, use, modify,
// sell and distribute this software is granted provided this
// copyright notice appears in all copies. This software is provided
// "as is" without express or implied warranty, and with no claim as
// to its suitability for any purpose.

// Revision History:

// 27 Feb 2001   Jeremy Siek
//      Initial checkin.

#include <iostream>
#include <string>
#include <vector>

#include <boost/function_output_iterator.hpp>

struct string_appender {
  string_appender(std::string& s) : m_str(s) { }
  void operator()(const std::string& x) const {
    m_str += x;
  }
  std::string& m_str;
};

int main(int, char*[])
{
  std::vector<std::string> x;
  x.push_back("hello");
  x.push_back(" ");
  x.push_back("world");
  x.push_back("!");

  std::string s = "";
  std::copy(x.begin(), x.end(), 
            boost::make_function_output_iterator(string_appender(s)));
  
  std::cout << s << std::endl;

  return 0;
}
