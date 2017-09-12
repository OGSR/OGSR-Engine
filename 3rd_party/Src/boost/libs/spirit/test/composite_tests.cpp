/*=============================================================================
    Spirit v1.6.0
    Copyright (c) 2001-2003 Joel de Guzman
    http://spirit.sourceforge.net/

    Permission to copy, use, modify, sell and distribute this software is
    granted provided this copyright notice appears in all copies. This
    software is provided "as is" without express or implied warranty, and
    with no claim as to its suitability for any purpose.
=============================================================================*/
#include <iostream>
#include <cassert>

using namespace std;

#include "boost/spirit/core.hpp"
using namespace boost::spirit;

/////////////////////////////////////////////////////////////////////////////////
////
////  Composite tests
////
/////////////////////////////////////////////////////////////////////////////////
struct Base {};
struct X { typedef X embed_t; void foo() const { cout << "X\n"; } };
struct Y { typedef Y embed_t; void foo() const { cout << "Y\n"; } };
struct Z : public binary<X, Y, Base> { typedef Z embed_t; };

//struct A_ : public unary<Z, Base> { typedef A_ embed_t; };
//struct B_ { typedef B_ const& embed_t; void foo() const { cout << "B_\n"; } };
//struct C_ { typedef C_ const& embed_t; void foo() const { cout << "C_\n"; } };

void
composite_tests()
{
//    cout << "sizeof(X) == " << sizeof(X) << '\n';
//    cout << "sizeof(Y) == " << sizeof(Y) << '\n';
//
//    binary<X, Y, Base> b1;
//    b1.left().foo();
//    b1.right().foo();
//    cout << "sizeof(binary<X, Y, Base>) == "
//        << sizeof(binary<X, Y, Base>) << '\n';
//
//    A_ u1;
//    u1.subject().left().foo();
//    u1.subject().right().foo();
//    cout << "sizeof(unary<binary<X, Y, Base>, Base>) == "
//        << sizeof(A_) << '\n';
//
//    B_ b; C_ c;
//    binary<B_, C_, Base> b2(b, c);
//    b2.left().foo();
//    b2.right().foo();
//    cout << "sizeof(binary<B_&, C_&, Base>) == "
//        << sizeof(binary<B_, C_, Base>) << '\n';
//
//    cout << "sizeof(binary<binary<X, Y, Base>, binary<X, Y, Base>, Base>) == "
//        << sizeof(binary<Z, Z, Base>) << '\n';
}

///////////////////////////////////////////////////////////////////////////////
//
//  Main
//
///////////////////////////////////////////////////////////////////////////////
int
main()
{
    composite_tests();
    cout << "Tests concluded successfully\n";
    return 0;
}

