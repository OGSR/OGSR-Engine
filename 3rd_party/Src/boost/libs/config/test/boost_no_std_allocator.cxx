//  (C) Copyright John Maddock 2001. Permission to copy, use, modify, sell and
//  distribute this software is granted provided this copyright notice appears
//  in all copies. This software is provided "as is" without express or implied
//  warranty, and with no claim as to its suitability for any purpose.

//  MACRO:         BOOST_NO_STD_ALLOCATOR
//  TITLE:         std::allocator
//  DESCRIPTION:   The C++ standard library does not provide
//                 a standards conforming std::allocator.

#ifndef BOOST_NESTED_TEMPLATE
#define BOOST_NESTED_TEMPLATE template
#endif

#include <memory>

namespace boost_no_std_allocator{

template <class T>
int test_allocator(const T& i)
{
   typedef std::allocator<int> alloc1_t;
   typedef typename alloc1_t::size_type           size_type;
   typedef typename alloc1_t::difference_type     difference_type;
   typedef typename alloc1_t::pointer             pointer;
   typedef typename alloc1_t::const_pointer       const_pointer;
   typedef typename alloc1_t::reference           reference;
   typedef typename alloc1_t::const_reference     const_reference;
   typedef typename alloc1_t::value_type          value_type;

   typedef typename alloc1_t::BOOST_NESTED_TEMPLATE rebind<double> binder_t;
   typedef typename binder_t::other alloc2_t;

   alloc1_t a1;
   alloc1_t a2(a1);

   pointer p = a1.allocate(1);
   const_pointer cp = p;
   a1.construct(p,i);
   size_type s = a1.max_size();
   (void)s;
   reference r = *p;
   const_reference cr = *cp;
   if(p != a1.address(r)) return -1;
   if(cp != a1.address(cr)) return -1;
   a1.destroy(p);
   a1.deallocate(p,1);

   alloc2_t a3(a1);
   // this chokes early versions of the MSL library
   // and isn't currently required by anything in boost
   // so don't test for now...
   // a3 = a2;
   return 0;
}


int test()
{
   return test_allocator(0);
}

}




