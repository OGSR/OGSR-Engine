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
#include <boost/thread/xtime.hpp>
#include <iostream>

struct thread_alarm
{
    thread_alarm(int secs) : m_secs(secs) { }
    void operator()()
    {
        boost::xtime xt;
        boost::xtime_get(&xt, boost::TIME_UTC);
        xt.sec += m_secs;

        boost::thread::sleep(xt);

        std::cout << "alarm sounded..." << std::endl;
    }

    int m_secs;
};

int main(int argc, char* argv[])
{
    int secs = 5;
    std::cout << "setting alarm for 5 seconds..." << std::endl;
    thread_alarm alarm(secs);
    boost::thread thrd(alarm);
    thrd.join();
}
