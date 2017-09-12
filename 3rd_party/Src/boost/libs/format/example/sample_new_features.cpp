// -*- C++ -*-
//  Boost general library 'format'  ---------------------------
//  See http://www.boost.org for updates, documentation, and revision history.

//  (C) Samuel Krempp 2001
//                  krempp@crans.ens-cachan.fr
//  Permission to copy, use, modify, sell and
//  distribute this software is granted provided this copyright notice appears
//  in all copies. This software is provided "as is" without express or implied
//  warranty, and with no claim as to its suitability for any purpose.

// ----------------------------------------------------------------------------
// sample_new_features.cpp : demonstrate features that were added to printf's syntax
// ----------------------------------------------------------------------------

#include <iostream>
#include <iomanip>

#include "boost/format.hpp"

int main(){
    using namespace std;
    using boost::format;
    using boost::io::group;
    using boost::io::str;
    stringstream oss;

    // ------------------------------------------------------------------------
    // Simple style of reordering :
    cout << format("%1% %2% %3% %2% %1% \n") % "o" % "oo" % "O";
    //          prints  "o oo O oo o \n"


    // ------------------------------------------------------------------------
    // Centered alignment : flag '='
    cout << format("_%|=6|_") % 1 << endl;
    //          prints "_   1  _"  :  3 spaces are  padded before, and 2 after.



    // ------------------------------------------------------------------------
    // Tabulations :   "%|Nt|"  => tabulation of N spaces.
    //                 "%|NTf|" => tabulation of N times the character <f>.
    //  are useful when printing lines with several fields whose width can vary a lot
    //    but we'd like to print some fields at the same place when possible :
    vector<string>  names(1, "Marc-François Michel"), 
      surname(1,"Durand"), 
      tel(1, "+33 (0) 123 456 789");

    names.push_back("Jean"); 
    surname.push_back("de Lattre de Tassigny");
    tel.push_back("+33 (0) 987 654 321");

    for(unsigned int i=0; i<names.size(); ++i)
      cout << format("%1%, %2%, %|40t|%3%\n") % names[i] % surname[i] % tel[i];

    /* prints :

       
    Marc-François Michel, Durand,       +33 (0) 123 456 789
    Jean, de Lattre de Tassigny,        +33 (0) 987 654 321
    
    
     the same using width on each field lead to unnecessary too long lines,
     while 'Tabulations' insure a lower bound on the *sum* of widths,
     and that's often what we really want.
    */



    cerr << "\n\nEverything went OK, exiting. \n";
    return 0;
}
