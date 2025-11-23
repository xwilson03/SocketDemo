#include "server.h"


int main() {

    try {
        ev::default_loop loop;
        SocketDemo::Server server(65535, 1024);
        loop.run();
    }

    catch (const std::exception& e) {
        std::cerr << e.what() << std::endl;
        std::cerr << "Reason: " << std::strerror(errno) << std::endl;
    }

    return EXIT_SUCCESS;
}
