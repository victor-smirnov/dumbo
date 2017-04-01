
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

#include "msvc_smp.hpp"
#include "../message/fiber_io_message.hpp"
#include "../ring_buffer.hpp"

#include <memory>
#include <thread>
#include <string>

#include <winsock2.h>
#include <ws2tcpip.h>

#include <stdio.h>

// Need to link with Ws2_32.lib
#pragma comment(lib, "Ws2_32.lib")



namespace dumbo {
namespace v1 {
namespace reactor {

std::string GetErrorMessage(DWORD error_code);
void DumpErrorMessage(DWORD error_code);
void DumpErrorMessage(std::string prefix, DWORD error_code);

using IOBuffer = RingBuffer<Message*>;

struct OVERLAPPEDMsg;

class AIOMessage;


class AIOMessage: public FiberIOMessage {
	
	DWORD size_{};
	ULONG_PTR completion_key_{};
	OVERLAPPEDMsg* overlapped_{};

public:
	AIOMessage(int cpu, FiberContext* fiber_context = fibers::context::active()): 
		FiberIOMessage(cpu, 1, fiber_context) 
	{}

	virtual void report(DWORD size, ULONG_PTR completion_key, OVERLAPPEDMsg* overlapped) {
		size_ = size; completion_key_ = completion_key; overlapped_ = overlapped;
	}

	DWORD size() const { return size_; };
	ULONG_PTR completion_key() const { return completion_key_; }
	OVERLAPPEDMsg* overlapped() const { return overlapped_; }

	virtual std::string describe() { return "AIOMessage"; }
};



struct OVERLAPPEDMsg : OVERLAPPED {
	AIOMessage* msg_;
};



class IOPoller {
    
    static constexpr int BATCH_SIZE = 512;
	
	HANDLE completion_port_{};
    
	IOBuffer& buffer_;

public:
    IOPoller(IOBuffer& buffer);
    
    ~IOPoller();
    
    void poll();
    
    
	HANDLE completion_port() { return completion_port_; }
};
    
}}}
