//  boost::dynamic_bitset timing test ---------------------------------------//

//  (C) Copyright Gennaro Prota 2002.
//      Permission to copy, use, modify, sell and distribute this software
//      is granted provided this copyright notice appears in all copies.
//      This software is provided "as is" without express or implied warranty,
//      and with no claim as to its suitability for any purpose.



//****************************************************************************//

//  WARNING:
//  ~~~~~~~
//   This is a preliminary version, for internal testing only.
//   For now, it stresses the count() function only and has been executed
//   on a very few platforms. The previous version, for instance, was never
//   executed on MSVC (boost::bitset<> didn't even compile with it) and at
//   the first try it crashed at startup, presumably because of a linker bug.
//   To cope with it, the definition of
//
//      template <typename T> void timing_test()
//
//   has been moved before the definition of main()
//
//   LAST MODIFIED: 2 Aug 2002
//****************************************************************************//



#include <iostream>
#include <typeinfo>
#include <iomanip>

#include "boost/timer.hpp"
#include "boost/dynamic_bitset.hpp"
#include "boost/cstdlib.hpp"



void prologue()
{
    std::cout << "Compiler: " << BOOST_COMPILER << '\n';
    std::cout << "STLPort used? ";
#  ifdef _STLPORT_VERSION
    std::cout << "Yes, v." << std::hex << _STLPORT_VERSION;
#  else
    std::cout << "No.";
#  endif
    std::cout << std::dec << "\n";
}





template <typename T>
void timing_test()
{
    const unsigned long num = 10000;
    
    std::size_t dummy = 0; // this is printed at the end of the test,
                           // to prevent the optimizer to eliminate
                           // the call to count() in the loop below :-)


    std::cout << "\n\nTimings for dynamic_bitset<" << typeid(T).name()
              << ">  [" << num << " iterations]\n";
    std::cout << "--------------------------------------------------\n";

    { // new implementation
        boost::timer time;
        
        for (unsigned long i=0; i<num; ++i) {
            boost::dynamic_bitset<T> bs(std::size_t(5000), i);
            dummy += bs.count();
        }
        const double elaps = time.elapsed();
        std::cout << "Elapsed: " << elaps << '\n';
    }

    std::cout << "(total count: " << dummy << ")\n";
}



int main()
{

    prologue();

    timing_test<unsigned short>();
    timing_test<unsigned int>();
    timing_test<unsigned long>();
# ifdef BOOST_HAS_LONG_LONG
    timing_test<unsigned long long>();
# endif
    
    return boost::exit_success;
}

