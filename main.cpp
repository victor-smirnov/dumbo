
#include "dumbo/v1/fiber/all.hpp"

#include <iostream>

namespace df  = dumbo::v1::fibers;

int main(int argc, char **argv) 
{
    df::fiber ff([]{
        std::cout << "Hello from fiber \n" << std::endl;
    });
    
    ff.join();
    
    return 0;
}
