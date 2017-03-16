
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

#include <dumbo/v1/fiber/context.hpp>

namespace dumbo {
namespace v1 {
namespace reactor {

using FiberContext = dumbo::v1::fibers::context;    
    
class Message {
protected:
    int cpu_;
    FiberContext* fiber_context_;
    
    bool return_ {false};
public:
    Message(int cpu, FiberContext* fiber_context): 
        cpu_(cpu), 
        fiber_context_(fiber_context)
    {}
    
    virtual ~Message() {}
    
    int cpu() const {return cpu_;}
    bool is_return() const {return return_;}
    FiberContext* fiber_context() {return fiber_context_;}
    
    virtual void process() = 0;
    virtual void finish()  = 0;
};
    
class SmpBase {
public:
    SmpBase();
    
    void submit_to(int cpu, const Message*);
    Message* receive();
};
    
}}}
