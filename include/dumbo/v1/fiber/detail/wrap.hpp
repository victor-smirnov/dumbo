
//          Copyright Oliver Kowalke 2014.
// Distributed under the Boost Software License, Version 1.0.
//    (See accompanying file LICENSE_1_0.txt or copy at
//          http://www.boost.org/LICENSE_1_0.txt)

#pragma once

#include <type_traits>

#include <boost/config.hpp>
#include <boost/context/detail/invoke.hpp>
#include <boost/context/execution_context.hpp>

#include <dumbo/v1/fiber/detail/config.hpp>
#include <dumbo/v1/fiber/detail/data.hpp>

#ifdef BOOST_HAS_ABI_HEADERS
# include BOOST_ABI_PREFIX
#endif

namespace dumbo {
namespace v1 {    
namespace fibers {
namespace detail {

template< typename Fn1, typename Fn2, typename Tpl  >
class wrapper {
private:
    typename std::decay< Fn1 >::type    fn1_;
    typename std::decay< Fn2 >::type    fn2_;
    typename std::decay< Tpl >::type    tpl_;

public:
    wrapper( Fn1 && fn1, Fn2 && fn2, Tpl && tpl) :
        fn1_( std::move( fn1) ),
        fn2_( std::move( fn2) ),
        tpl_( std::move( tpl) ) {
    }

    wrapper( wrapper const&) = delete;
    wrapper & operator=( wrapper const&) = delete;

    wrapper( wrapper && other) = default;
    wrapper & operator=( wrapper && other) = default;

    boost::context::execution_context< data_t * >
    operator()( boost::context::execution_context< data_t * > && ctx, data_t * dp) {
        return boost::context::detail::invoke(
                std::move( fn1_),
                fn2_,
                tpl_,
                std::forward< boost::context::execution_context< data_t * > >( ctx),
                dp);
    }
};

template< typename Fn1, typename Fn2, typename Tpl  >
wrapper< Fn1, Fn2, Tpl >
wrap( Fn1 && fn1, Fn2 && fn2, Tpl && tpl) {
    return wrapper< Fn1, Fn2, Tpl >(
            std::forward< Fn1 >( fn1),
            std::forward< Fn2 >( fn2),
            std::forward< Tpl >( tpl) );
}


}}}}

#ifdef BOOST_HAS_ABI_HEADERS
#include BOOST_ABI_SUFFIX
#endif


