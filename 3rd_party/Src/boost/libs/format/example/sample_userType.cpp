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
// sample_userType.cc :  example usage of format with a user-defined type
// ------------------------------------------------------------------------------

#include <iostream>
#include <iomanip>
#include "boost/format.hpp"


class Rational {
public:
  Rational(int n, unsigned int d) : n_(n), d_(d) {}
  Rational(int n, int d);    // convert denominator to unsigned
  friend std::ostream& operator<<(std::ostream&, const Rational&);
private:
  int n_;               // numerator
  unsigned int d_;      // denominator
};

Rational::Rational(int n, int d) : n_(n) 
{
    if(d < 0) { n_ = -n_; d=-d; }
    d_ = static_cast<unsigned int>(d);
}

std::ostream& operator<<(std::ostream& os, const Rational& r) {
  os << r.n_ << "/";
  std::ios_base::fmtflags oflags = os.flags();
  os << std::noshowpos << r.d_;
  os.flags( oflags);
  return os;
}





int main(){
    using namespace std;
    using boost::format;
    using boost::io::group; 
    using boost::io::str;
    string s;

    Rational  r(16, 9);

    cout << r;
    //          prints : "16/9" 

    cout << showpos << r << ", " << 5;
    //          prints : "+16/9, +5"

    cout << format("[%+8d] \n") % r ;
    //          prints : "[   +16/9] \n"

    cout << format("[%0+8d] \n") % r;
    //          prints : "[+00016/9] \n"

    cout << format("[%0 8d] \n") % r;
    //          prints : "[000 16/9] \n"
    
    cerr << "\n\nEverything went OK, exiting. \n";
    return 0;
}
