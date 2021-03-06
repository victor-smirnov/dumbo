
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

//#define WIN32_LEAN_AND_MEAN

#include "../../tools/bzero_struct.hpp"
#include "../message/fiber_io_message.hpp"

#include <boost/mpl/size_t.hpp>
#include <boost/variant.hpp>



//#define _INC_WINDOWS



#include <winsock2.h>
#include <ws2tcpip.h>
//#include <windows.h>

#pragma comment (lib, "Ws2_32.lib")

#include <stdint.h>
#include <stdio.h>
#include <sys/types.h>
#include <cstring>
#include <string>


#include <iostream>
#include <memory>


namespace dumbo {
namespace v1 {
namespace reactor {    

using ssize_t = std::int64_t;
    
class IPAddress {
    
    using AddressT = boost::variant<in_addr, in6_addr>;
    
    AddressT address_{};
public:
    IPAddress(const char* addr, bool v4 = true);
    IPAddress();
    
    IPAddress(uint8_t oct3, uint8_t oct2, uint8_t oct1, uint8_t oct0);
    
    IPAddress(const uint8_t* octets, bool v4 = true);
    
    bool is_v4() const; 
    
    std::string to_string() const;
    
    AddressT& address() {return address_;}
    const AddressT& address() const {return address_;}
    
    in_addr& to_in_addr() {return boost::get<in_addr>(address_);}
    const in_addr& to_in_addr() const {return boost::get<in_addr>(address_);}
    
    in6_addr& to_in6_addr() {return boost::get<in6_addr>(address_);}
    const in6_addr& to_in6_addr() const {return boost::get<in6_addr>(address_);}
};

std::ostream& operator<<(std::ostream& out, const IPAddress& addr);






class StreamSocket: public std::enable_shared_from_this<StreamSocket> {
protected:
    IPAddress ip_address_;
    uint16_t ip_port_;
    SOCKET socket_fd_{};
public:
    StreamSocket(const IPAddress& ip_address, uint16_t ip_port ):
        ip_address_(ip_address),
        ip_port_(ip_port)
    {}
    
    virtual ~StreamSocket() noexcept {}
    
    const IPAddress& address() const {return ip_address_;}
    uint16_t port() const {return ip_port_;}
    
    SOCKET fd() const {return socket_fd_;}
};

class StreamSocketConnection {
    std::shared_ptr<StreamSocket> socket_;
    SOCKET connection_fd_;
    
public:
    StreamSocketConnection(SOCKET connection_fd, const std::shared_ptr<StreamSocket>& socket);
    
    virtual ~StreamSocketConnection() noexcept;
    
    std::shared_ptr<StreamSocket>& socket() {return socket_;}
    const std::shared_ptr<StreamSocket>& socket() const {return socket_;}
    
    ssize_t read(char* data, size_t size);
    ssize_t write(const char* data, size_t size);
    
    SOCKET fd() const {return connection_fd_;}
};


    
class StreamServerSocket: public StreamSocket {
    
    sockaddr_in sock_address_;
    bool closed_{false};
    
public:
    StreamServerSocket ( const IPAddress& ip_address, uint16_t ip_port );
    
    virtual ~StreamServerSocket() noexcept;

    void listen();
    
    std::unique_ptr<StreamSocketConnection> accept();
};
    
    
}}}


