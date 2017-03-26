
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
    dr::Application app(argc, argv);
    
    app.set_shutdown_hook([](){
        std::cout << "Custon shutdown hook at " << dr::engine().cpu() << std::endl;
        dr::engine().stop();
    });
    
    
    auto vv = app.run([](){
        std::cout << "Hello from Dumbo App! " << std::endl;
        
        /*auto value = dr::engine().run_at(1, [=]() {
            std::cout << "From thread " << dr::engine().cpu() << std::endl;
            return std::string("ABCDEF");
        });
    
        std::cout << "returned value = " << value << std::endl;
        */
        
        try {
            dr::File file("../file.bin", dr::FileFlags::RDWR);
            
            int buf_size = 4096*1024;
            uint8_t* data = (uint8_t*)aligned_alloc(4096, buf_size);
            
            for (int c = 0; c < buf_size; c++) {data[c] = 0xEE;}
            
            dr::FileIOBatch batch;
            
            for (int c = 0; c < 2000; c++) 
            {
                batch.add_read(data + c * 512, c * 512, 512);
            }
            
            //std::cout << "process batch1: " << file.process_batch(batch) << " -- " << batch.submited() << std::endl;
                      
                        
            for (int c = 0; c < 32; c++) 
            {
                std::cout << std::hex << (uint16_t) data[c] << std::dec << " ";
            }
            
            std::cout << std::endl;
            
            
            dr::FileIOBatch batch2;
            
            for (int c = 0; c < 2000; c++) 
            {
                batch2.add_write(data + c * 512, c * 512, 512);
            }
            
            std::cout << "process batch2: " << file.process_batch(batch2) << " -- " << batch2.submited() << std::endl;
            
            file.close();
            
            dr::app().shutdown();
        }
        catch (...) {
            dr::app().shutdown();
            throw;
        }
        
        return 5678;
    });

    std::cout << "vv = " << vv << std::endl;
    
    
    /*
    auto fn = [=]{
        for (size_t c = 0; c < 1000; c++) {
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
