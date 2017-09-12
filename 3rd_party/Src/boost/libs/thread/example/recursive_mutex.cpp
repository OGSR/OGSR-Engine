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

#include <boost/thread/recursive_mutex.hpp>
#include <boost/thread/thread.hpp>
#include <iostream>

class counter
{
public:
    counter() : count(0) { }

    int add(int val) {
        boost::recursive_mutex::scoped_lock scoped_lock(mutex);
        count += val;
        return count;
    }
    int increment() {
        boost::recursive_mutex::scoped_lock scoped_lock(mutex);
        return add(1);
    }

private:
    boost::recursive_mutex mutex;
    int count;
};

counter c;

void change_count()
{
    std::cout << "count == " << c.increment() << std::endl;
}

int main(int, char*[])
{
    const int num_threads=4;

    boost::thread_group threads;
    for (int i=0; i < num_threads; ++i)
        threads.create_thread(&change_count);

    threads.join_all();

    return 0;
}
