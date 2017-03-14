//          Copyright Oliver Kowalke 2013.
// Distributed under the Boost Software License, Version 1.0.
//    (See accompanying file LICENSE_1_0.txt or copy at
//          http://www.boost.org/LICENSE_1_0.txt)

#pragma once

#include <condition_variable>
#include <chrono>
#include <mutex>

#include <boost/config.hpp>

#include <dumbo/v1/fiber/algo/algorithm.hpp>
#include <dumbo/v1/fiber/context.hpp>
#include <dumbo/v1/fiber/detail/config.hpp>
#include <dumbo/v1/fiber/scheduler.hpp>

#ifdef BOOST_HAS_ABI_HEADERS
#  include BOOST_ABI_PREFIX
#endif

#ifdef _MSC_VER
# pragma warning(push)
# pragma warning(disable:4251)
#endif

namespace dumbo {
namespace v1 {    
namespace fibers {
namespace algo {

class DUMBO_FIBERS_DECL round_robin : public algorithm {
private:
    typedef scheduler::ready_queue_t rqueue_t;

    rqueue_t                    rqueue_{};
    std::mutex                  mtx_{};
    std::condition_variable     cnd_{};
    bool                        flag_{ false };

public:
    round_robin() = default;

    round_robin( round_robin const&) = delete;
    round_robin & operator=( round_robin const&) = delete;

    virtual void awakened( context *) noexcept;

    virtual context * pick_next() noexcept;

    virtual bool has_ready_fibers() const noexcept;

    virtual void suspend_until( std::chrono::steady_clock::time_point const&) noexcept;

    virtual void notify() noexcept;
};

}}}}

#ifdef _MSC_VER
# pragma warning(pop)
#endif

#ifdef BOOST_HAS_ABI_HEADERS
#  include BOOST_ABI_SUFFIX
#endif


