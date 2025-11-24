#include "server.h"
#include "spdlog/spdlog.h"


int main() {

    try {
        ev::default_loop loop;
        SocketDemo::Server server(65535, 1024, nullptr);
        loop.run();
    }

    catch (const std::exception& e) {
        spdlog::error("{}\n\tReason: {}", e.what(), std::strerror(errno));
    }

    return EXIT_SUCCESS;
}
