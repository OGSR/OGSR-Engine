// Copyright (C) 2001-2003
// William E. Kempf
//
// Permission to use, copy, modify, distribute and sell this software
// and its documentation for any purpose is hereby granted without fee,
// provided that the above copyright notice appear in all copies and
// that both that copyright notice and this permission notice appear
// in supporting documentation.  William E. Kempf makes no representations
// about the suitability of this software for any purpose.
// It is provided "as is" without express or implied warranty.

#include <boost/thread/thread.hpp>
#include <iostream>

const int NUM_CALCS=5;

class factorial
{
public:
    factorial(int x, int& res) : x(x), res(res) { }
    void operator()() { res = calculate(x); }
    int result() const { return res; }

private:
    int calculate(int x) { return x <= 1 ? 1 : x * calculate(x-1); }

private:
    int x;
    int& res;
};

int main()
{
    int results[NUM_CALCS];
    boost::thread_group thrds;
    for (int i=0; i < NUM_CALCS; ++i)
        thrds.create_thread(factorial(i*10, results[i]));
    thrds.join_all();
    for (int j=0; j < NUM_CALCS; ++j)
        std::cout << j*10 << "! = " << results[j] << std::endl;
}
