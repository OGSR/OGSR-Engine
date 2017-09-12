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
#include <boost/thread/tss.hpp>
#include <cassert>

boost::thread_specific_ptr<int> value;

void increment()
{
    int* p = value.get();
    ++*p;
}

void thread_proc()
{
    value.reset(new int(0)); // initialize the thread's storage
    for (int i=0; i<10; ++i)
    {
        increment();
        int* p = value.get();
        assert(*p == i+1);
    }
}

int main(int argc, char* argv[])
{
    boost::thread_group threads;
    for (int i=0; i<5; ++i)
        threads.create_thread(&thread_proc);
    threads.join_all();
}
