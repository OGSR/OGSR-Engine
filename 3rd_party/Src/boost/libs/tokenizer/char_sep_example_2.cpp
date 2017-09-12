// © Copyright Jeremy Siek 2002. 

// Permission to copy, use, modify, sell and distribute this software
// is granted provided this copyright notice appears in all
// copies. This software is provided "as is" without express or
// implied warranty, and with no claim as to its suitability for any
// purpose.

// Sample output:
//
// <> <> <Hello> <|> <world> <|> <> <|> <> <foo> <> <bar> <yow> <baz> <|> <>

// char_sep_example_2.cpp
#include <iostream>
#include <boost/tokenizer.hpp>
#include <string>

int main()
{
  std::string str = ";;Hello|world||-foo--bar;yow;baz|";

  typedef boost::tokenizer<boost::char_separator<char> > 
    tokenizer;
  boost::char_separator<char> sep("-;", "|", boost::keep_empty_tokens);
  tokenizer tokens(str, sep);
  for (tokenizer::iterator tok_iter = tokens.begin();
       tok_iter != tokens.end(); ++tok_iter)
    std::cout << "<" << *tok_iter << "> ";
  std::cout << "\n";
  return EXIT_SUCCESS;
}
