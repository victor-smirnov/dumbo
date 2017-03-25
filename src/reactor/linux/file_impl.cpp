
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


#include <dumbo/v1/reactor/linux/file_impl.hpp>
#include <dumbo/v1/reactor/linux/io_poller.hpp>
#include <dumbo/v1/reactor/reactor.hpp>

#include <dumbo/v1/tools/ptr_cast.hpp>
#include <dumbo/v1/tools/bzero_struct.hpp>
#include <dumbo/v1/tools/perror.hpp>

#ifndef _GNU_SOURCE
#define _GNU_SOURCE
#endif


#include <stdio.h>
#include <unistd.h>
#include <sys/syscall.h>
#include <sys/epoll.h>
#include <sys/eventfd.h>
#include <fcntl.h>
#include <string.h>
#include <stdint.h>
#include <exception>



namespace dumbo {
namespace v1 {
namespace reactor {

File::File(std::string path, FileFlags flags, FileMode mode): 
    message_(engine().cpu()), path_(path) 
{
    fd_ = ::open(path_.c_str(), (int)flags | O_DIRECT, (mode_t)mode);
    if (fd_ < 0)
    {
        tools::rise_perror(tools::SBuf() << "Can't open file " << path);
    }
}

File::~File() noexcept
{
    
}
    
void File::close()
{
    if (::close(fd_) < 0) {
        tools::rise_perror(tools::SBuf() << "Can't close file " << path_);
    }
}

int64_t File::seek(int64_t pos, FileSeek whence) 
{
    off_t res = ::lseek(fd_, pos, (int)whence);
    if (res >= 0) 
    {
        return res;
    }
    else {
        tools::rise_perror(tools::SBuf() << "Error seeking in file " << path_);
    }
}

void File::read(char* buffer, int64_t offset, int64_t size) 
{
    if (((size_t)buffer) % 512) 
    {
        tools::rise_error(tools::SBuf() << "Reading buffer address must be 512-aligned for " << path_);
    }
    
    if (offset % 512) 
    {
        tools::rise_error(tools::SBuf() << "Reading offset must be multiple of 512 for " << path_);
    }
    
    if (size % 512) 
    {
        tools::rise_error(tools::SBuf() << "Reading size must be multiple of 512 for " << path_);
    }
    
    Reactor& r = engine();
    
    iocb block = tools::make_zeroed<iocb>();
    
    block.aio_fildes = fd_;
    block.aio_lio_opcode = IOCB_CMD_PREAD;
    block.aio_reqprio = 0;
    block.aio_buf = (__u64) buffer;
    block.aio_nbytes = size;
    block.aio_offset = offset;
    block.aio_flags = IOCB_FLAG_RESFD;
    block.aio_resfd = r.io_poller().event_fd();
    
    block.aio_data = (__u64) &message_;
    
    
    iocb* pblock = &block;
    
    while (true) 
    {
        int res = io_submit(r.io_poller().aio_context(), 1, &pblock);
    
        if (res > 0) 
        {
            message_.wait_for();
            return;
        } 
        else if (res < 0)
        {
            tools::rise_perror(tools::SBuf() << "Can't submit AIO read operation for file " << path_);
        }
    }
}




void File::write(const char* buffer, int64_t offset, int64_t size)
{
    if (((size_t)buffer) % 512) 
    {
        tools::rise_error(tools::SBuf() << "Writing buffer address must be 512-aligned for " << path_);
    }
    
    
    if (offset % 512) 
    {
        tools::rise_error(tools::SBuf() << "Writing offset must be multiple of 512 for " << path_);
    }
    
    if (size % 512) 
    {
        tools::rise_error(tools::SBuf() << "Writing size must be multiple of 512 for " << path_);
    }
    
    Reactor& r = engine();
    
    iocb block = tools::make_zeroed<iocb>();
    
    block.aio_fildes = fd_;
    block.aio_lio_opcode = IOCB_CMD_PWRITE;
    block.aio_reqprio = 0;
    block.aio_buf = (__u64) buffer;
    block.aio_nbytes = size;
    block.aio_offset = offset;
    block.aio_flags = IOCB_FLAG_RESFD;
    block.aio_resfd = r.io_poller().event_fd();
    
    block.aio_data = (__u64) &message_;
    
    iocb* pblock = &block;
    
    while (true) 
    {
        int res = io_submit(r.io_poller().aio_context(), 1, &pblock);
    
        if (res > 0) 
        {
            message_.wait_for();
            return;
        } 
        else if (res < 0)
        {
            tools::rise_perror(tools::SBuf() << "Can't submit AIO write operation for file " << path_);
        }
    }
}

void File::flush()
{
    
}

    
}}}
