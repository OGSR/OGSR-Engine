// (C) 2002, Fernando Luis Cacciola Carballal.
//
// This material is provided "as is", with absolutely no warranty expressed
// or implied. Any use is at your own risk.
//
// Permission to use or copy this software for any purpose is hereby granted
// without fee, provided the above notices are retained on all copies.
// Permission to modify the code and to distribute modified code is granted,
// provided the above notices are retained, and a notice that the code was
// modified is included with the above copyright notice.
//
// Test program for "boost/utility/value_init.hpp"
//
// Initial: 21 Agu 2002

#include <iostream>
#include <string>

#include "boost/utility/value_init.hpp"

#ifdef __BORLANDC__
#pragma hdrstop
#endif

#define BOOST_INCLUDE_MAIN
#include "boost/test/test_tools.hpp"

//
// Sample POD type
//
struct POD
{
  POD () : c(0), i(0), f(0) {}

  POD ( char c_, int i_, float f_ ) : c(c_), i(i_), f(f_) {}

  friend std::ostream& operator << ( std::ostream& os, POD const& pod )
    { return os << '(' << pod.c << ',' << pod.i << ',' << pod.f << ')' ; }

  friend bool operator == ( POD const& lhs, POD const& rhs )
    { return lhs.f == rhs.f && lhs.c == rhs.c && lhs.i == rhs.i ; }

  float f;
  char  c;
  int   i;
} ;

//
// Sample non POD type
//
struct NonPODBase
{
  virtual ~NonPODBase() {}
} ;
struct NonPOD : NonPODBase
{
  NonPOD () : id() {}
  NonPOD ( std::string const& id_) : id(id_) {}

  friend std::ostream& operator << ( std::ostream& os, NonPOD const& npod )
    { return os << '(' << npod.id << ')' ; }

  friend bool operator == ( NonPOD const& lhs, NonPOD const& rhs )
    { return lhs.id == rhs.id ; }

  std::string id ;
} ;

template<class T>
void test ( T const& y, T const& z )
{
  boost::value_initialized<T> x ;
  BOOST_TEST ( y == x ) ;
  BOOST_TEST ( y == get(x) ) ;
  static_cast<T&>(x) = z ;
  get(x) = z ;
  BOOST_TEST ( x == z ) ;

  boost::value_initialized<T> const x_c ;
  BOOST_TEST ( y == x_c ) ;
  BOOST_TEST ( y == get(x_c) ) ;
  static_cast<T&>(x_c) = z ;
  BOOST_TEST ( x_c == z ) ;
  #ifdef PRODUCE_ERROR_1
  get(x_c) = z ; // this should produce an ERROR
  #endif

  boost::value_initialized<T const> cx ;
  BOOST_TEST ( y == cx ) ;
  BOOST_TEST ( y == get(cx) ) ;
  #ifdef PRODUCE_ERROR_2
  get(cx) = z ; // this should produce an ERROR
  #endif

  boost::value_initialized<T const> const cx_c ;
  BOOST_TEST ( y == cx_c ) ;
  BOOST_TEST ( y == get(cx_c) ) ;
  #ifdef PRODUCE_ERROR_3
  get(cx_c) = z ; // this should produce an ERROR
  #endif
}

int test_main(int, char **)
{
  test( 0,1234 ) ;
  test( 0.0,12.34 ) ;
  test( POD(0,0,0.0), POD('a',1234,56.78) ) ;
  test( NonPOD( std::string() ), NonPOD( std::string("something") ) ) ;

  return 0;
}


unsigned int expected_failures = 0;





