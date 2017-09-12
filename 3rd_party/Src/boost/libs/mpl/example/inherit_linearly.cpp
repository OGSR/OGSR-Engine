//-----------------------------------------------------------------------------
// boost mpl/example/inherit_linearly.cpp source file
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

#include "boost/mpl/inherit_linearly.hpp"
#include "boost/mpl/int.hpp"
#include "boost/mpl/list.hpp"

#include <iostream>

namespace mpl = boost::mpl;
using namespace mpl::placeholders;

template< typename Base, typename T >
struct tuple_part
    : Base
{
    typedef tuple_part type; // note the typedef
    typedef typename Base::index::next index;

    friend T& field(tuple_part& t, index) { return t.field_; }
    T field_;
};

struct empty_tuple
{
    typedef mpl::int_<-1> index;
};


typedef mpl::inherit_linearly<
      mpl::list<int,char const*,bool>
    , tuple_part<_,_>
    , empty_tuple
    >::type my_tuple;
    

int main()
{
    my_tuple t;
    
    field(t, mpl::int_<0>()) = -1;
    field(t, mpl::int_<1>()) = "text";
    field(t, mpl::int_<2>()) = false;

    std::cout
        << field(t, mpl::int_<0>()) << '\n'
        << field(t, mpl::int_<1>()) << '\n'
        << field(t, mpl::int_<2>()) << '\n'
        ;

    return 0;
}
