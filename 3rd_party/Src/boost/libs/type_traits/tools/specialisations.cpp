
//  (C) Copyright John Maddock 2000. Permission to copy, use, modify, sell and   
//  distribute this software is granted provided this copyright notice appears
//  in all copies. This software is provided "as is" without express or implied
//  warranty, and with no claim as to its suitability for any purpose.

//
// Simple program to output some template specialisations for the type_traits library.
//

#include <fstream>

using namespace std;

unsigned specializations = 30;

int main()
{
   unsigned i, j;
   ofstream os("specialisations");

   //
   // generate is_function tester prototypes:
   for(i = 0; i <= specializations; ++i)
   {
      os << "template <class R";
      for(j = 0; j < i; ++j)
      {
         os << ", class A" << j;
      }
      os << ">\n::boost::type_traits::yes_type is_function_tester(R (*)(";
      if(i == 0)
         os << "void";
      else
      {
         for(j = 0; j < i; ++j)
         {
            if(j) os << ", ";
            os << "A" << j;
         }
      }
      os << "));" << endl;
   }
   os << endl << endl;
   //
   // generate is_function_helper partial specialisations:
   //
   for(i = 0; i < specializations; ++i)
   {
      os << "template <class R";
      for(j = 0; j < i; ++j)
      {
         os << ", class A" << j;
      }
      os << ">\nstruct is_function_helper_base<R (*)(";
      if(i == 0)
         os << "void";
      else
      {
         for(j = 0; j < i; ++j)
         {
            if(j) os << ", ";
            os << "A" << j;
         }
      }
      os << ")>{ BOOST_STATIC_CONSTANT(bool, value = true); };" << endl;
   }
   os << endl << endl;


   //
   // generate is_member_pointer_helper tester prototypes:
   for(i = 0; i <= specializations; ++i)
   {
      os << "template <class R, class T";
      for(j = 0; j < i; ++j)
      {
         os << ", class A" << j;
      }
      os << ">\n::boost::type_traits::yes_type is_member_pointer_helper(R (T::*)(";
      if(i == 0)
         os << "void";
      else
      {
         for(j = 0; j < i; ++j)
         {
            if(j) os << ", ";
            os << "A" << j;
         }
      }
      os << "));" << endl;
   }
   os << endl << endl;
   //
   // generate is_member_pointer partial specialisations:
   //
   for(i = 0; i < specializations; ++i)
   {
      os << "template <class R, class T";
      for(j = 0; j < i; ++j)
      {
         os << ", class A" << j;
      }
      os << ">\nstruct is_member_pointer<R (T::*)(";
      if(i == 0)
         os << "void";
      else
      {
         for(j = 0; j < i; ++j)
         {
            if(j) os << ", ";
            os << "A" << j;
         }
      }
      os << ")>{ BOOST_STATIC_CONSTANT(bool, value = true); };" << endl;
   }
   os << endl << endl;


   return 0;
}
