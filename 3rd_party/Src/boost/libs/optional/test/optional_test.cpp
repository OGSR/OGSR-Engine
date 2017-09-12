// (C) 2003, Fernando Luis Cacciola Carballal.
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
// You are welcome to contact the author at:
//  fernando_cacciola@hotmail.com
//
#include<iostream>
#include<stdexcept>
#include<string>

#define BOOST_ENABLE_ASSERT_HANDLER

#include "boost/optional.hpp"

#ifdef __BORLANDC__
#pragma hdrstop
#endif

#include "boost/test/minimal.hpp"

#ifdef ENABLE_TRACE
#define TRACE(msg) std::cout << msg << std::endl ;
#else
#define TRACE(msg)
#endif

namespace boost {

void assertion_failed (char const * expr, char const * func, char const * file, long )
{
  using std::string ;
  string msg =  string("Boost assertion failure for \"")
               + string(expr)
               + string("\" at file \"")
               + string(file)
               + string("\" function \"")
               + string(func)
               + string("\"") ;

  TRACE(msg);

  throw std::logic_error(msg);
}

}

using boost::optional ;

template<class T> inline void unused_variable ( T const& ) {}

#ifdef BOOST_NO_ARGUMENT_DEPENDENT_LOOKUP
using boost::swap ;
using boost::get_pointer ;
#endif

// MSVC6.0 does not support comparisons of optional against a literal null pointer value (0)
// via the safe_bool operator.
#if BOOST_WORKAROUND(BOOST_MSVC, BOOST_TESTED_AT(1300) ) // 1300 == VC++ 7.1
#define BOOST_OPTIONAL_NO_NULL_COMPARE
#endif

#define ARG(T) (static_cast< T const* >(0))

//
// Helper class used to verify the lifetime managment of the values held by optional
//
class X
{
  public :

    X ( int av ) : v(av)
    {
      ++ count ;

      TRACE ( "X::X(" << av << "). this=" << this ) ;
    }

    X ( X const& rhs ) : v(rhs.v)
    {
       pending_copy = false ;

       TRACE ( "X::X( X const& rhs). this=" << this << " rhs.v=" << rhs.v ) ;

       if ( throw_on_copy )
       {
         TRACE ( "throwing exception in X's copy ctor" ) ;
         throw 0 ;
       }

       ++ count ;
    }

    ~X()
    {
      pending_dtor = false ;

      -- count ;

      TRACE ( "X::~X(). v=" << v << "  this=" << this );
    }

    X& operator= ( X const& rhs )
      {
        v = rhs.v ;

        TRACE ( "X::operator =( X const& rhs). this=" << this << " rhs.v=" << rhs.v ) ;

        return *this ;
      }

    friend bool operator == ( X const& a, X const& b )
      { return a.v == b.v ; }

    friend bool operator != ( X const& a, X const& b )
      { return a.v != b.v ; }

    friend bool operator < ( X const& a, X const& b )
      { return a.v < b.v ; }

    int  V() const { return v ; }
    int& V()       { return v ; }

    static int  count ;
    static bool pending_copy   ;
    static bool pending_dtor   ;
    static bool throw_on_copy ;

  private :

    int  v ;

  private :

    X() ;
} ;


int  X::count = 0 ;
bool X::pending_copy = false ;
bool X::pending_dtor = false ;
bool X::throw_on_copy = false ;

inline void set_pending_copy         ( X const* x ) { X::pending_copy  = true  ; }
inline void set_pending_dtor         ( X const* x ) { X::pending_dtor  = true  ; }
inline void set_throw_on_copy        ( X const* x ) { X::throw_on_copy = true  ; }
inline void reset_throw_on_copy      ( X const* x ) { X::throw_on_copy = false ; }
inline void check_is_pending_copy    ( X const* x ) { BOOST_CHECK( X::pending_copy ) ; }
inline void check_is_pending_dtor    ( X const* x ) { BOOST_CHECK( X::pending_dtor ) ; }
inline void check_is_not_pending_copy( X const* x ) { BOOST_CHECK( !X::pending_copy ) ; }
inline void check_is_not_pending_dtor( X const* x ) { BOOST_CHECK( !X::pending_dtor ) ; }
inline void check_instance_count     ( int c, X const* x ) { BOOST_CHECK( X::count == c ) ; }
inline int  get_instance_count       ( X const* x ) { return X::count ; }

inline void set_pending_copy         (...) {}
inline void set_pending_dtor         (...) {}
inline void set_throw_on_copy        (...) {}
inline void reset_throw_on_copy      (...) {}
inline void check_is_pending_copy    (...) {}
inline void check_is_pending_dtor    (...) {}
inline void check_is_not_pending_copy(...) {}
inline void check_is_not_pending_dtor(...) {}
inline void check_instance_count     (...) {}
inline int  get_instance_count       (...) { return 0 ; }


template<class T>
inline void check_uninitialized_const ( optional<T> const& opt )
{
#ifndef BOOST_OPTIONAL_NO_NULL_COMPARE
  BOOST_CHECK( opt == 0 ) ;
#endif  
  BOOST_CHECK( !opt ) ;
  BOOST_CHECK( !get_pointer(opt) ) ;
  BOOST_CHECK( !opt.get() ) ;
}
template<class T>
inline void check_uninitialized ( optional<T>& opt )
{
#ifndef BOOST_OPTIONAL_NO_NULL_COMPARE
  BOOST_CHECK( opt == 0 ) ;
#endif
  BOOST_CHECK( !opt ) ;
  BOOST_CHECK( !get_pointer(opt) ) ;
  BOOST_CHECK( !opt.get() ) ;

  check_uninitialized_const(opt);
}

template<class T>
inline void check_initialized_const ( optional<T> const& opt )
{
  BOOST_CHECK( opt ) ;

#ifndef BOOST_OPTIONAL_NO_NULL_COMPARE
  BOOST_CHECK( opt != 0 ) ;
#endif

  BOOST_CHECK ( !!opt ) ;
  BOOST_CHECK ( get_pointer(opt) ) ;
  BOOST_CHECK ( opt.get() ) ;
}

template<class T>
inline void check_initialized ( optional<T>& opt )
{
  BOOST_CHECK( opt ) ;

#ifndef BOOST_OPTIONAL_NO_NULL_COMPARE
  BOOST_CHECK( opt != 0 ) ;
#endif

  BOOST_CHECK ( !!opt ) ;
  BOOST_CHECK ( get_pointer(opt) ) ;
  BOOST_CHECK ( opt.get() ) ;

  check_initialized_const(opt);
}

template<class T>
inline void check_value_const ( optional<T> const& opt, T const& v, T const& z )
{
  BOOST_CHECK( *opt == v ) ;
  BOOST_CHECK( *opt != z ) ;
  BOOST_CHECK( (*(opt.operator->()) == v) ) ;
  BOOST_CHECK( *get_pointer(opt) == v ) ;
  BOOST_CHECK( *opt.get() == v ) ;
}

template<class T>
inline void check_value ( optional<T>& opt, T const& v, T const& z )
{
#if BOOST_WORKAROUND(BOOST_MSVC, <= 1200) // 1200 == VC++ 6.0
  // For some reason, VC6.0 is creating a temporary while evaluating (*opt == v),
  // so we need to turn throw on copy off first.
  reset_throw_on_copy( ARG(T) ) ;
#endif

  BOOST_CHECK( *opt == v ) ;
  BOOST_CHECK( *opt != z ) ;
  BOOST_CHECK( (*(opt.operator->()) == v) ) ;
  BOOST_CHECK( *get_pointer(opt) == v ) ;
  BOOST_CHECK( *opt.get() == v ) ;

  check_value_const(opt,v,z);
}


//
// Basic test.
// Check ordinary functionality:
//   Initialization, assignment, comparison and value-accessing.
//
template<class T>
void test_basics( T const* )
{
  TRACE( std::endl << BOOST_CURRENT_FUNCTION  );

  T z(0);

  T a(1);

  // Default construction.
  // 'def' state is Uninitialized.
  // T::T() is not called (and it is not even defined)
  optional<T> def ;
  check_uninitialized(def);

  // Direct initialization.
  // 'oa' state is Initialized with 'a'
  // T::T( T const& x ) is used.
  set_pending_copy( ARG(T) ) ;
  optional<T> oa ( a ) ;
  check_is_not_pending_copy( ARG(T) );
  check_initialized(oa);
  check_value(oa,a,z);


  T b(2);

  optional<T> ob ;

  // Value-Assignment upon Uninitialized optional.
  // T::T ( T const& x ) is used.
  set_pending_copy( ARG(T) ) ;
  ob.reset(a) ;
  check_is_not_pending_copy( ARG(T) ) ;
  check_initialized(ob);
  check_value(ob,a,z);

  // Value-Assignment upon Initialized optional.
  // This uses T::operator= ( T const& x ) directly
  // on the reference returned by operator*()
  set_pending_dtor( ARG(T) ) ;
  set_pending_copy( ARG(T) ) ;
  *ob = b ;
  check_is_pending_dtor( ARG(T) ) ;
  check_is_pending_copy( ARG(T) ) ;
  check_initialized(ob);
  check_value(ob,b,z);

  // Assignment initialization.
  // T::T ( T const& x ) is used to copy new value.
  set_pending_copy( ARG(T) ) ;
  optional<T> const oa2 ( oa ) ;
  check_is_not_pending_copy( ARG(T) ) ;
  check_initialized_const(oa2);
  check_value_const(oa2,a,z);

  // Assignment
  // T::~T() is used to destroy previous value in ob.
  // T::T ( T const& x ) is used to copy new value.
  set_pending_dtor( ARG(T) ) ;
  set_pending_copy( ARG(T) ) ;
  oa = ob ;
  check_is_not_pending_dtor( ARG(T) ) ;
  check_is_not_pending_copy( ARG(T) ) ;
  check_initialized(oa);
  check_value(oa,b,z);

  // Uninitializing Assignment upon Initialized Optional
  // T::~T() is used to destroy previous value in oa.
  set_pending_dtor( ARG(T) ) ;
  set_pending_copy( ARG(T) ) ;
  oa = def ;
  check_is_not_pending_dtor( ARG(T) ) ;
  check_is_pending_copy    ( ARG(T) ) ;
  check_uninitialized(oa);

  // Uninitializing Assignment upon Uninitialized Optional
  // (Dtor is not called this time)
  set_pending_dtor( ARG(T) ) ;
  set_pending_copy( ARG(T) ) ;
  oa = def ;
  check_is_pending_dtor( ARG(T) ) ;
  check_is_pending_copy( ARG(T) ) ;
  check_uninitialized(oa);

  // Deinitialization of Initialized Optional
  // T::~T() is used to destroy previous value in ob.
  set_pending_dtor( ARG(T) ) ;
  ob.reset();
  check_is_not_pending_dtor( ARG(T) ) ;
  check_uninitialized(ob);

  // Deinitialization of Uninitialized Optional
  // (Dtor is not called this time)
  set_pending_dtor( ARG(T) ) ;
  ob.reset();
  check_is_pending_dtor( ARG(T) ) ;
  check_uninitialized(ob);
}

//
// Test Direct Value Manipulation
//
template<class T>
void test_direct_value_manip( T const* )
{
  TRACE( std::endl << BOOST_CURRENT_FUNCTION   );
  
  T x(3);

  optional<T> const c_opt0(x) ;
  optional<T>         opt0(x);

  BOOST_CHECK( c_opt0->V() == x.V() ) ;
  BOOST_CHECK(   opt0->V() == x.V() ) ;

  BOOST_CHECK( (*c_opt0).V() == x.V() ) ;
  BOOST_CHECK( (*  opt0).V() == x.V() ) ;

  T y(4);
  *opt0 = y ;
  BOOST_CHECK( (*opt0).V() == y.V() ) ;

  BOOST_CHECK( x < (*opt0) ) ;
}

//
// Test Uninitialized access assert
//
template<class T>
void test_uninitialized_access( T const* )
{
  TRACE( std::endl << BOOST_CURRENT_FUNCTION   );

  optional<T> def ;

  bool passed = false ;
  try
  {
    // This should throw because 'def' is uninitialized
    T const& n = *def ;
    unused_variable(n);
    passed = true ;
  }
  catch (...) {}
  BOOST_CHECK(!passed);

  passed = false ;
  try
  {
    T v(5) ;
    unused_variable(v);
    // This should throw because 'def' is uninitialized
    *def = v ;
    passed = true ;
  }
  catch (...) {}
  BOOST_CHECK(!passed);

  passed = false ;
  try
  {
    // This should throw because 'def' is uninitialized
    T v = *(def.operator->()) ;
    unused_variable(v);
    passed = true ;
  }
  catch (...) {}
  BOOST_CHECK(!passed);
}

#if BOOST_WORKAROUND( BOOST_INTEL_CXX_VERSION, <= 700) // Intel C++ 7.0
void prevent_buggy_optimization( bool v ) {}
#endif

//
// Test Direct Initialization of optional for a T with throwing copy-ctor.
//
template<class T>
void test_throwing_direct_init( T const* )
{
  TRACE( std::endl << BOOST_CURRENT_FUNCTION   );

  T a(6);

  int count = get_instance_count( ARG(T) ) ;

  set_throw_on_copy( ARG(T) ) ;

  bool passed = false ;
  try
  {
    // This should:
    //   Attempt to copy construct 'a' and throw.
    // 'opt' won't be constructed.
    set_pending_copy( ARG(T) ) ;

#if BOOST_WORKAROUND( BOOST_INTEL_CXX_VERSION, <= 700) // Intel C++ 7.0
    // Intel C++ 7.0 specific:
    //    For some reason, when "check_is_not_pending_copy",
    //    after the exception block is reached,
    //    X::pending_copy==true even though X's copy ctor set it to false.
    //    I guessed there is some sort of optimization bug,
    //    and it seems to be the since the following additional line just
    //    solves the problem (!?)
    prevent_buggy_optimization(X::pending_copy);
#endif

    optional<T> opt(a) ;
    passed = true ;
  }
  catch ( ... ){}

  BOOST_CHECK(!passed);
  check_is_not_pending_copy( ARG(T) );
  check_instance_count(count, ARG(T) );
}

//
// Test Value Assignment to an Uninitialized optional for a T with a throwing copy-ctor
//
template<class T>
void test_throwing_val_assign_on_uninitialized( T const* )
{
  TRACE( std::endl << BOOST_CURRENT_FUNCTION   );

  T a(7);

  int count = get_instance_count( ARG(T) ) ;

  set_throw_on_copy( ARG(T) ) ;

  optional<T> opt ;

  bool passed = false ;
  try
  {
    // This should:
    //   Attempt to copy construct 'a' and throw.
    //   opt should be left uninitialized.
    set_pending_copy( ARG(T) ) ;
    opt.reset( a );
    passed = true ;
  }
  catch ( ... ) {}

  BOOST_CHECK(!passed);

  check_is_not_pending_copy( ARG(T) );
  check_instance_count(count, ARG(T) );
  check_uninitialized(opt);
}

//
// Test Value Reset on an Initialized optional for a T with a throwing copy-ctor
//
template<class T>
void test_throwing_val_assign_on_initialized( T const* )
{
  TRACE( std::endl << BOOST_CURRENT_FUNCTION   );

  T z(0);
  T a(8);
  T b(9);

  int count = get_instance_count( ARG(T) ) ;

  reset_throw_on_copy( ARG(T) ) ;

  optional<T> opt ( b ) ;
  ++ count ;

  check_instance_count(count, ARG(T) );

  check_value(opt,b,z);

  set_throw_on_copy( ARG(T) ) ;

  bool passed = false ;
  try
  {
    // This should:
    //   Attempt to copy construct 'a' and throw.
    //   opt should be left uninitialized (even though it was initialized)
    set_pending_dtor( ARG(T) ) ;
    set_pending_copy( ARG(T) ) ;
    opt.reset ( a ) ;
    passed = true ;
  }
  catch ( ... ) {}

  BOOST_CHECK(!passed);

  -- count ;
  
  check_is_not_pending_dtor( ARG(T) );
  check_is_not_pending_copy( ARG(T) );
  check_instance_count(count, ARG(T) );
  check_uninitialized(opt);
}

//
// Test Copy Initialization from an Initialized optional for a T with a throwing copy-ctor
//
template<class T>
void test_throwing_copy_initialization( T const* )
{
  TRACE( std::endl << BOOST_CURRENT_FUNCTION   );

  T z(0);
  T a(10);

  reset_throw_on_copy( ARG(T) ) ;

  optional<T> opt (a);

  int count = get_instance_count( ARG(T) ) ;

  set_throw_on_copy( ARG(T) ) ;

  bool passed = false ;
  try
  {
    // This should:
    //   Attempt to copy construct 'opt' and throw.
    //   opt1 won't be constructed.
    set_pending_copy( ARG(T) ) ;
    optional<T> opt1 = opt ;
    passed = true ;
  }
  catch ( ... ) {}

  BOOST_CHECK(!passed);

  check_is_not_pending_copy( ARG(T) );
  check_instance_count(count, ARG(T) );

  // Nothing should have happened to the source optional.
  check_initialized(opt);
  check_value(opt,a,z);
}

//
// Test Assignment to an Uninitialized optional from an Initialized optional
// for a T with a throwing copy-ctor
//
template<class T>
void test_throwing_assign_to_uninitialized( T const* )
{
  TRACE( std::endl << BOOST_CURRENT_FUNCTION   );

  T z(0);
  T a(11);

  reset_throw_on_copy( ARG(T) ) ;

  optional<T> opt0 ;
  optional<T> opt1(a) ;

  int count = get_instance_count( ARG(T) ) ;

  set_throw_on_copy( ARG(T) ) ;

  bool passed = false ;
  try
  {
    // This should:
    //   Attempt to copy construct 'opt1.value()' into opt0 and throw.
    //   opt0 should be left uninitialized.
    set_pending_copy( ARG(T) ) ;
    opt0 = opt1 ;
    passed = true ;
  }
  catch ( ... ) {}

  BOOST_CHECK(!passed);

  check_is_not_pending_copy( ARG(T) );
  check_instance_count(count, ARG(T) );
  check_uninitialized(opt0);
}

//
// Test Assignment to an Initialized optional from an Initialized optional
// for a T with a throwing copy-ctor
//
template<class T>
void test_throwing_assign_to_initialized( T const* )
{
  TRACE( std::endl << BOOST_CURRENT_FUNCTION   );

  T z(0);
  T a(12);
  T b(13);

  reset_throw_on_copy( ARG(T) ) ;

  optional<T> opt0(a) ;
  optional<T> opt1(b) ;

  int count = get_instance_count( ARG(T) ) ;

  set_throw_on_copy( ARG(T) ) ;

  bool passed = false ;
  try
  {
    // This should:
    //   Attempt to copy construct 'opt1.value()' into opt0 and throw.
    //   opt0 should be left uninitialized (even though it was initialized)
    set_pending_dtor( ARG(T) ) ;
    set_pending_copy( ARG(T) ) ;
    opt0 = opt1 ;
    passed = true ;
  }
  catch ( ... ) {}

  BOOST_CHECK(!passed);

  -- count ;

  check_is_not_pending_dtor( ARG(T) );
  check_is_not_pending_copy( ARG(T) );
  check_instance_count(count, ARG(T) );
  check_uninitialized(opt0);
}

//
// Test swap in a no-throwing case
//
template<class T>
void test_no_throwing_swap( T const* )
{
  TRACE( std::endl << BOOST_CURRENT_FUNCTION   );
  
  T z(0);
  T a(14);
  T b(15);

  reset_throw_on_copy( ARG(T) ) ;

  optional<T> def0 ;
  optional<T> def1 ;
  optional<T> opt0(a) ;
  optional<T> opt1(b) ;

  int count = get_instance_count( ARG(T) ) ;

  using boost::swap ;

  swap(def0,def1);
  check_uninitialized(def0);
  check_uninitialized(def1);

  swap(def0,opt0);
  check_uninitialized(opt0);
  check_initialized(def0);
  check_value(def0,a,z);

  // restore def0 and opt0
  swap(def0,opt0);

  swap(opt0,opt1);
  check_instance_count(count, ARG(T) );
  check_initialized(opt0);
  check_initialized(opt1);
  check_value(opt0,b,z);
  check_value(opt1,a,z);
}

//
// Test swap in a throwing case
//
template<class T>
void test_throwing_swap( T const* )
{
  TRACE( std::endl << BOOST_CURRENT_FUNCTION   );
  
  T a(16);
  T b(17);

  reset_throw_on_copy( ARG(T) ) ;

  optional<T> opt0(a) ;
  optional<T> opt1(b) ;

  set_throw_on_copy( ARG(T) ) ;

  //
  // Case 1: Both Initialized.
  //
  bool passed = false ;
  try
  {
    // This should attempt to swap optionals and fail at swap(X&,X&).
    swap(opt0,opt1);

    passed = true ;
  }
  catch ( ... ) {}

  BOOST_CHECK(!passed);

  // Assuming swap(T&,T&) has at least the basic guarantee, these should hold.
  BOOST_CHECK( ( !opt0 || ( !!opt0 && ( ( *opt0 == a ) || ( *opt0 == b ) ) ) ) ) ;
  BOOST_CHECK( ( !opt1 || ( !!opt1 && ( ( *opt1 == a ) || ( *opt1 == b ) ) ) ) ) ;

  //
  // Case 2: Only one Initialized.
  //
  reset_throw_on_copy( ARG(T) ) ;

  opt0.reset();
  opt1.reset(a);

  set_throw_on_copy( ARG(T) ) ;

  passed = false ;
  try
  {
    // This should attempt to swap optionals and fail at opt0.reset(*opt1)
    // opt0 should be left uninitialized and opt1 unchanged.
    swap(opt0,opt1);

    passed = true ;
  }
  catch ( ... ) {}

  BOOST_CHECK(!passed);

  check_uninitialized(opt0);
  check_initialized(opt1);
  check_value(opt1,a,b);
}

//
// This verifies relational operators.
//
template<class T>
void test_relops( T const* )
{
  TRACE( std::endl << BOOST_CURRENT_FUNCTION   );
  
  reset_throw_on_copy( ARG(T) ) ;
  
  T v0(18);
  T v1(19);
  T v2(19);

  optional<T> def0 ;
  optional<T> def1 ;
  optional<T> opt0(v0);
  optional<T> opt1(v1);
  optional<T> opt2(v2);

  // Check identity
  BOOST_CHECK ( def0 == def0 ) ;
  BOOST_CHECK ( opt0 == opt0 ) ;
  BOOST_CHECK ( !(def0 != def0) ) ;
  BOOST_CHECK ( !(opt0 != opt0) ) ;

  // If both are uininitalized they compare equal
  BOOST_CHECK (   def0 == def1  ) ;
  BOOST_CHECK ( !(def0 != def1) ) ;

  // If only one is initialized they compare unequal
  BOOST_CHECK (   def0 != opt0  ) ;
  BOOST_CHECK ( !(def1 == opt1) ) ;

  // If both are initialized, values are compared
  BOOST_CHECK ( opt0 != opt1 ) ;
  BOOST_CHECK ( opt1 == opt2 ) ;
}

void test_with_builtin_types()
{
  TRACE( std::endl << BOOST_CURRENT_FUNCTION   );
  
  test_basics( ARG(double) );
  test_uninitialized_access( ARG(double) );
  test_no_throwing_swap( ARG(double) );
  test_relops( ARG(double) ) ;
}

void test_with_class_type()
{
  TRACE( std::endl << BOOST_CURRENT_FUNCTION   );

  test_basics( ARG(X) );
  test_direct_value_manip( ARG(X) );
  test_uninitialized_access( ARG(X) );
  test_throwing_direct_init( ARG(X) );
  test_throwing_val_assign_on_uninitialized( ARG(X) );
  test_throwing_val_assign_on_initialized( ARG(X) );
  test_throwing_copy_initialization( ARG(X) );
  test_throwing_assign_to_uninitialized( ARG(X) );
  test_throwing_assign_to_initialized( ARG(X) );
  test_no_throwing_swap( ARG(X) );
  test_throwing_swap( ARG(X) );
  test_relops( ARG(X) ) ;
  BOOST_CHECK ( X::count == 0 ) ;
}

int eat ( char ) { return 1 ; }
int eat ( int  ) { return 1 ; }
int eat ( void const* ) { return 1 ; }

template<class T> int eat ( T ) { return 0 ; }

//
// This verifies that operator safe_bool() behaves properly.
//
template<class T>
void test_no_implicit_conversions_impl( T const& )
{
  TRACE( std::endl << BOOST_CURRENT_FUNCTION   );

  optional<T> def ;
  BOOST_CHECK ( eat(def) == 0 ) ;
}

void test_no_implicit_conversions()
{
  TRACE( std::endl << BOOST_CURRENT_FUNCTION   );

  char c = 0 ;
  int i = 0 ;
  void const* p = 0 ;

  test_no_implicit_conversions_impl(c);
  test_no_implicit_conversions_impl(i);
  test_no_implicit_conversions_impl(p);
}

struct A {} ;
void test_conversions()
{
  TRACE( std::endl << BOOST_CURRENT_FUNCTION );

#ifndef BOOST_OPTIONAL_NO_CONVERTING_COPY_CTOR
  char c = 20 ;
  optional<char> opt0(c);
  optional<int> opt1(opt0);
  BOOST_CHECK(*opt1 == static_cast<int>(c));
#endif

#ifndef BOOST_OPTIONAL_NO_CONVERTING_ASSIGNMENT
  float f = 21.22f ;
  double d = f ;
  optional<float> opt2(f) ;
  optional<double> opt3 ;
  opt3 = opt2 ;
  BOOST_CHECK(*opt3 == d);
#endif  
}

int test_main( int, char* [] )
{
  try
  {
    test_with_class_type();
    test_with_builtin_types();
    test_no_implicit_conversions();
    test_conversions();
  }
  catch ( ... )
  {
    BOOST_ERROR("Unexpected Exception caught!");
  }

  return 0;
}


