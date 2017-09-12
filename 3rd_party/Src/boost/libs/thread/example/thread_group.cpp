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

int count = 0;
boost::mutex mutex;

void increment_count()
{
    boost::mutex::scoped_lock lock(mutex);
    std::cout << "count = " << ++count << std::endl;
}

int main(int argc, char* argv[])
{
    boost::thread_group threads;
    for (int i = 0; i < 10; ++i)
        threads.create_thread(&increment_count);
    threads.join_all();
}
