
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

#include <stdint.h>
#include <string>
#include <sys/types.h>
#include <sys/stat.h>
#include <sys/uio.h>
#include <fcntl.h>
#include <stdlib.h>

#include <iostream>

namespace dumbo {
namespace v1 {
namespace reactor {


class BuffersBase {
public:
    virtual ~BuffersBase() {}
    
    virtual iovec* vectors() = 0;
    virtual const iovec* vectors() const = 0;
    virtual size_t num() const = 0;
};


template <size_t SIZE>
class StaticBuffers: public BuffersBase {
    iovec vec_[SIZE];
public:
    StaticBuffers() {}
    
    virtual ~StaticBuffers(){}
    
    virtual iovec* vectors() {return vec_;}
    virtual const iovec* vectors() const {return vec_;}
    
    virtual size_t num() const {return SIZE;}
    
    iovec* vectors(size_t idx) {return &vec_[idx];}
    const iovec* vectors(size_t idx) const {return &vec_[idx];}
};

template <size_t Size>
std::ostream& operator<<(std::ostream& os, const StaticBuffers<Size>& buf) 
{
    bool first = true;
    
    os << "[";
    
    for (size_t c = 0; c < Size; c++)
    {
        if (!first) 
        {
            os << ", ";
        }
        else first = false;
        
        os << "{" << buf.vectors(c)->iov_base << ", " << buf.vectors(c)->iov_len << "}";
    }
    
    os << "]";
    
    return os;
}

namespace detail {
    struct StaticBuffersH {
        template <size_t Idx, typename Buf, typename PtrT, typename SizeT, typename... Args>
        static void process(Buf& buf, PtrT&& ptr, SizeT&& size, Args&&... args) 
        {
            buf.vectors(Idx)->iov_base = ptr;
            buf.vectors(Idx)->iov_len  = size;
            
            process<Idx + 1>(buf, std::forward<Args>(args)...);
        }
        
        template <size_t Idx, typename Buf, typename PtrT, typename SizeT>
        static void process(Buf& buf, PtrT&& ptr, SizeT&& size) 
        {
            buf.vectors(Idx)->iov_base = ptr;
            buf.vectors(Idx)->iov_len  = size;
        }
    };
}

template <typename... Args>
StaticBuffers<sizeof...(Args)/2> make_bufferv(Args&&... args)
{
    static_assert(sizeof...(Args) % 2 == 0, "Number of arguments must be even");
    
    StaticBuffers<sizeof...(Args)/2> buf;
    detail::StaticBuffersH::process<0>(buf, std::forward<Args>(args)...);
    
    return buf;
}



    
}}}
