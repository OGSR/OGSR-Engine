/*=============================================================================
    Spirit v1.6.0
    Copyright (c) 2002-2003 Martin Wille
    http://spirit.sourceforge.net/

    Permission to copy, use, modify, sell and distribute this software is
    granted provided this copyright notice appears in all copies. This
    software is provided "as is" without express or implied warranty, and
    with no claim as to its suitability for any purpose.
=============================================================================*/
// vim:ts=4:sw=4:et

#undef BOOST_SPIRIT_THREADSAFE
#include <boost/spirit/core/non_terminal/impl/object_with_id.ipp>
#include <cassert>
#include <iostream>

using boost::spirit::impl::object_with_id;

struct tag1 {};
struct tag2 {};

typedef object_with_id<tag1> class1;
typedef object_with_id<tag2> class2;

int
main()
{
    std::cout << "/////////////////////////////////////////////////////////\n";
    std::cout << "\n";
    std::cout << "          object_with_id test (ST)\n";
    std::cout << "\n";
    std::cout << "/////////////////////////////////////////////////////////\n";
    std::cout << "\n";

    class1 *c1o1 = new class1;
    class1 *c1o2 = new class1;
    class1 *c1o3 = new class1;

    // test wether the objects have consecutive numbers
    assert(c1o1->get_object_id()==1);
    assert(c1o2->get_object_id()==2);
    assert(c1o3->get_object_id()==3);

    // test wether number recycling works
    delete c1o3;
    c1o3 = new class1;
    assert(c1o3->get_object_id()==3);

    delete c1o2;
    c1o2 = new class1;
    assert(c1o2->get_object_id()==2);

    delete c1o2;
    delete c1o3;
    c1o2 = new class1;
    c1o3 = new class1;
    assert(c1o3->get_object_id()==3);
    assert(c1o2->get_object_id()==2);

    // test wether objects of different classes are numbered independently
    class2 *c2o1 = new class2;
    class2 *c2o2 = new class2;
    class2 *c2o3 = new class2;
    assert(c2o1->get_object_id()==1);
    assert(c2o2->get_object_id()==2);
    assert(c2o3->get_object_id()==3);

    //
    delete c1o1;
    delete c2o2;
    c2o2 = new class2;
    c1o1 = new class1;
    assert(c1o1->get_object_id()==1);
    assert(c2o2->get_object_id()==2);

    // test wether the copy ctor doesn't copy the id
    delete c1o1;
    c1o1 = new class1(*c1o2);
    assert(c1o1->get_object_id()==1);

    // test wether the assignment operator doesn't assign the id
    *c1o1 = *c1o2;
    assert(c1o1->get_object_id()==1);

    std::cout << "Test concluded successfully\n";
    return 0;
}
