//  (C) Copyright John Maddock 2001. Permission to copy, use, modify, sell and
//  distribute this software is granted provided this copyright notice appears
//  in all copies. This software is provided "as is" without express or implied
//  warranty, and with no claim as to its suitability for any purpose.

//  MACRO:         BOOST_NO_CV_SPECIALIZATIONS
//  TITLE:         template specialisations of cv-qualified types
//  DESCRIPTION:   If template specialisations for cv-qualified types
//                 conflict with a specialisation for a cv-unqualififed type.


namespace boost_no_cv_specializations{

template <class T>
struct is_int
{
};

template <>
struct is_int<int>
{};

template <>
struct is_int<const int>
{};

template <>
struct is_int<volatile int>
{};

template <>
struct is_int<const volatile int>
{};

int test()
{
   return 0;
}


}



