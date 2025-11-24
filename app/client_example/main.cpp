#include "client.h"
#include "spdlog/spdlog.h"


int main() {

    std::string message = "Hello from Client!";

    try {
        SocketDemo::Client client(65535, "127.0.0.1");

        client.connect();
        client.send(message.data(), message.size());
    }

    catch (const std::exception& e) {
        spdlog::error("{}\n\tReason: {}", e.what(), std::strerror(errno));
    }

    return EXIT_SUCCESS;
}
