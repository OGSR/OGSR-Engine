
//  (C) Copyright John Maddock 2000. Permission to copy, use, modify, sell and   
//  distribute this software is granted provided this copyright notice appears
//  in all copies. This software is provided "as is" without express or implied
//  warranty, and with no claim as to its suitability for any purpose.

#ifndef TT_TEST_HPP
#define TT_TEST_HPP

#include <boost/test/unit_test.hpp>

//
// basic configuration:
//
#ifdef TEST_STD

#define tt std

#define TYPE_TRAITS(x) <type_traits>
#define TYPE_COMPARE(x) <type_compare>
#define TYPE_TRANSFORM(x) <type_transform>

#else

#define tt boost

#define TYPE_TRAITS(x) BOOST_STRINGIZE(boost/type_traits/x.hpp)
#define TYPE_COMPARE(x) BOOST_STRINGIZE(boost/type_traits/x.hpp)
#define TYPE_TRANSFORM(x) BOOST_STRINGIZE(boost/type_traits/x.hpp)

#endif

//
// global unit, this is not safe, but until the unit test framework uses
// shared_ptr throughout this is about as good as it gets :-(
//
boost::unit_test_framework::test_suite* get_master_unit(const char* name = 0);

//
// initialisation class:
//
class unit_initialiser
{
public:
   unit_initialiser(void (*f)(), const char* name)
   {
      get_master_unit("Type Traits")->add( ::boost::unit_test_framework::create_test_case(f, name) );
   }
};

#define TT_TEST_BEGIN(trait_name)\
   namespace{\
   void trait_name();\
   unit_initialiser init(trait_name, BOOST_STRINGIZE(trait_name));\
   void trait_name(){

#define TT_TEST_END }}

#define TRANSFORM_CHECK(name, from_suffix, to_suffix)\
   BOOST_CHECK_TYPE(bool to_suffix, name<bool from_suffix>::type);\
   BOOST_CHECK_TYPE(char to_suffix, name<char from_suffix>::type);\
   BOOST_CHECK_TYPE(wchar_t to_suffix, name<wchar_t from_suffix>::type);\
   BOOST_CHECK_TYPE(signed char to_suffix, name<signed char from_suffix>::type);\
   BOOST_CHECK_TYPE(unsigned char to_suffix, name<unsigned char from_suffix>::type);\
   BOOST_CHECK_TYPE(short to_suffix, name<short from_suffix>::type);\
   BOOST_CHECK_TYPE(unsigned short to_suffix, name<unsigned short from_suffix>::type);\
   BOOST_CHECK_TYPE(int to_suffix, name<int from_suffix>::type);\
   BOOST_CHECK_TYPE(unsigned int to_suffix, name<unsigned int from_suffix>::type);\
   BOOST_CHECK_TYPE(long to_suffix, name<long from_suffix>::type);\
   BOOST_CHECK_TYPE(unsigned long to_suffix, name<unsigned long from_suffix>::type);\
   BOOST_CHECK_TYPE(float to_suffix, name<float from_suffix>::type);\
   BOOST_CHECK_TYPE(long double to_suffix, name<long double from_suffix>::type);\
   BOOST_CHECK_TYPE(double to_suffix, name<double from_suffix>::type);\
   BOOST_CHECK_TYPE(UDT to_suffix, name<UDT from_suffix>::type);\
   BOOST_CHECK_TYPE(enum1 to_suffix, name<enum1 from_suffix>::type);

#define BOOST_DUMMY_MACRO_PARAM /**/

#define BOOST_DECL_TRANSFORM_TEST(name, type, from, to)\
void name(){ TRANSFORM_CHECK(type, from, to) }
#define BOOST_DECL_TRANSFORM_TEST3(name, type, from)\
void name(){ TRANSFORM_CHECK(type, from, BOOST_DUMMY_MACRO_PARAM) }
#define BOOST_DECL_TRANSFORM_TEST2(name, type, to)\
void name(){ TRANSFORM_CHECK(type, BOOST_DUMMY_MACRO_PARAM, to) }
#define BOOST_DECL_TRANSFORM_TEST0(name, type)\
void name(){ TRANSFORM_CHECK(type, BOOST_DUMMY_MACRO_PARAM, BOOST_DUMMY_MACRO_PARAM) }



//
// VC++ emits an awful lot of warnings unless we define these:
#ifdef BOOST_MSVC
#  pragma warning(disable:4800)
#endif

//
// define some test types:
//
enum enum_UDT{ one, two, three };
struct UDT
{
   UDT(){};
   ~UDT(){};
   UDT(const UDT&);
   UDT& operator=(const UDT&);
   int i;

   void f1();
   int f2();
   int f3(int);
   int f4(int, float);
};

typedef void(*f1)();
typedef int(*f2)(int);
typedef int(*f3)(int, bool);
typedef void (UDT::*mf1)();
typedef int (UDT::*mf2)();
typedef int (UDT::*mf3)(int);
typedef int (UDT::*mf4)(int, float);
typedef int (UDT::*mp);
typedef int (UDT::*cmf)(int) const;

// cv-qualifiers applied to reference types should have no effect
// declare these here for later use with is_reference and remove_reference:
# ifdef BOOST_MSVC
#  pragma warning(push)
#  pragma warning(disable: 4181)
# elif defined(__ICL)
#  pragma warning(push)
#  pragma warning(disable: 21)
# endif
//
// This is intentional:
// r_type and cr_type should be the same type
// but some compilers wrongly apply cv-qualifiers
// to reference types (this may generate a warning
// on some compilers):
//
typedef int& r_type;
typedef const r_type cr_type;
# ifdef BOOST_MSVC
#  pragma warning(pop)
# elif defined(__ICL)
#  pragma warning(pop)
#  pragma warning(disable: 985) // identifier truncated in debug information
# endif

struct POD_UDT { int x; };
struct empty_UDT
{
   ~empty_UDT(){};
   empty_UDT& operator=(const empty_UDT&){ return *this; }
   bool operator==(const empty_UDT&)const
   { return true; }
};
struct empty_POD_UDT
{
   empty_POD_UDT& operator=(const empty_POD_UDT&){ return *this; }
   bool operator==(const empty_POD_UDT&)const
   { return true; }
};
union union_UDT
{
  int x;
  double y;
  ~union_UDT(){}
};
union POD_union_UDT
{
  int x;
  double y;
};
union empty_union_UDT
{
   ~empty_union_UDT(){}
};
union empty_POD_union_UDT{};

class Base { };

class Derived : public Base { };
class Derived2 : public Base { };
class MultiBase : public Derived, public Derived2 {};
class PrivateBase : private Base {};

class NonDerived { };

enum enum1
{
   one_,two_
};

enum enum2
{
   three_,four_
};

struct VB
{
   virtual ~VB(){};
};

struct VD : VB
{
   ~VD(){};
};
//
// struct non_pointer:
// used to verify that is_pointer does not return
// true for class types that implement operator void*()
//
struct non_pointer
{
   operator void*(){return this;}
};
struct non_int_pointer
{
   int i;
   operator int*(){return &i;}
};
struct int_constructible
{
   int_constructible(int);
};
struct int_convertible
{
   operator int();
};
//
// struct non_empty:
// used to verify that is_empty does not emit
// spurious warnings or errors.
//
struct non_empty : private boost::noncopyable
{
   int i;
};
//
// abstract base classes:
struct test_abc1
{
   virtual void foo() = 0;
   virtual void foo2() = 0;
};

struct test_abc2
{
   virtual void foo() = 0;
   virtual void foo2() = 0;
};

struct test_abc3 : public test_abc1
{
   virtual void foo3() = 0;
};

struct incomplete_type;

typedef void foo0_t();
typedef void foo1_t(int);
typedef void foo2_t(int&, double);
typedef void foo3_t(int&, bool, int, int);
typedef void foo4_t(int, bool, int*, int[], int, int, int, int, int);


#endif
