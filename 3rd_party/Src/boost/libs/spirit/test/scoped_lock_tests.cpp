/*=============================================================================
    Spirit v1.6.0
    Copyright (C) 2003 Martin Wille
    http://spirit.sourceforge.net/

    Permission to copy, use, modify, sell and distribute this software is
    granted provided this copyright notice appears in all copies. This
    software is provided "as is" without express or implied warranty, and
    with no claim as to its suitability for any purpose.
=============================================================================*/

// Nota bene: the actual locking is _not_ tested here!

#include <iostream>

void banner()
{
    std::cout << "/////////////////////////////////////////////////////////\n";
    std::cout << "\n";
    std::cout << "          scoped_lock test\n";
    std::cout << "\n";
    std::cout << "/////////////////////////////////////////////////////////\n";
    std::cout << "\n";
}

#if defined(DONT_HAVE_BOOST)
// if boost libraries are not available we have to skip the tests
int
main()
{
    banner();
    std::cout << "Test skipped (Boost libaries not available)\n";
    return 0;
}
#else

#include <boost/thread/mutex.hpp>
#include <boost/spirit/core.hpp>
#include <boost/spirit/utility/scoped_lock.hpp>
#include <cassert>

int
main()
{
    banner();

    using boost::spirit::rule;
    using boost::spirit::scoped_lock_d;
    using boost::spirit::parse_info;
    using boost::spirit::parse;
    using boost::mutex;

    mutex m;
    rule<> r = scoped_lock_d(m)['x'];
    parse_info<> pi = parse("x", r);
    assert(pi.hit);
    assert(pi.full);

    std::cout  << "Test concluded successfully\n";

    return 0;
}

#endif // !defined(HAVE_BOOST)
