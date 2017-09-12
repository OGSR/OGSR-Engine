// Copyright David Abrahams 2002. Permission to copy, use,
// modify, sell and distribute this software is granted provided this
// copyright notice appears in all copies. This software is provided
// "as is" without express or implied warranty, and with no claim as
// to its suitability for any purpose.
#ifndef TEST_CLASS_DWA2002326_HPP
# define TEST_CLASS_DWA2002326_HPP
# include <cassert>

template <int n = 0>
struct test_class
{
    explicit test_class(int x) : x(x), magic(7654321 + n) { ++counter; }
    test_class(test_class const& rhs) : x(rhs.x), magic(7654321 + n) { ++counter; }
    virtual ~test_class() { assert(magic == 7654321 + n); magic = 6666666; x = 9999; --counter; }

    void set(int x) { assert(magic == 7654321 + n); this->x = x; }
    int value() const { assert(magic == 7654321 + n); return x; }
    operator int() const { return x; }
    static int count() { return counter; }

    int x;
    long magic;
    static int counter;
    
 private:
    void operator=(test_class const&);
};

template <int n>
int test_class<n>::counter;

#endif // TEST_CLASS_DWA2002326_HPP
