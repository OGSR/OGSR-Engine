#if defined(_MSC_VER) && !defined(__ICL) && !defined(__COMO__)
#pragma warning(disable: 4786)  // identifier truncated in debug info
#pragma warning(disable: 4710)  // function not inlined
#pragma warning(disable: 4711)  // function selected for automatic inline expansion
#pragma warning(disable: 4514)  // unreferenced inline removed
#endif

//
//  shared_from_this_test.cpp
//
//  Copyright (c) 2002, 2003 Peter Dimov
//
//  Permission to copy, use, modify, sell and distribute this software
//  is granted provided this copyright notice appears in all copies.
//  This software is provided "as is" without express or implied
//  warranty, and with no claim as to its suitability for any purpose.
//


#include <boost/enable_shared_from_this.hpp>
#include <boost/shared_ptr.hpp>

#include <boost/detail/lightweight_test.hpp>

//

class X
{
public:

    virtual void f() = 0;

protected:

    ~X() {}
};

class Y
{
public:

    virtual boost::shared_ptr<X> getX() = 0;

protected:

    ~Y() {}
};

boost::shared_ptr<Y> createY();

void test()
{
    boost::shared_ptr<Y> py = createY();
    BOOST_TEST(py.get() != 0);
    BOOST_TEST(py.use_count() == 1);

    boost::shared_ptr<X> px = py->getX();
    BOOST_TEST(px.get() != 0);
    BOOST_TEST(py.use_count() == 2);

    px->f();

    boost::shared_ptr<Y> py2 = boost::dynamic_pointer_cast<Y>(px);
    BOOST_TEST(py.get() == py2.get());
    BOOST_TEST(!(py < py2 || py2 < py));
    BOOST_TEST(py.use_count() == 3);
}

void test2();

int main()
{
    test();
    test2();
    return boost::report_errors();
}

// virtual inheritance from Y to stress the implementation
// (prevents Y* -> impl* casts)

class impl: public X, public virtual Y, public boost::enable_shared_from_this<impl>
{
public:

    virtual void f()
    {
    }

    virtual boost::shared_ptr<X> getX()
    {
        boost::shared_ptr<impl> pi = shared_from_this();
        BOOST_TEST(pi.get() == this);
        return pi;
    }
};

// intermediate impl2 to stress the implementation

class impl2: public impl
{
};

boost::shared_ptr<Y> createY()
{
    boost::shared_ptr<Y> pi(new impl2);
    return pi;
}

void test2()
{
    boost::shared_ptr<Y> pi(static_cast<impl2*>(0));
}
