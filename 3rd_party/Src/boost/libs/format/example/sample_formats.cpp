// -*- C++ -*-
//  Boost general library 'format'  ---------------------------
//  See http://www.boost.org for updates, documentation, and revision history.

//  (C) Samuel Krempp 2001
//                  krempp@crans.ens-cachan.fr
//  Permission to copy, use, modify, sell and
//  distribute this software is granted provided this copyright notice appears
//  in all copies. This software is provided "as is" without express or implied
//  warranty, and with no claim as to its suitability for any purpose.

// ------------------------------------------------------------------------------
// sample_formats.cc :  example of basic usage of format
// ------------------------------------------------------------------------------

#include <iostream>
#include <iomanip>

#include "boost/format.hpp"

int main(){
    using namespace std;
    using boost::format;
    using boost::io::group;
    using boost::io::str;
    stringstream oss;

    
    // Reordering :
    cout << format("%1% %2% %3% %2% %1% \n") % "o" % "oo" % "O"; // 'simple' style.
    //          prints  "o oo O oo o \n"
    cout << format("(x,y) = (%1$+5d,%2$+5d) \n") % -23 % 35;     // Posix-Printf style


    // No reordering :
    cout << format("writing %s,  x=%s : %d-th step \n") % "toto" % 40.23 % 50; 
    //          prints  "writing toto,  x=40.23 : 50-th step \n"

    cout << format("(x,y) = (%+5d,%+5d) \n") % -23 % 35;
    cout << format("(x,y) = (%|+5|,%|+5|) \n") % -23 % 35;
    cout << format("(x,y) = (%|1$+5|,%|2$+5|) \n") % -23 % 35;
    //   all those are the same,  it prints  "(x,y) = (  -23,  +35) \n"



    // Using manipulators, via 'group' :
    cout << format("%2% %1% %2%\n")  % 1   % group(setfill('X'), hex, setw(4), 16+3) ;
    // prints "XX13 1 XX13\n"


    // printf directives's type-flag can be used to pass formatting options :
    cout <<  format("_%1$4d_ is : _%1$#4x_, _%1$#4o_, and _%1$s_ by default\n")  % 18;
    //          prints  "_  18_ is : _0x12_, _ 022_, and _18_ by default\n"

    // Taking the string value :
    std::string s;
    s= str( format(" %d %d ") % 11 % 22 );
    assert( s == " 11 22 ");


    // -----------------------------------------------
    //  %% prints '%'

    cout << format("%%##%#x ") % 20 << endl;
    //          prints  "%##0x14 "
 

    // -----------------------------------------------
    //    Enforcing the right number of arguments 

    // Too much arguments will throw an exception when feeding the unwanted argument :
    try {
      format(" %1% %1% ") % 101 % 102;
      // the format-string refers to ONE argument, twice. not 2 arguments.
      // thus giving 2 arguments is an error
    }
    catch (boost::io::too_many_args& exc) { 
      cerr <<  exc.what() << "\n\t\t***Dont worry, that was planned\n";
    }

    
    // Too few arguments when requesting the result will also throw an exception :
    try {
      cerr << format(" %|3$| ") % 101; 
      // even if %1$ and %2$ are not used, you should have given 3 arguments
    }
    catch (boost::io::too_few_args& exc) { 
      cerr <<  exc.what() << "\n\t\t***Dont worry, that was planned\n";
    }

    
    cerr << "\n\nEverything went OK, exiting. \n";
    return 0;
}
