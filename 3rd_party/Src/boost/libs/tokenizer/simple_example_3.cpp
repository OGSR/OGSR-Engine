// © Copyright John R. Bandela 2001. 

// Permission to copy, use, modify, sell and distribute this software
// is granted provided this copyright notice appears in all
// copies. This software is provided "as is" without express or
// implied warranty, and with no claim as to its suitability for any
// purpose.

// See http://www.boost.org/libs/tokenizer for documenation

// simple_example_3.cpp
#include<iostream>
#include<boost/tokenizer.hpp>
#include<string>

int main(){
   using namespace std;
   using namespace boost;
   string s = "12252001";
   int offsets[] = {2,2,4};
   offset_separator f(offsets, offsets+3);
   tokenizer<offset_separator> tok(s,f);
   for(tokenizer<offset_separator>::iterator beg=tok.begin(); beg!=tok.end();++beg){
       cout << *beg << "\n";
   }
}
