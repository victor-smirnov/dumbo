
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

#include "reactor.hpp"
#include "linux/smp.hpp"

#include <functional>
#include <thread>
#include <vector>
#include <memory>

namespace dumbo {
namespace v1 {
namespace reactor {

class Application {
    
    std::vector<std::unique_ptr<Reactor>> reactors_;
    
public:
    Application(int argc, char** argv): Application(argc, argv, nullptr) {}
    Application(int argc, char** argv, char** envp);
    
    Application(const Application&) = delete;
    Application(Application&&);
    
    void run(std::function<void()> fn);
    
};
    
}}}
