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

#include <iostream>
#include <vector>
#include <boost/utility.hpp>
#include <boost/thread/condition.hpp>
#include <boost/thread/thread.hpp>

class bounded_buffer : private boost::noncopyable
{
public:
    typedef boost::mutex::scoped_lock lock;

    bounded_buffer(int n) : begin(0), end(0), buffered(0), circular_buf(n) { }

    void send (int m) {
        lock lk(monitor);
        while (buffered == circular_buf.size())
            buffer_not_full.wait(lk);
        circular_buf[end] = m;
        end = (end+1) % circular_buf.size();
        ++buffered;
        buffer_not_empty.notify_one();
    }
    int receive() {
        lock lk(monitor);
        while (buffered == 0)
            buffer_not_empty.wait(lk);
        int i = circular_buf[begin];
        begin = (begin+1) % circular_buf.size();
        --buffered;
        buffer_not_full.notify_one();
        return i;
    }

private:
    int begin, end, buffered;
    std::vector<int> circular_buf;
    boost::condition buffer_not_full, buffer_not_empty;
    boost::mutex monitor;
};

bounded_buffer buf(2);

void sender() {
    int n = 0;
    while (n < 100) {
        buf.send(n);
        std::cout << "sent: " << n << std::endl;
        ++n;
    }
    buf.send(-1);
}

void receiver() {
    int n;
    do {
        n = buf.receive();
        std::cout << "received: " << n << std::endl;
    } while (n != -1); // -1 indicates end of buffer
}

int main(int, char*[])
{
    boost::thread thrd1(&sender);
    boost::thread thrd2(&receiver);
    thrd1.join();
    thrd2.join();
    return 0;
}
