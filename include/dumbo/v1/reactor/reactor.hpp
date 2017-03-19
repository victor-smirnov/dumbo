
// Copyright 2017 Victor Smirnov
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

#include "scheduler.hpp"

#include "linux/smp.hpp"

#include <thread>
#include <memory>
#include <atomic>

namespace dumbo {
namespace v1 {
namespace reactor {
    

class Reactor: public std::enable_shared_from_this<Reactor> {
    std::shared_ptr<Smp> smp_ {};
    int cpu_;
    bool own_thread_;
    
    std::thread worker_;
    
    bool running_{false};
    
    Scheduler<Reactor>* scheduler_{};
    
public:
    
    /*class ShutdownMessage: public Message {
        Reactor* reactor_;
    public:
        ShutdownMessage(int cpu, Reactor* reactor): 
            Message(cpu, true), reactor_(reactor) 
        {}
    
        virtual void process() noexcept
        {
            reactor_->shutdown();
        }
        
        virtual void finish() {}
        
        virtual std::string describe() {return "ShutdownMessage: ";}
    };*/
    
    Reactor(std::shared_ptr<Smp> smp, int cpu, bool own_thread):
        smp_(smp), cpu_(cpu), own_thread_(own_thread)
    {
        std::atomic_thread_fence(std::memory_order_seq_cst);
    }
    
    Reactor(const Reactor&) = delete;
    Reactor(Reactor&&) = delete;
    
    Reactor* operator=(const Reactor&) = delete;
    Reactor* operator=(Reactor&&) = delete;
    
    int cpu() const {return cpu_;}
    bool own_thread() const {return own_thread_;}
    

    
    void join() 
    {
        if (own_thread())
        {
            return worker_.join();
        }
    }
    
    template <typename Fn, typename... Args>
    auto run_at(int target_cpu, Fn&& task, Args&&... args) 
    {
        auto ctx = fibers::context::active();
        BOOST_ASSERT_MSG(ctx != nullptr, "Fiber context is null");
        
        auto msg = make_fiber_lambda_message(cpu_, this, ctx, std::forward<Fn>(task), std::forward<Args>(args)...);
        smp_->submit_to(target_cpu, msg.get());
        scheduler_->suspend(ctx);
        
        return msg->result();
    }
    
    template <typename Fn, typename... Args>
    void run_at_async(int target_cpu, Fn&& task, Args&&... args) 
    {
        auto ctx = fibers::context::active();
        BOOST_ASSERT_MSG(ctx != nullptr, "Fiber context is null");
        
        auto msg = make_one_way_lambda_message(cpu_, std::forward<Fn>(task), std::forward<Args>(args)...);
        smp_->submit_to(target_cpu, msg);
    }
    
    friend class Application;
    friend Reactor& engine();
    template <typename> friend class FiberMessage;
    
    void stop() {
        running_ = false;
    }
    
private:
    
    static thread_local Reactor* local_engine_;
    
    void start()
    {
        if (own_thread_) 
        {
            worker_ = std::thread([this](){
                local_engine_ = this;
                event_loop();
            });
        }
        else {
            local_engine_ = this;
        }
    }
    
    void event_loop();
    
    
    void shutdown() 
    {
        running_ = false;
    }
    
    Scheduler<Reactor>* scheduler() {return scheduler_;}
    const Scheduler<Reactor>* scheduler() const {return scheduler_;}
};

Reactor& engine();
    
}}}
