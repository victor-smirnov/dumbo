
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

#include "smp.hpp"
#include "../message/message.hpp"

#include <memory>
#include <thread>

#include <linux/aio_abi.h>


namespace dumbo {
namespace v1 {
namespace reactor {

int io_setup(unsigned nr, aio_context_t *ctxp);
int io_destroy(aio_context_t ctx);
int io_submit(aio_context_t ctx, long nr,  struct iocb **iocbpp); 
    
class IOPoller {
    std::shared_ptr<Smp> smp_;
    int epoll_fd_{};
    
    std::thread thread_;
    
    aio_context_t aio_context_{};
    
    bool running_{true};
    
public:
    IOPoller(std::shared_ptr<Smp> smp):
        smp_(smp), thread_([this]{do_polling();}) 
    {}
    
    ~IOPoller();
    void stop();
    
private:
    void do_polling();
};
    
}}}
