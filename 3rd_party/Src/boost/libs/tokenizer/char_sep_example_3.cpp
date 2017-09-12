// © Copyright Jeremy Siek 2002. 

// Permission to copy, use, modify, sell and distribute this software
// is granted provided this copyright notice appears in all
// copies. This software is provided "as is" without express or
// implied warranty, and with no claim as to its suitability for any
// purpose.

// Sample output:
// <This> <is> <,> <a> <test> 

// char_sep_example_3.cpp
#include <iostream>
#include <boost/tokenizer.hpp>
#include <string>

int main()
{
   std::string str = "This is,  a test";
   typedef boost::tokenizer<boost::char_separator<char> > Tok;
   boost::char_separator<char> sep; // default constructed
   Tok tok(str, sep);
   for(Tok::iterator tok_iter = tok.begin(); tok_iter != tok.end(); ++tok_iter)
     std::cout << "<" << *tok_iter << "> ";
   std::cout << "\n";
   return EXIT_SUCCESS;
}

