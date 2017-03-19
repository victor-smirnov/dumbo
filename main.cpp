
#include "dumbo/v1/fiber/all.hpp"
#include "dumbo/v1/reactor/application.hpp"

#include <iostream>
#include <thread>

namespace df  = dumbo::v1::fibers;
namespace dr  = dumbo::v1::reactor;

int main(int argc, char **argv) 
{
    dr::Application app(argc, argv);
    
    app.set_shutdown_hook([](){
        std::cout << "Custon shutdown hook at " << dr::engine().cpu() << std::endl;
        dr::engine().stop();
    });
    
    
    auto vv = app.run([](){
        std::cout << "Hello from Dumbo App! " << std::endl;
        
        auto value = dr::engine().run_at(1, [=]() {
            std::cout << "From thread " << dr::engine().cpu() << std::endl;
            return std::string("ABCDEF");
        });
    
        std::cout << "returned value = " << value << std::endl;
        
        dr::app().shutdown();
        
        return 5678;
    });

    std::cout << "vv = " << vv << std::endl;
    
    return 0;
}
