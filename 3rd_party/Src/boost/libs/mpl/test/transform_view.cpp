//-----------------------------------------------------------------------------
// boost mpl/test/transform_view.cpp source file
// See http://www.boost.org for updates, documentation, and revision history.
//-----------------------------------------------------------------------------
//
// Copyright (c) 2001-02
// Aleksey Gurtovoy
//
// Permission to use, copy, modify, distribute and sell this software
// and its documentation for any purpose is hereby granted without fee, 
// provided that the above copyright notice appears in all copies and 
// that both the copyright notice and this permission notice appear in 
// supporting documentation. No representations are made about the 
// suitability of this software for any purpose. It is provided "as is" 
// without express or implied warranty.

#include "boost/mpl/transform_view.hpp"
#include "boost/mpl/max_element.hpp"
#include "boost/mpl/list.hpp"
#include "boost/mpl/sizeof.hpp"
#include "boost/static_assert.hpp"

namespace mpl = boost::mpl;
using mpl::_;

int main()
{
    typedef mpl::list<int,long,char,char[50],double> types;
    typedef mpl::max_element<
          mpl::transform_view< types, mpl::sizeof_<_> >
        >::type iter;

    BOOST_STATIC_ASSERT(iter::type::value == 50);
    return 0;
}
