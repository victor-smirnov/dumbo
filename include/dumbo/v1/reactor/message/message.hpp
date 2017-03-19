
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

#include <exception>
#include <string>

namespace dumbo {
namespace v1 {
namespace reactor {

class Message {
protected:
    int cpu_;
    bool one_way_{false};
    bool return_{false};
    
    std::exception_ptr exception_;
    
public:
    Message(int cpu, bool one_way):
        cpu_(cpu),
        one_way_(one_way)
    {}
    
    virtual ~Message() {}
    
    int cpu() const {return cpu_;}
    
    bool is_one_way() const {return one_way_;}
    bool is_return() const {return return_;}
    bool is_exception() const {return exception_ != nullptr;}
    
    void rethrow() const 
    {
        std::rethrow_exception(exception_);
    }
    
    virtual void process() noexcept = 0;
    virtual void finish() = 0;
    
    virtual std::string describe() = 0;
};

    
}}}
