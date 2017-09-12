//  (C) Copyright John Maddock 2002. Permission to copy, use, modify, sell and
//  distribute this software is granted provided this copyright notice appears
//  in all copies. This software is provided "as is" without express or implied
//  warranty, and with no claim as to its suitability for any purpose.

//  MACRO:         BOOST_NO_TEMPLATE_TEMPLATES
//  TITLE:         template template paramters.
//  DESCRIPTION:   Verify that template template parameters both work
//                 and can be deduced through a function call.


namespace boost_no_template_templates{


template<class T>
class foo
{
public:
   foo(){};
   foo(const T&){};
   const foo& bar()const{ return *this; }
   foo& operator=(const foo&){ return *this; }
};

template<typename T, template<typename> class U>
U<T> sinhc_pi(const U<T> x)
{
   return x.bar();
}


int test()
{
   foo<double> f1;
   foo<int> f2;
   f1 = sinhc_pi(f1);
   f2 = sinhc_pi(f2);
   return 0;
}


}





