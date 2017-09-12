//  boost utility cast test program  -----------------------------------------//

//  (C) Copyright boost.org 1999. Permission to copy, use, modify, sell
//  and distribute this software is granted provided this copyright
//  notice appears in all copies. This software is provided "as is" without
//  express or implied warranty, and with no claim as to its suitability for
//  any purpose.

//  See http://www.boost.org for most recent version including documentation.

//  Revision History
//   20 Jan 01  removed use of <limits> for portability to raw GCC (David Abrahams)
//   28 Jun 00  implicit_cast removed (Beman Dawes)
//   30 Aug 99  value_cast replaced by numeric_cast
//    3 Aug 99  Initial Version

#include <iostream>
#include <climits>
#include <cfloat>   // for DBL_MAX (Peter Schmid)
#include <boost/cast.hpp>

#  if SCHAR_MAX == LONG_MAX
#      error "This test program doesn't work if SCHAR_MAX == LONG_MAX"
#  endif 

using namespace boost;
using std::cout;

namespace
{
    struct Base
    { 
        virtual char kind() { return 'B'; }
    };
    
    struct Base2
    { 
        virtual char kind2() { return '2'; }
    };
    
    struct Derived : public Base, Base2
    {
        virtual char kind() { return 'D'; }
    }; 
}   


int main( int argc, char * argv[] )
{
    cout << "Usage: test_casts [n], where n omitted or is:\n"
            "  1 = execute #1 assert failure (#ifndef NDEBUG)\n"
            "  2 = execute #2 assert failure (#ifndef NDEBUG)\n"
            "Example: test_casts 2\n\n";

#   ifdef NDEBUG
        cout << "NDEBUG is defined\n";
#   else
        cout << "NDEBUG is not defined\n";
#   endif

    cout << "\nBeginning tests...\n";        

//  test polymorphic_cast  ---------------------------------------------------//
    
    //  tests which should succeed
    Base *    base = new Derived;
    Base2 *   base2 = 0;
    Derived * derived = 0;
    derived = polymorphic_downcast<Derived*>( base );  // downcast
    assert( derived->kind() == 'D' );

    derived = 0;
    derived = polymorphic_cast<Derived*>( base ); // downcast, throw on error
    assert( derived->kind() == 'D' );

    base2 = polymorphic_cast<Base2*>( base ); // crosscast
    assert( base2->kind2() == '2' );

     //  tests which should result in errors being detected
    int err_count = 0;
    base = new Base;

    if ( argc > 1 && *argv[1] == '1' )
        { derived = polymorphic_downcast<Derived*>( base ); } // #1 assert failure

    bool caught_exception = false;
    try { derived = polymorphic_cast<Derived*>( base ); }
    catch (std::bad_cast)
        { cout<<"caught bad_cast\n"; caught_exception = true; }
    if ( !caught_exception ) ++err_count;
    //  the following is just so generated code can be inspected
    if ( derived->kind() == 'B' ) ++err_count;

//  test implicit_cast and numeric_cast  -------------------------------------//
    
    //  tests which should succeed
    long small_value = 1;
    long small_negative_value = -1;
    long large_value = LONG_MAX;
    long large_negative_value = LONG_MIN;
    signed char c = 0;

    c = large_value;  // see if compiler generates warning

    c = numeric_cast<signed char>( small_value );
    assert( c == 1 );
    c = 0;
    c = numeric_cast<signed char>( small_value );
    assert( c == 1 );
    c = 0;
    c = numeric_cast<signed char>( small_negative_value );
    assert( c == -1 );

    // These tests courtesy of Joe R NWP Swatosh<joe.r.swatosh@usace.army.mil>
    assert( 0.0f == numeric_cast<float>( 0.0 ) );
    assert( 0.0 == numeric_cast<double>( 0.0 ) );
    
    //  tests which should result in errors being detected

    caught_exception = false;
    try { c = numeric_cast<signed char>( large_value ); }
    catch (bad_numeric_cast)
        { cout<<"caught bad_numeric_cast #1\n"; caught_exception = true; }
    if ( !caught_exception ) ++err_count;

    caught_exception = false;
    try { c = numeric_cast<signed char>( large_negative_value ); }
    catch (bad_numeric_cast)
        { cout<<"caught bad_numeric_cast #2\n"; caught_exception = true; }
    if ( !caught_exception ) ++err_count;

    unsigned long ul;
    caught_exception = false;
    try { ul = numeric_cast<unsigned long>( large_negative_value ); }
    catch (bad_numeric_cast)
        { cout<<"caught bad_numeric_cast #3\n"; caught_exception = true; }
    if ( !caught_exception ) ++err_count;

    caught_exception = false;
    try { ul = numeric_cast<unsigned long>( small_negative_value ); }
    catch (bad_numeric_cast)
        { cout<<"caught bad_numeric_cast #4\n"; caught_exception = true; }
    if ( !caught_exception ) ++err_count;

    caught_exception = false;
    try { numeric_cast<int>( DBL_MAX ); }
    catch (bad_numeric_cast)
        { cout<<"caught bad_numeric_cast #5\n"; caught_exception = true; }
    if ( !caught_exception ) ++err_count;

    cout << err_count << " errors detected\nTest "
         << (err_count==0 ? "passed\n" : "failed\n");
    return err_count;
}   // main
