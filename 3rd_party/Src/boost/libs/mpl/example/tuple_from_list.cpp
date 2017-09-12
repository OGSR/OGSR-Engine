//-----------------------------------------------------------------------------
// boost mpl/example/tuple_from_list.cpp source file
// See http://www.boost.org for updates, documentation, and revision history.
//-----------------------------------------------------------------------------
//
// Copyright (c) 2002
// Aleksey Gurtovoy
//
// Permission to use, copy, modify, distribute and sell this software
// and its documentation for any purpose is hereby granted without fee, 
// provided that the above copyright notice appears in all copies and 
// that both the copyright notice and this permission notice appear in 
// supporting documentation. No representations are made about the 
// suitability of this software for any purpose. It is provided "as is" 
// without express or implied warranty.

#include "boost/mpl/fold_backward.hpp"
#include "boost/mpl/list.hpp"
#include "boost/mpl/assert_is_same.hpp"
#include "boost/tuple/tuple.hpp"

#include <iostream>

using namespace boost::mpl;

template< typename Types > struct tuple_gen
    : fold_backward<
          Types
        , boost::tuples::null_type
        , boost::tuples::cons<_2,_1>
        >
{
};

int main()
{
    tuple_gen< list<int,char const*,bool> >::type t;
    
    boost::get<0>(t) = -1;
    boost::get<1>(t) = "text";
    boost::get<2>(t) = false;

    std::cout
        << boost::get<0>(t) << '\n'
        << boost::get<1>(t) << '\n'
        << boost::get<2>(t) << '\n'
        ;

    return 0;
}
