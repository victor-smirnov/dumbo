
// Copyright 2016 Victor Smirnov
//
// Licensed under the Apache License, Version 2.0 (the "License");
// you may not use this file except in compliance with the License.
// You may obtain a copy of the License at
//
//     http://www.apache.org/licenses/LICENSE-2.0
//
// Unless required by applicable law or agreed to in writing, software
// distributed under the License is distributed on an "AS IS" BASIS,
// WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
// See the License for the specific language governing permissions and
// limitations under the License.


#pragma once

#include <iostream>
#include <type_traits>
#include <memory>
#include <condition_variable>
#include <mutex>


namespace dumbo {
namespace v1 {
namespace tools {

template <typename T>
class CountDownLatch {
    
    using MutexT = std::mutex;
    
	T value_;
	MutexT mutex_;
	std::condition_variable cv_;
public:
	CountDownLatch(): value_() {}
	CountDownLatch(const T& value): value_(value) {}
	
	MutexT& mutex() {return mutex_;}
	const MutexT& mutex() const {return mutex_;}

	void inc() {
		std::unique_lock<MutexT> lk(mutex_);
		++value_;
	}

	void dec() {
		std::unique_lock<MutexT> lk(mutex_);
		--value_;
	}

	const T& get() const {
		std::unique_lock<MutexT> lk(mutex_);
		return value_;
	}

	void wait(const T& value)
	{
		std::unique_lock<MutexT> lk(mutex_);
		cv_.wait(lk, [&]{return value_ == value;});
	}

	template< class Rep, class Period, class Predicate >
	void wait_for(const T& value, const std::chrono::duration<Rep, Period>& rel_time)
	{
		std::unique_lock<MutexT> lk(mutex_);
		cv_.wait_for(lk, rel_time, [&]{return value_ == value;});
	}

	void wait_for(const T& value, size_t rel_time_ms)
	{
		std::unique_lock<MutexT> lk(mutex_);
		cv_.wait_for(lk, std::chrono::milliseconds(rel_time_ms), [&]{return value_ == value;});
	}
};

}}}
