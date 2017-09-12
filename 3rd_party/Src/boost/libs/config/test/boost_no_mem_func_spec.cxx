//  (C) Copyright John Maddock 2002. Permission to copy, use, modify, sell and
//  distribute this software is granted provided this copyright notice appears
//  in all copies. This software is provided "as is" without express or implied
//  warranty, and with no claim as to its suitability for any purpose.

//  MACRO:         BOOST_NO_MEMBER_FUNCTION_SPECIALIZATIONS
//  TITLE:         Specialisation of individual member functions.
//  DESCRIPTION:   Verify that specializations of individual members
//                 of template classes work OK.


namespace boost_no_member_function_specializations{


template<class T>
class foo
{
public:
   foo();
   foo(const T&);
   ~foo();
   int bar();
};

// declare specialisations:
template<> foo<int>::foo();
template<> foo<int>::foo(const int&);
template<> foo<int>::~foo();
template<> int foo<int>::bar();

// provide defaults:
template<class T> foo<T>::foo(){}
template<class T> foo<T>::foo(const T&){}
template<class T> foo<T>::~foo(){}
template<class T> int foo<T>::bar(){ return 0; }

// provide defs:
template<> foo<int>::foo(){}
template<> foo<int>::foo(const int&){}
template<> foo<int>::~foo(){}
template<> int foo<int>::bar(){ return 1; }


int test()
{
   foo<double> f1;
   foo<int> f2;
   f1.bar();
   f2.bar();
   return 0;
}


}





