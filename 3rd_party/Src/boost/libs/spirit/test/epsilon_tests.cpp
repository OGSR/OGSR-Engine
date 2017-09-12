/*=============================================================================
    Spirit v1.6.0
    Copyright (c) 2002-2003 Martin Wille
    http://spirit.sourceforge.net/

    Permission to copy, use, modify, sell and distribute this software is
    granted provided this copyright notice appears in all copies. This
    software is provided "as is" without express or implied warranty, and
    with no claim as to its suitability for any purpose.
=============================================================================*/
// vi:ts=4:sw=4:et
// Tests for boost::spirit::epsilon_p
// [30-Dec-2002]
////////////////////////////////////////////////////////////////////////////////
#define qDebug 0
#include <iostream>
#include <cstring>
#include <cassert>
#if qDebug
#define BOOST_SPIRIT_DEBUG
#endif
#include <boost/spirit/core/composite/epsilon.hpp>
#include <boost/ref.hpp>
////////////////////////////////////////////////////////////////////////////////

namespace local
{
    template <typename T>
    struct var_wrapper
        : public ::boost::reference_wrapper<T>
    {
        typedef ::boost::reference_wrapper<T> parent;

        explicit inline var_wrapper(T& t) : parent(t) {}

        inline T& operator()() const { return parent::get(); }
    };

    template <typename T>
    inline var_wrapper<T>
    var(T& t)
    {
        return var_wrapper<T>(t);
    }
}

boost::spirit::parse_info<char const *> pi;

template<typename ParserT>
void
parse(char const *s, ParserT const &p, bool match)
{
    using namespace std;
    pi = boost::spirit::parse(s, s+strlen(s), p);
    if (match)
    {
        assert(pi.hit);
        assert(pi.length == 0);
        assert(pi.stop == s);
    }
    else
    {
        assert(!pi.hit);
    }
}

int
main()
{
    using namespace std;

    char const empty[]="";
    char const not_empty[]="asdfgh";

    ////////////////////////////////////////////////
    // Test wether epsilon_p/eps_p work correctly as
    // a primitive parser

    parse(empty, boost::spirit::epsilon_p, true);
    assert(pi.full);
    parse(not_empty, boost::spirit::epsilon_p, true);
    assert(!pi.full);

    parse(empty, boost::spirit::eps_p, true);
    assert(pi.full);
    parse(not_empty, boost::spirit::eps_p, true);
    assert(!pi.full);

    ////////////////////////////////////////////////
    // Test wether epsilon_p/eps_p work correctly as
    // a parser generator for functors

    bool       flag = false;
    parse(empty, boost::spirit::epsilon_p(local::var(flag)), flag);
    assert(!pi.full);

    flag = true;
    parse(empty, boost::spirit::epsilon_p(local::var(flag)), flag);
    assert(pi.full);

    ////////////////////////////////////////////////
    // Test wether epsilon_p/eps_p work correctly as
    // a parser generator for creating parsers from
    // other parsers

    flag = false;
    parse(empty, boost::spirit::epsilon_p(boost::spirit::epsilon_p(local::var(flag))), flag);
    assert(!pi.full);

    flag = true;
    parse(empty, boost::spirit::epsilon_p(boost::spirit::epsilon_p(local::var(flag))), flag);
    assert(pi.full);

    return 0;
}
