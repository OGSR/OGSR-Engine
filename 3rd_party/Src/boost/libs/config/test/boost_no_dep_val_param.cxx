//  (C) Copyright John Maddock 2001. Permission to copy, use, modify, sell and
//  distribute this software is granted provided this copyright notice appears
//  in all copies. This software is provided "as is" without express or implied
//  warranty, and with no claim as to its suitability for any purpose.

//  MACRO:         BOOST_NO_DEPENDENT_TYPES_IN_TEMPLATE_VALUE_PARAMETERS
//  TITLE:         dependent non-type template parameters
//  DESCRIPTION:   Template value parameters cannot have a dependent 
//                 type, for example:
//                 template<class T, typename T::type value> 
//                 class X { ... };


namespace boost_no_dependent_types_in_template_value_parameters{

template <class T, typename T::type value = 0>
class X
{};

template <class T>
struct typifier
{
   typedef T type;
};

int test()
{
   X<typifier<int> > x;
   (void) &x;        // avoid "unused variable" warning
   return 0;
}

}

