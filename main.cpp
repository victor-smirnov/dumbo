
#include "dumbo/v1/fiber/all.hpp"
#include "dumbo/v1/reactor/application.hpp"

#include <iostream>
#include <thread>

namespace df  = dumbo::v1::fibers;
namespace dr  = dumbo::v1::reactor;


void print_fiber_sizes(const char* msg) 
{
    std::cout << "Contexts: " << msg << " -- " << df::context::contexts() << std::endl;
}

int main(int argc, char **argv) 
{
	WSADATA wsaData = { 0 };

	// Initialize Winsock
	int iResult = WSAStartup(MAKEWORD(2, 2), &wsaData);
	if (iResult != 0) {
		wprintf(L"WSAStartup failed: %d\n", iResult);
		return 1;
	}

    dr::Application app(argc, argv);
    
    app.set_shutdown_hook([](){
        std::cout << "Custon shutdown hook at " << dr::engine().cpu() << std::endl;
        dr::engine().stop();
    });
    
    
    auto vv = app.run([](){
        std::cout << "Hello from Dumbo App! " << std::endl;
        
		dr::IPAddress addr("127.0.0.1");

		auto socket = std::make_shared<dr::StreamServerSocket>(addr, 5544);
		socket->listen();
		auto conn = socket->accept();

		char buffer[4096];
		
		int64_t size;
		while ((size = conn->read(buffer, sizeof(buffer))) > 0) 
		{			
			std::cout << "Read " << size << " bytes" << std::endl;
			conn->write(buffer, size);
		}

		dr::app().shutdown();

        return 5678;
    });

    std::cout << "vv = " << vv << std::endl;
    
    /*
    
    auto fn = [=]{
        for (size_t c = 0; c < 10000000; c++) {
            dumbo::v1::this_fiber::yield();
        }
    };
    
    print_fiber_sizes("Before");
    
    df::fiber f0(fn);
    
    print_fiber_sizes("+1+");
    
    df::fiber f1(fn);
    
    print_fiber_sizes("+2+");
    
    df::fiber f2(fn);
    
    print_fiber_sizes("+3+");
    
    df::fiber f3(fn);
    
    print_fiber_sizes("+4+");
    
    df::fiber f4(fn);
    
    print_fiber_sizes("+5+");
    
    df::fiber f5(fn);
    
    print_fiber_sizes("+6+");
    
    df::fiber f6(fn);
    
    print_fiber_sizes("+7+");
    
    df::fiber f7(fn);
    
    print_fiber_sizes("+8+");
    
    df::fiber f8(fn);
    
    print_fiber_sizes("+9+");
    
    df::fiber f9(fn);
    
    print_fiber_sizes("+10+");
    
    f0.join();
    
    print_fiber_sizes("+j1+");
    
    f1.join();
    
    print_fiber_sizes("+j2+");
    
    f2.join();
    f3.join();
    f4.join();
    f5.join();
    f6.join();
    f7.join();
    f8.join();
    f9.join();
    
    print_fiber_sizes("+N+");
    */
    
    return 0;
}
