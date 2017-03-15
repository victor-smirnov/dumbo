
//          Copyright Oliver Kowalke 2013.
// Distributed under the Boost Software License, Version 1.0.
//    (See accompanying file LICENSE_1_0.txt or copy at
//          http://www.boost.org/LICENSE_1_0.txt)
//
// This test is based on the tests of Boost.Thread 

#include <chrono>
#include <cstdlib>
#include <cstdio>
#include <iostream>
#include <map>
#include <stdexcept>
#include <vector>

#include <boost/test/unit_test.hpp>

#include <dumbo/v1/fiber/all.hpp>

typedef std::chrono::nanoseconds  ns;
typedef std::chrono::milliseconds ms;

int value = 0;

inline
std::chrono::system_clock::time_point delay(int secs, int msecs=0, int nsecs=0) {
    std::chrono::system_clock::time_point t = std::chrono::system_clock::now();
    t += std::chrono::seconds( secs);
    t += std::chrono::milliseconds( msecs);
    //t += std::chrono::nanoseconds( nsecs);

    return t;
}

struct condition_test_data {
    condition_test_data() : notified(0), awoken(0) { }

    dumbo::v1::fibers::mutex mutex;
    dumbo::v1::fibers::condition_variable_any cond;
    int notified;
    int awoken;
};

void condition_test_fiber(condition_test_data* data) {
    try {
    data->mutex.lock();
    while (!(data->notified > 0))
        data->cond.wait(data->mutex);
    data->awoken++;
    } catch ( ... ) {
    }
    data->mutex.unlock();
}

struct cond_predicate {
    cond_predicate(int& var, int val) : _var(var), _val(val) { }

    bool operator()() { return _var == _val; }

    int& _var;
    int _val;
private:
    void operator=(cond_predicate&);
    
};

void notify_one_fn( dumbo::v1::fibers::condition_variable_any & cond) {
	cond.notify_one();
}

void notify_all_fn( dumbo::v1::fibers::condition_variable_any & cond) {
	cond.notify_all();
}

void wait_fn(
	dumbo::v1::fibers::mutex & mtx,
	dumbo::v1::fibers::condition_variable_any & cond) {
	mtx.lock();
	cond.wait( mtx);
	++value;
    mtx.unlock();
}

void test_one_waiter_notify_one() {
	value = 0;
	dumbo::v1::fibers::mutex mtx;
	dumbo::v1::fibers::condition_variable_any cond;

    dumbo::v1::fibers::fiber f1(
                dumbo::v1::fibers::launch::post,
                wait_fn,
                std::ref( mtx),
                std::ref( cond) );
	BOOST_CHECK_EQUAL( 0, value);

	dumbo::v1::fibers::fiber f2(
                dumbo::v1::fibers::launch::post,
                notify_one_fn,
                std::ref( cond) );

	BOOST_CHECK_EQUAL( 0, value);

    f1.join();
    f2.join();

	BOOST_CHECK_EQUAL( 1, value);
}

void test_two_waiter_notify_one() {
	value = 0;
	dumbo::v1::fibers::mutex mtx;
	dumbo::v1::fibers::condition_variable_any cond;

    dumbo::v1::fibers::fiber f1(
                dumbo::v1::fibers::launch::post,
                wait_fn,
                std::ref( mtx),
                std::ref( cond) );
	BOOST_CHECK_EQUAL( 0, value);

    dumbo::v1::fibers::fiber f2(
                dumbo::v1::fibers::launch::post,
                wait_fn,
                std::ref( mtx),
                std::ref( cond) );
	BOOST_CHECK_EQUAL( 0, value);

    dumbo::v1::fibers::fiber f3(
                dumbo::v1::fibers::launch::post,
                notify_one_fn,
                std::ref( cond) );
	BOOST_CHECK_EQUAL( 0, value);

    dumbo::v1::fibers::fiber f4(
                dumbo::v1::fibers::launch::post,
                notify_one_fn,
                std::ref( cond) );
	BOOST_CHECK_EQUAL( 0, value);

    f1.join();
    f2.join();
    f3.join();
    f4.join();

	BOOST_CHECK_EQUAL( 2, value);
}

void test_two_waiter_notify_all() {
	value = 0;
	dumbo::v1::fibers::mutex mtx;
	dumbo::v1::fibers::condition_variable_any cond;

    dumbo::v1::fibers::fiber f1(
                dumbo::v1::fibers::launch::post,
                wait_fn,
                std::ref( mtx),
                std::ref( cond) );
	BOOST_CHECK_EQUAL( 0, value);

    dumbo::v1::fibers::fiber f2(
                dumbo::v1::fibers::launch::post,
                wait_fn,
                std::ref( mtx),
                std::ref( cond) );
	BOOST_CHECK_EQUAL( 0, value);

    dumbo::v1::fibers::fiber f3(
                dumbo::v1::fibers::launch::post,
                notify_all_fn,
                std::ref( cond) );
	BOOST_CHECK_EQUAL( 0, value);

    dumbo::v1::fibers::fiber f4(
                dumbo::v1::fibers::launch::post,
                wait_fn,
                std::ref( mtx),
                std::ref( cond) );
	BOOST_CHECK_EQUAL( 0, value);

    dumbo::v1::fibers::fiber f5(
                dumbo::v1::fibers::launch::post,
                notify_all_fn,
                std::ref( cond) );
	BOOST_CHECK_EQUAL( 0, value);

    f1.join();
    f2.join();
    f3.join();
    f4.join();
    f5.join();

	BOOST_CHECK_EQUAL( 3, value);
}

int test1 = 0;
int test2 = 0;

int runs = 0;

void fn1( dumbo::v1::fibers::mutex & m, dumbo::v1::fibers::condition_variable_any & cv) {
    m.lock();
    BOOST_CHECK(test2 == 0);
    test1 = 1;
    cv.notify_one();
    while (test2 == 0) {
        cv.wait(m);
    }
    BOOST_CHECK(test2 != 0);
    m.unlock();
}

void fn2( dumbo::v1::fibers::mutex & m, dumbo::v1::fibers::condition_variable_any & cv) {
    m.lock();
    BOOST_CHECK(test2 == 0);
    test1 = 1;
    cv.notify_one();
    std::chrono::system_clock::time_point t0 = std::chrono::system_clock::now();
    std::chrono::system_clock::time_point t = t0 + ms(250);
    int count=0;
    while (test2 == 0 && cv.wait_until(m, t) == dumbo::v1::fibers::cv_status::no_timeout)
        count++;
    std::chrono::system_clock::time_point t1 = std::chrono::system_clock::now();
    if (runs == 0) {
        BOOST_CHECK(t1 - t0 < ms(250));
        BOOST_CHECK(test2 != 0);
    } else {
        BOOST_CHECK(t1 - t0 - ms(250) < ms(count*250+5+1000));
        BOOST_CHECK(test2 == 0);
    }
    ++runs;
    m.unlock();
}

class Pred {
     int    &   i_;

public:
    explicit Pred(int& i) :
        i_(i)
    {}

    bool operator()()
    { return i_ != 0; }
};

void fn3( dumbo::v1::fibers::mutex & m, dumbo::v1::fibers::condition_variable_any & cv) {
    m.lock();
    BOOST_CHECK(test2 == 0);
    test1 = 1;
    cv.notify_one();
    std::chrono::steady_clock::time_point t0 = std::chrono::steady_clock::now();
    std::chrono::steady_clock::time_point t = t0 + ms(250);
    bool r = cv.wait_until(m, t, Pred(test2));
    std::chrono::steady_clock::time_point t1 = std::chrono::steady_clock::now();
    if (runs == 0) {
        BOOST_CHECK(t1 - t0 < ms(250));
        BOOST_CHECK(test2 != 0);
        BOOST_CHECK(r);
    } else {
        BOOST_CHECK(t1 - t0 - ms(250) < ms(250+2));
        BOOST_CHECK(test2 == 0);
        BOOST_CHECK(!r);
    }
    ++runs;
    m.unlock();
}

void fn4( dumbo::v1::fibers::mutex & m, dumbo::v1::fibers::condition_variable_any & cv) {
    m.lock();
    BOOST_CHECK(test2 == 0);
    test1 = 1;
    cv.notify_one();
    std::chrono::steady_clock::time_point t0 = std::chrono::steady_clock::now();
    int count=0;
    while (test2 == 0 && cv.wait_for(m, ms(250)) == dumbo::v1::fibers::cv_status::no_timeout)
        count++;
    std::chrono::steady_clock::time_point t1 = std::chrono::steady_clock::now();
    if (runs == 0) {
        BOOST_CHECK(t1 - t0 < ms(250));
        BOOST_CHECK(test2 != 0);
    } else {
        BOOST_CHECK(t1 - t0 - ms(250) < ms(count*250+5+1000));
        BOOST_CHECK(test2 == 0);
    }
    ++runs;
    m.unlock();
}

void fn5( dumbo::v1::fibers::mutex & m, dumbo::v1::fibers::condition_variable_any & cv) {
    m.lock();
    BOOST_CHECK(test2 == 0);
    test1 = 1;
    cv.notify_one();
    std::chrono::steady_clock::time_point t0 = std::chrono::steady_clock::now();
    int count=0;
    cv.wait_for(m, ms(250), Pred(test2));
    count++;
    std::chrono::steady_clock::time_point t1 = std::chrono::steady_clock::now();
    if (runs == 0) {
        BOOST_CHECK(t1 - t0 < ms(250+1000));
        BOOST_CHECK(test2 != 0);
    } else {
        BOOST_CHECK(t1 - t0 - ms(250) < ms(count*250+2));
        BOOST_CHECK(test2 == 0);
    }
    ++runs;
    m.unlock();
}

void do_test_condition_wait() {
    test1 = 0;
    test2 = 0;
    runs = 0;

    dumbo::v1::fibers::mutex m;
    dumbo::v1::fibers::condition_variable_any cv;
    m.lock();
    dumbo::v1::fibers::fiber f( dumbo::v1::fibers::launch::post, & fn1, std::ref( m), std::ref( cv) );
    BOOST_CHECK(test1 == 0);
    while (test1 == 0)
        cv.wait(m);
    BOOST_CHECK(test1 != 0);
    test2 = 1;
    m.unlock();
    cv.notify_one();
    f.join();
}

void test_condition_wait() {
    dumbo::v1::fibers::fiber( dumbo::v1::fibers::launch::post, & do_test_condition_wait).join();
    do_test_condition_wait();
}

void do_test_condition_wait_until() {
    test1 = 0;
    test2 = 0;
    runs = 0;

    dumbo::v1::fibers::mutex m;
    dumbo::v1::fibers::condition_variable_any cv;
    {
        m.lock();
        dumbo::v1::fibers::fiber f( dumbo::v1::fibers::launch::post, & fn2, std::ref( m), std::ref( cv) );
        BOOST_CHECK(test1 == 0);
        while (test1 == 0)
            cv.wait(m);
        BOOST_CHECK(test1 != 0);
        test2 = 1;
        m.unlock();
        cv.notify_one();
        f.join();
    }
    test1 = 0;
    test2 = 0;
    {
        m.lock();
        dumbo::v1::fibers::fiber f( dumbo::v1::fibers::launch::post, & fn2, std::ref( m), std::ref( cv) );
        BOOST_CHECK(test1 == 0);
        while (test1 == 0)
            cv.wait(m);
        BOOST_CHECK(test1 != 0);
        m.unlock();
        f.join();
    }
}

void test_condition_wait_until() {
    dumbo::v1::fibers::fiber( dumbo::v1::fibers::launch::post, & do_test_condition_wait_until).join();
    do_test_condition_wait_until();
}

void do_test_condition_wait_until_pred() {
    test1 = 0;
    test2 = 0;
    runs = 0;

    dumbo::v1::fibers::mutex m;
    dumbo::v1::fibers::condition_variable_any cv;
    {
        m.lock();
        dumbo::v1::fibers::fiber f( dumbo::v1::fibers::launch::post, & fn3, std::ref( m), std::ref( cv) );
        BOOST_CHECK(test1 == 0);
        while (test1 == 0)
            cv.wait(m);
        BOOST_CHECK(test1 != 0);
        test2 = 1;
        m.unlock();
        cv.notify_one();
        f.join();
    }
    test1 = 0;
    test2 = 0;
    {
        m.lock();
        dumbo::v1::fibers::fiber f( dumbo::v1::fibers::launch::post, & fn3, std::ref( m), std::ref( cv) );
        BOOST_CHECK(test1 == 0);
        while (test1 == 0)
            cv.wait(m);
        BOOST_CHECK(test1 != 0);
        m.unlock();
        f.join();
    }
}

void test_condition_wait_until_pred() {
    dumbo::v1::fibers::fiber( dumbo::v1::fibers::launch::post, & do_test_condition_wait_until_pred).join();
    do_test_condition_wait_until_pred();
}

void do_test_condition_wait_for() {
    test1 = 0;
    test2 = 0;
    runs = 0;

    dumbo::v1::fibers::mutex m;
    dumbo::v1::fibers::condition_variable_any cv;
    {
        m.lock();
        dumbo::v1::fibers::fiber f( dumbo::v1::fibers::launch::post, & fn4, std::ref( m), std::ref( cv) );
        BOOST_CHECK(test1 == 0);
        while (test1 == 0)
            cv.wait(m);
        BOOST_CHECK(test1 != 0);
        test2 = 1;
        m.unlock();
        cv.notify_one();
        f.join();
    }
    test1 = 0;
    test2 = 0;
    {
        m.lock();
        dumbo::v1::fibers::fiber f( dumbo::v1::fibers::launch::post, & fn4, std::ref( m), std::ref( cv) );
        BOOST_CHECK(test1 == 0);
        while (test1 == 0)
            cv.wait(m);
        BOOST_CHECK(test1 != 0);
        m.unlock();
        f.join();
    }
}

void test_condition_wait_for() {
    dumbo::v1::fibers::fiber( dumbo::v1::fibers::launch::post, & do_test_condition_wait_for).join();
    do_test_condition_wait_for();
}

void do_test_condition_wait_for_pred() {
    test1 = 0;
    test2 = 0;
    runs = 0;

    dumbo::v1::fibers::mutex m;
    dumbo::v1::fibers::condition_variable_any cv;
    {
        m.lock();
        dumbo::v1::fibers::fiber f( dumbo::v1::fibers::launch::post, & fn5, std::ref( m), std::ref( cv) );
        BOOST_CHECK(test1 == 0);
        while (test1 == 0)
            cv.wait(m);
        BOOST_CHECK(test1 != 0);
        test2 = 1;
        m.unlock();
        cv.notify_one();
        f.join();
    }
    test1 = 0;
    test2 = 0;
    {
        m.lock();
        dumbo::v1::fibers::fiber f( dumbo::v1::fibers::launch::post, & fn5, std::ref( m), std::ref( cv) );
        BOOST_CHECK(test1 == 0);
        while (test1 == 0)
            cv.wait(m);
        BOOST_CHECK(test1 != 0);
        m.unlock();
        f.join();
    }
}

void test_condition_wait_for_pred() {
    dumbo::v1::fibers::fiber( dumbo::v1::fibers::launch::post, & do_test_condition_wait_for_pred).join();
    do_test_condition_wait_for_pred();
}

boost::unit_test::test_suite * init_unit_test_suite( int, char* []) {
    boost::unit_test::test_suite * test =
        BOOST_TEST_SUITE("Boost.Fiber: condition_variable_any test suite");

    test->add( BOOST_TEST_CASE( & test_one_waiter_notify_one) );
    test->add( BOOST_TEST_CASE( & test_two_waiter_notify_one) );
    test->add( BOOST_TEST_CASE( & test_two_waiter_notify_all) );
    test->add( BOOST_TEST_CASE( & test_condition_wait) );
    test->add( BOOST_TEST_CASE( & test_condition_wait_until) );
    test->add( BOOST_TEST_CASE( & test_condition_wait_until_pred) );
    test->add( BOOST_TEST_CASE( & test_condition_wait_for) );
    test->add( BOOST_TEST_CASE( & test_condition_wait_for_pred) );

	return test;
}
