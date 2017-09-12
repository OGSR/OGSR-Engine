//  (C) Copyright Terje Slettebø 2002. Permission to copy, use, modify, sell and
//  distribute this software is granted provided this copyright notice appears
//  in all copies. This software is provided "as is" without express or implied
//  warranty, and with no claim as to its suitability for any purpose.

//  MACRO:         BOOST_BCB_PARTIAL_SPECIALIZATION_BUG
//  TITLE:         Full partial specialization support.
//  DESCRIPTION:   Borland C++ Builder has some rather specific partial
//                 specialisation bugs which this code tests for.


#include <vector>

namespace boost_bcb_partial_specialization_bug{


template<class T1,class T2>
class Test
{
};

template<class T1,class T2>
class Test<std::vector<T1>,T2>
{
};

template<class T>
class Test<std::vector<int>,T>
{
};

template <class T>
struct is_const{};
template <class T>
struct is_const<T const>{};

int test()
{
   Test<std::vector<int>,double> v;
   is_const<const int> ci;
   (void)v; // warning suppression
   (void)ci;
   return 0;
}


}





