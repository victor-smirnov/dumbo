
//          Copyright Oliver Kowalke 2013.
// Distributed under the Boost Software License, Version 1.0.
//    (See accompanying file LICENSE_1_0.txt or copy at
//          http://www.boost.org/LICENSE_1_0.txt)
//
// This test is based on the tests of Boost.Thread

#include <sstream>
#include <string>

#include <boost/test/unit_test.hpp>

#include <dumbo/v1/fiber/all.hpp>

int value1 = 0;
int value2 = 0;

void fn1( dumbo::v1::fibers::barrier & b) {
    ++value1;
    dumbo::v1::this_fiber::yield();

    b.wait();

    ++value1;
    dumbo::v1::this_fiber::yield();
    ++value1;
    dumbo::v1::this_fiber::yield();
    ++value1;
    dumbo::v1::this_fiber::yield();
    ++value1;
}

void fn2( dumbo::v1::fibers::barrier & b) {
    ++value2;
    dumbo::v1::this_fiber::yield();
    ++value2;
    dumbo::v1::this_fiber::yield();
    ++value2;
    dumbo::v1::this_fiber::yield();

    b.wait();

    ++value2;
    dumbo::v1::this_fiber::yield();
    ++value2;
}

void test_barrier() {
    value1 = 0;
    value2 = 0;

    dumbo::v1::fibers::barrier b( 2);
    dumbo::v1::fibers::fiber f1( dumbo::v1::fibers::launch::post, fn1, std::ref( b) );
    dumbo::v1::fibers::fiber f2( dumbo::v1::fibers::launch::post, fn2, std::ref( b) );

    f1.join();
    f2.join();

    BOOST_CHECK_EQUAL( 5, value1);
    BOOST_CHECK_EQUAL( 5, value2);
}

boost::unit_test::test_suite * init_unit_test_suite( int, char* []) {
    boost::unit_test::test_suite * test =
        BOOST_TEST_SUITE("Boost.Fiber: barrier test suite");

    test->add( BOOST_TEST_CASE( & test_barrier) );

    return test;
}
