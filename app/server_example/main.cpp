#include "server.h"
#include "spdlog/spdlog.h"


int main() {

    const uint16_t port = 65535;
    const std::size_t buffer_size = 1024;

    try {
        ev::default_loop loop;
        const SocketDemo::Server server(port, buffer_size, nullptr);
        loop.run();
    }

    catch (const std::exception& e) {
        spdlog::error("{}\n\tReason: {}", e.what(), std::strerror(errno));
    }

    return EXIT_SUCCESS;
}
