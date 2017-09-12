//  (C) Copyright John Maddock 2001. Permission to copy, use, modify, sell and
//  distribute this software is granted provided this copyright notice appears
//  in all copies. This software is provided "as is" without express or implied
//  warranty, and with no claim as to its suitability for any purpose.

//  MACRO:         BOOST_NO_TEMPLATE_PARTIAL_SPECIALIZATION
//  TITLE:         partial specialisation
//  DESCRIPTION:   Class template partial specialization
//                 (14.5.4 [temp.class.spec]) not supported.


namespace boost_no_template_partial_specialization{

template <class T>
struct partial1
{
   typedef T& type;
};

template <class T>
struct partial1<T&>
{
   typedef T& type;
};

template <class T, bool b>
struct partial2
{
   typedef T& type;
};

template <class T>
struct partial2<T,true>
{
   typedef T type;
};


int test()
{
   int i = 0;
   partial1<int&>::type p1 = i;
   partial2<int&,true>::type p2 = i;
   (void)p1;
   (void)p2;
   (void)i;
   return 0;
}

}



