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

#include <boost/thread/mutex.hpp>
#include <boost/thread/thread.hpp>
#include <iostream>

boost::mutex io_mutex; // The iostreams are not guaranteed to be thread-safe!

class counter
{
public:
    counter() : count(0) { }

    int increment() {
        boost::mutex::scoped_lock scoped_lock(mutex);
        return ++count;
    }

private:
    boost::mutex mutex;
    int count;
};

counter c;

void change_count()
{
    int i = c.increment();
    boost::mutex::scoped_lock scoped_lock(io_mutex);
    std::cout << "count == " << i << std::endl;
}

int main(int, char*[])
{
    const int num_threads = 4;
    boost::thread_group thrds;
    for (int i=0; i < num_threads; ++i)
        thrds.create_thread(&change_count);

    thrds.join_all();

    return 0;
}
