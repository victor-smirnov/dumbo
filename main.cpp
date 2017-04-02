
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
        
		using namespace dr;

		
		/*IPAddress addr("127.0.0.1");

		auto socket = std::make_shared<dr::StreamServerSocket>(addr, 5544);
		socket->listen();
		auto conn = socket->accept();

		char buffer[4096];
		
		int64_t size;
		while ((size = conn->read(buffer, sizeof(buffer))) > 0) 
		{			
			std::cout << "Read " << size << " bytes" << std::endl;
			conn->write(buffer, size);
		}*/
		
		

			size_t buf_size = 1024 * 4096;
			auto buf = dr::allocate_dma_buffer(buf_size);
			for (size_t c = 0; c < buf_size; c++) {
				buf.get()[c] = 0;
			}

			File file("data.bin", FileFlags::DEFAULT | FileFlags::CREATE, FileMode::IDEFLT);

			FileIOBatch batch;

			for (int c = 0; c < buf_size; c += 512)
			{
				batch.add_read(buf.get() + c, c, 512 + (c > 512*10));
			}

			int processed = 0;

			try {
				processed = file.process_batch(batch, false);
			}
			catch (std::exception& ex) {
				std::cout << ex.what() << std::endl;
			}

			std::cout << "Submited " << processed << " of " << batch.nblocks() << std::endl;

			for (int c = 0; c < 512; c++)
			{
				std::cout.precision(5);
				std::cout.width(5);
				std::cout << std::hex;
				std::cout << (c*16) << ": ";

				for (int d = 0; d < 16; d++)
				{
					std::cout.precision(2);
					std::cout.width(2);
					std::cout << (uint32_t)(buf.get()[c * 16 + d] & 0xFF) << " ";
				}

				std::cout << std::dec << std::endl;
			}

			file.close();

		

			dr::app().shutdown();

        return 5678;
    });

    std::cout << "vv = " << vv << std::endl;
    
    return 0;
}
