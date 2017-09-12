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

#include <boost/thread/condition.hpp>
#include <boost/thread/thread.hpp>
#include <boost/thread/xtime.hpp>

#include <boost/test/unit_test.hpp>

#include "util.inl"

struct condition_test_data
{
    condition_test_data() : notified(0), awoken(0) { }

    boost::mutex mutex;
    boost::condition condition;
    int notified;
    int awoken;
};

void condition_test_thread(condition_test_data* data)
{
    boost::mutex::scoped_lock lock(data->mutex);
    BOOST_CHECK(lock ? true : false);
    while (!(data->notified > 0))
        data->condition.wait(lock);
    BOOST_CHECK(lock ? true : false);
    data->awoken++;
}

struct cond_predicate
{
    cond_predicate(int& var, int val) : _var(var), _val(val) { }

    bool operator()() { return _var == _val; }

    int& _var;
    int _val;
};

void condition_test_waits(condition_test_data* data)
{
    boost::mutex::scoped_lock lock(data->mutex);
    BOOST_CHECK(lock ? true : false);

    // Test wait.
    while (data->notified != 1)
        data->condition.wait(lock);
    BOOST_CHECK(lock ? true : false);
    BOOST_CHECK_EQUAL(data->notified, 1);
    data->awoken++;
    data->condition.notify_one();

    // Test predicate wait.
    data->condition.wait(lock, cond_predicate(data->notified, 2));
    BOOST_CHECK(lock ? true : false);
    BOOST_CHECK_EQUAL(data->notified, 2);
    data->awoken++;
    data->condition.notify_one();

    // Test timed_wait.
    boost::xtime xt = delay(10);
    while (data->notified != 3)
        data->condition.timed_wait(lock, xt);
    BOOST_CHECK(lock ? true : false);
    BOOST_CHECK_EQUAL(data->notified, 3);
    data->awoken++;
    data->condition.notify_one();

    // Test predicate timed_wait.
    xt = delay(10);
    cond_predicate pred(data->notified, 4);
    BOOST_CHECK(data->condition.timed_wait(lock, xt, pred));
    BOOST_CHECK(lock ? true : false);
    BOOST_CHECK(pred());
    BOOST_CHECK_EQUAL(data->notified, 4);
    data->awoken++;
    data->condition.notify_one();
}

void do_test_condition_notify_one()
{
    condition_test_data data;

    boost::thread thread(bind(&condition_test_thread, &data));

    {
        boost::mutex::scoped_lock lock(data.mutex);
        BOOST_CHECK(lock ? true : false);
        data.notified++;
        data.condition.notify_one();
    }

    thread.join();
    BOOST_CHECK_EQUAL(data.awoken, 1);
}

void test_condition_notify_one()
{
    timed_test(&do_test_condition_notify_one, 2, execution_monitor::use_mutex);
}

void do_test_condition_notify_all()
{
    const int NUMTHREADS = 5;
    boost::thread_group threads;
    condition_test_data data;

    for (int i = 0; i < NUMTHREADS; ++i)
        threads.create_thread(bind(&condition_test_thread, &data));

    {
        boost::mutex::scoped_lock lock(data.mutex);
        BOOST_CHECK(lock ? true : false);
        data.notified++;
        data.condition.notify_all();
    }

    threads.join_all();
    BOOST_CHECK_EQUAL(data.awoken, NUMTHREADS);
}

void test_condition_notify_all()
{
    // We should have already tested notify_one here, so
    // a timed test with the default execution_monitor::use_condition
    // should be OK, and gives the fastest performance
    timed_test(&do_test_condition_notify_all, 3);
}

void do_test_condition_waits()
{
    condition_test_data data;

    boost::thread thread(bind(&condition_test_waits, &data));

    {
        boost::mutex::scoped_lock lock(data.mutex);
        BOOST_CHECK(lock ? true : false);

        boost::thread::sleep(delay(1));
        data.notified++;
        data.condition.notify_one();
        while (data.awoken != 1)
            data.condition.wait(lock);
        BOOST_CHECK(lock ? true : false);
        BOOST_CHECK_EQUAL(data.awoken, 1);

        boost::thread::sleep(delay(1));
        data.notified++;
        data.condition.notify_one();
        while (data.awoken != 2)
            data.condition.wait(lock);
        BOOST_CHECK(lock ? true : false);
        BOOST_CHECK_EQUAL(data.awoken, 2);

        boost::thread::sleep(delay(1));
        data.notified++;
        data.condition.notify_one();
        while (data.awoken != 3)
            data.condition.wait(lock);
        BOOST_CHECK(lock ? true : false);
        BOOST_CHECK_EQUAL(data.awoken, 3);

        boost::thread::sleep(delay(1));
        data.notified++;
        data.condition.notify_one();
        while (data.awoken != 4)
            data.condition.wait(lock);
        BOOST_CHECK(lock ? true : false);
        BOOST_CHECK_EQUAL(data.awoken, 4);
    }

    thread.join();
    BOOST_CHECK_EQUAL(data.awoken, 4);
}

void test_condition_waits()
{
    // We should have already tested notify_one here, so
    // a timed test with the default execution_monitor::use_condition
    // should be OK, and gives the fastest performance
    timed_test(&do_test_condition_waits, 12);
}

boost::unit_test_framework::test_suite* init_unit_test_suite(int, char*[])
{
    boost::unit_test_framework::test_suite* test =
        BOOST_TEST_SUITE("Boost.Threads: condition test suite");

    test->add(BOOST_TEST_CASE(&test_condition_notify_one));
    test->add(BOOST_TEST_CASE(&test_condition_notify_all));
    test->add(BOOST_TEST_CASE(&test_condition_waits));

    return test;
}
