// Copyright (C) 2001-2003
// William E. Kempf
// Copyright (C) 2007 Anthony Williams
//
//  Distributed under the Boost Software License, Version 1.0. (See accompanying
//  file LICENSE_1_0.txt or copy at http://www.boost.org/LICENSE_1_0.txt)

#include <iostream>
#include <mutex>

#include <boost/test/unit_test.hpp>

#include <dumbo/v1/fiber/all.hpp>

dumbo::v1::fibers::mutex check_mutex;
dumbo::v1::fibers::mutex fss_mutex;
int fss_instances = 0;
int fss_total = 0;

struct fss_value_t {
    fss_value_t() {
        std::unique_lock<dumbo::v1::fibers::mutex> lock(fss_mutex);
        ++fss_instances;
        ++fss_total;
        value = 0;
    }
    ~fss_value_t() {
        std::unique_lock<dumbo::v1::fibers::mutex> lock(fss_mutex);
        --fss_instances;
    }
    int value;
};

dumbo::v1::fibers::fiber_specific_ptr<fss_value_t> fss_value;

void fss_fiber() {
    fss_value.reset(new fss_value_t());
    for (int i=0; i<1000; ++i) {
        int& n = fss_value->value;
        if (n != i) {
            std::unique_lock<dumbo::v1::fibers::mutex> lock(check_mutex);
            BOOST_CHECK_EQUAL(n, i);
        }
        ++n;
    }
}

void fss() {
    fss_instances = 0;
    fss_total = 0;

    dumbo::v1::fibers::fiber f1( dumbo::v1::fibers::launch::post, fss_fiber);
    dumbo::v1::fibers::fiber f2( dumbo::v1::fibers::launch::post, fss_fiber);
    dumbo::v1::fibers::fiber f3( dumbo::v1::fibers::launch::post, fss_fiber);
    dumbo::v1::fibers::fiber f4( dumbo::v1::fibers::launch::post, fss_fiber);
    dumbo::v1::fibers::fiber f5( dumbo::v1::fibers::launch::post, fss_fiber);
    f1.join();
    f2.join();
    f3.join();
    f4.join();
    f5.join();

    std::cout
        << "fss_instances = " << fss_instances
        << "; fss_total = " << fss_total
        << "\n";
    std::cout.flush();

    BOOST_CHECK_EQUAL(fss_instances, 0);
    BOOST_CHECK_EQUAL(fss_total, 5);
}

void test_fss() {
    dumbo::v1::fibers::fiber( dumbo::v1::fibers::launch::post, fss).join();
}

bool fss_cleanup_called=false;

struct Dummy {
};

void fss_custom_cleanup(Dummy* d) {
    delete d;
    fss_cleanup_called=true;
}

dumbo::v1::fibers::fiber_specific_ptr<Dummy> fss_with_cleanup(fss_custom_cleanup);

void fss_fiber_with_custom_cleanup() {
    fss_with_cleanup.reset(new Dummy);
}

void fss_with_custom_cleanup() {
    dumbo::v1::fibers::fiber f( dumbo::v1::fibers::launch::post, fss_fiber_with_custom_cleanup);
    try {
        f.join();
    } catch(...) {
        f.join();
        throw;
    }

    BOOST_CHECK(fss_cleanup_called);
}

void test_fss_with_custom_cleanup() {
    dumbo::v1::fibers::fiber( dumbo::v1::fibers::launch::post, fss_with_custom_cleanup).join();
}

Dummy* fss_object=new Dummy;

void fss_fiber_with_custom_cleanup_and_release() {
    fss_with_cleanup.reset(fss_object);
    fss_with_cleanup.release();
}

void do_test_fss_does_no_cleanup_after_release() {
    fss_cleanup_called=false;
    dumbo::v1::fibers::fiber f( dumbo::v1::fibers::launch::post, fss_fiber_with_custom_cleanup_and_release);
    try {
        f.join();
    } catch(...) {
        f.join();
        throw;
    }

    BOOST_CHECK(!fss_cleanup_called);
    if(!fss_cleanup_called) {
        delete fss_object;
    }
}

struct dummy_class_tracks_deletions {
    static unsigned deletions;

    ~dummy_class_tracks_deletions() {
        ++deletions;
    }
};

unsigned dummy_class_tracks_deletions::deletions=0;

dumbo::v1::fibers::fiber_specific_ptr<dummy_class_tracks_deletions> fss_with_null_cleanup(NULL);

void fss_fiber_with_null_cleanup(dummy_class_tracks_deletions* delete_tracker) {
    fss_with_null_cleanup.reset(delete_tracker);
}

void do_test_fss_does_no_cleanup_with_null_cleanup_function() {
    dummy_class_tracks_deletions* delete_tracker=new dummy_class_tracks_deletions;
    dumbo::v1::fibers::fiber f( dumbo::v1::fibers::launch::post, [&delete_tracker](){
        fss_fiber_with_null_cleanup( delete_tracker); });
    try {
        f.join();
    } catch(...) {
        f.join();
        throw;
    }

    BOOST_CHECK(!dummy_class_tracks_deletions::deletions);
    if(!dummy_class_tracks_deletions::deletions) {
        delete delete_tracker;
    }
}

void test_fss_does_no_cleanup_after_release() {
    dumbo::v1::fibers::fiber( dumbo::v1::fibers::launch::post, do_test_fss_does_no_cleanup_after_release).join();
}

void test_fss_does_no_cleanup_with_null_cleanup_function() {
    dumbo::v1::fibers::fiber( dumbo::v1::fibers::launch::post, do_test_fss_does_no_cleanup_with_null_cleanup_function).join();
}


void fiber_with_local_fss_ptr() {
    {
        dumbo::v1::fibers::fiber_specific_ptr<Dummy> local_fss(fss_custom_cleanup);

        local_fss.reset(new Dummy);
    }
    BOOST_CHECK(fss_cleanup_called);
    fss_cleanup_called=false;
}

void fss_does_not_call_cleanup_after_ptr_destroyed() {
    dumbo::v1::fibers::fiber( dumbo::v1::fibers::launch::post, fiber_with_local_fss_ptr).join();
    BOOST_CHECK(!fss_cleanup_called);
}

void test_fss_does_not_call_cleanup_after_ptr_destroyed() {
    dumbo::v1::fibers::fiber( dumbo::v1::fibers::launch::post, fss_does_not_call_cleanup_after_ptr_destroyed).join();
}


void fss_cleanup_not_called_for_null_pointer() {
    dumbo::v1::fibers::fiber_specific_ptr<Dummy> local_fss(fss_custom_cleanup);
    local_fss.reset(new Dummy);
    fss_cleanup_called=false;
    local_fss.reset(0);
    BOOST_CHECK(fss_cleanup_called);
    fss_cleanup_called=false;
    local_fss.reset(new Dummy);
    BOOST_CHECK(!fss_cleanup_called);
}

void test_fss_cleanup_not_called_for_null_pointer() {
    dumbo::v1::fibers::fiber( dumbo::v1::fibers::launch::post, fss_cleanup_not_called_for_null_pointer).join();
}


void fss_at_the_same_adress() {
  for(int i=0; i<2; i++) {
    dumbo::v1::fibers::fiber_specific_ptr<Dummy> local_fss(fss_custom_cleanup);
    local_fss.reset(new Dummy);
    fss_cleanup_called=false;
    BOOST_CHECK(fss_cleanup_called);
    fss_cleanup_called=false;
    BOOST_CHECK(!fss_cleanup_called);
  }
}

void test_fss_at_the_same_adress() {
    dumbo::v1::fibers::fiber( dumbo::v1::fibers::launch::post, fss_at_the_same_adress).join();
}

boost::unit_test::test_suite* init_unit_test_suite(int, char*[]) {
    boost::unit_test::test_suite* test =
        BOOST_TEST_SUITE("Boost.Fiber: fss test suite");

    test->add(BOOST_TEST_CASE(test_fss));
    test->add(BOOST_TEST_CASE(test_fss_with_custom_cleanup));
    test->add(BOOST_TEST_CASE(test_fss_does_no_cleanup_after_release));
    test->add(BOOST_TEST_CASE(test_fss_does_no_cleanup_with_null_cleanup_function));
    test->add(BOOST_TEST_CASE(test_fss_does_not_call_cleanup_after_ptr_destroyed));
    test->add(BOOST_TEST_CASE(test_fss_cleanup_not_called_for_null_pointer));

    return test;
}
