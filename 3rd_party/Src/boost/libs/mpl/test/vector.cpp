//-----------------------------------------------------------------------------
// boost mpl/test/vector.cpp source file
// See http://www.boost.org for updates, documentation, and revision history.
//-----------------------------------------------------------------------------
//
// Copyright (c) 2000-02
// Aleksey Gurtovoy
//
// Permission to use, copy, modify, distribute and sell this software
// and its documentation for any purpose is hereby granted without fee, 
// provided that the above copyright notice appears in all copies and 
// that both the copyright notice and this permission notice appear in 
// supporting documentation. No representations are made about the 
// suitability of this software for any purpose. It is provided "as is" 
// without express or implied warranty.

#include "boost/mpl/vector.hpp"

namespace mpl = boost::mpl;

int main()
{
    typedef mpl::vector0<> vector0;
    typedef mpl::vector1<char> vector1;
    typedef mpl::vector2<char,long> vector2;
    typedef mpl::vector9<char,char,char,char,char,char,char,char,char> vector9;

    return 0;
}
