#include <boost/python/detail/destroy.hpp>
#include <cassert>

struct bar;

namespace boost
{
  // lie to the library about bar so we can show that its destructor is optimized away.
  template <>
  struct has_trivial_destructor<bar>
  {
      BOOST_STATIC_CONSTANT(bool, value=true);
  };
}


int count;
int marks[] = {
    -1
    , -1, -1
    , -1, -1, -1, -1
    , -1
};
int* kills = marks;

struct foo
{
    foo() : n(count++) {}
    ~foo()
    {
        *kills++ = n;
    }
    int n;
};

struct bar : foo {};

void assert_destructions(int n)
{
    for (int i = 0; i < n; ++i)
        assert(marks[i] == i);
    assert(marks[n] == -1);
}

int main()
{
    assert_destructions(0);
    typedef int a[2];
    
    foo* f1 = new foo;
    boost::python::detail::destroy_referent<foo const volatile&>(f1);
    assert_destructions(1);
    
    foo* f2 = new foo[2];
    typedef foo x[2];
    
    boost::python::detail::destroy_referent<x const&>(f2);
    assert_destructions(3);

    typedef foo y[2][2];
    x* f3 = new y;
    boost::python::detail::destroy_referent<y&>(f3);
    assert_destructions(7);

    bar* b1 = new bar;
    boost::python::detail::destroy_referent<bar&>(b1);
    assert_destructions(7);
    
    bar* b2 = new bar[2];
    typedef bar xb[2];
    
    boost::python::detail::destroy_referent<xb&>(b2);
    assert_destructions(7);

    typedef bar yb[2][2];
    xb* b3 = new yb;
    boost::python::detail::destroy_referent<yb&>(b3);
    assert_destructions(7);

    return 0;
}
