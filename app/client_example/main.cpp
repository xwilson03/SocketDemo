#include "client.h"
#include "spdlog/spdlog.h"


int main() {

    const uint16_t port = 65535;
    const std::string address = "127.0.0.1";
    std::string message = "Hello from Client!";

    try {
        SocketDemo::Client client;

        client.connect(port, address);
        client.send(message.data(), message.size());
    }

    catch (const std::exception& e) {
        spdlog::error("{}\n\tReason: {}", e.what(), std::strerror(errno));
    }

    return EXIT_SUCCESS;
}
