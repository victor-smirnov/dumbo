
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


#include <dumbo/v1/reactor/message/fiber_io_message.hpp>
#include <dumbo/v1/reactor/reactor.hpp>

#include <boost/assert.hpp>

namespace dumbo {
namespace v1 {
namespace reactor {
    
void FiberIOMessage::finish()
{
    if (--count_ == 0) 
    {
        engine().scheduler()->resume(fiber_context_);
    }
}    


std::string FiberIOMessage::describe()
{
    return "FiberIOMessage";
}


void FiberIOMessage::wait_for()
{
    engine().scheduler()->suspend(fiber_context_);
}

    
}}}
