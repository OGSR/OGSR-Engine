// -*- C++ -*-
//  Boost general library 'format'   ---------------------------
//  See http://www.boost.org for updates, documentation, and revision history.

//  (C) Samuel Krempp 2001
//                  krempp@crans.ens-cachan.fr
//  Permission to copy, use, modify, sell and
//  distribute this software is granted provided this copyright notice appears
//  in all copies. This software is provided "as is" without express or implied
//  warranty, and with no claim as to its suitability for any purpose.

#include "boost/format.hpp"

#include <iostream> 
#include <iomanip>

#define BOOST_INCLUDE_MAIN 
#include <boost/test/test_tools.hpp>


struct Rational {
  int n,d;
  Rational (int an, int ad) : n(an), d(ad) {}
};

std::ostream& operator<<( std::ostream& os, const Rational& r) {
  os << r.n << "/" << r.d;
  return os;
}


int test_main(int, char* [])
{
    using namespace std;
    using boost::format;
    using boost::io::group;
    using boost::io::str;

    Rational r(16,9);

    string s;
    s = str(format("%5%. %5$=6s . %1% format %5%, c'%3% %1% %2%.\n") 
            % "le" % "bonheur" % "est" % "trop" % group(setfill('_'), "bref") );

    if(s  != "bref. _bref_ . le format bref, c'est le bonheur.\n") {
      cerr << s;
      BOOST_ERROR("centered alignement : formatting result incorrect");
    }


    s = str(format("%+8d %-8d\n") % r % r );
    if(s  != "  +16/+9 16/9    \n") {
      cerr << s;  
      BOOST_ERROR("(user-type) formatting result incorrect");
    }

    s = str(format("%0+4d %0+8d %-08d\n") % 8 % r % r);
    if(s  != "+008 +0016/+9 16/9    \n") {
      cerr << s;  
      BOOST_ERROR("(zero-padded user-type) formatting result incorrect");
    }


    s = str( format("%1%, %20T_ (%|2$5|,%|3$5|)\n") % "98765" % 1326 % 88 ) ;
    if( s != "98765, _____________ ( 1326,   88)\n" ) 
            BOOST_ERROR("(tabulation) formatting result incorrect");
    s = str( format("%s, %|20t|=") % 88 ) ;
    if( s != "88,                 =" ) {
      cout << s << endl;
      BOOST_ERROR("(tabulation) formatting result incorrect");
    }


    s = str(format("%.2s %8c.\n") % "root" % "user" );
    if(s  != "ro        u.\n") {
      cerr << s;  
      BOOST_ERROR("(truncation) formatting result incorrect");
    }


    // nesting formats :
    s = str( format("%2$014x [%1%] %|2$05|\n") % (format("%05s / %s") % -18 % 7)
             %group(showbase, -100)
             );
    if( s != "0x0000ffffff9c [-0018 / 7] -0100\n" ){
      cerr << s ;
      BOOST_ERROR("nesting did not work");
    }

   // width in format-string is overridden by setw manipulator :
    s = str( format("%|1$4| %|1$|") % group(setfill('0'), setw(6), 1) );
    if( s!= "000001 000001")
      BOOST_ERROR("width in format VS in argument misbehaved");

    s = str( format("%|=s|") % group(setfill('_'), setw(6), r) );
    if( s!= "_16/9_") {
      cerr << s << endl;
      BOOST_ERROR("width in group context is not handled correctly");
    }


    // options that uses internal alignment : + 0 #
    s = str( format("%+6d %0#6x %s\n")  % 342 % 33 % "ok" );
    if( s !="  +342 0x0021 ok\n")
      BOOST_ERROR("(flags +, 0, or #) formatting result incorrect");

    // flags in the format string are not sticky
    // and hex in argument overrrides type-char d (->decimal) :
    s = str( format("%2$#4d %|1$4| %|2$#4| %|3$|")  
             % 101
             % group(setfill('_'), hex, 2)
             % 103 );
    if(s != "_0x2  101 _0x2 103")
      BOOST_ERROR("formatting error. (not-restoring state ?)");


    // special paddings 
    s = str( format("[%=6s] [%+6s] [%+6s] [% 6s] [%+6s]\n") 
                      % 123
                      % group(internal, setfill('W'), 234)
                      % group(internal, setfill('X'), -345)
                      % group(setfill('Y'), 456)
                      % group(setfill('Z'), -10 ) );

    if(s != "[  123 ] [+WW234] [-XX345] [YY 456] [ZZZ-10]\n" ) {
      cerr << s ;
      BOOST_ERROR("formatting error. (with special paddings)");
    }


    // flag '0' is tricky . 
    // left-align cancels '0':
    s = str( format("%2$0#12X %2$0#-12d %1$0#10d \n") % -20 % 10 );
    if( s != "0X000000000A 10           -000000020 \n"){
      cerr << s;
      BOOST_ERROR("formatting error. (flag 0)");
    }

    return 0;
}
