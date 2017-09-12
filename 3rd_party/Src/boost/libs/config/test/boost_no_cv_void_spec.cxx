//  (C) Copyright John Maddock 2001. Permission to copy, use, modify, sell and
//  distribute this software is granted provided this copyright notice appears
//  in all copies. This software is provided "as is" without express or implied
//  warranty, and with no claim as to its suitability for any purpose.

//  MACRO:         BOOST_NO_CV_VOID_SPECIALIZATIONS
//  TITLE:         template specialisations of cv-qualified void
//  DESCRIPTION:   If template specialisations for cv-void types
//                 conflict with a specialisation for void.


namespace boost_no_cv_void_specializations{

template <class T>
struct is_void
{
};

template <>
struct is_void<void>
{};

template <>
struct is_void<const void>
{};

template <>
struct is_void<volatile void>
{};

template <>
struct is_void<const volatile void>
{};

int test()
{
   return 0;
}

}


