#include "client.h"
#include "server.h"
#include "spdlog/spdlog.h"


int server_main(ev::default_loop *loop, ev::async *server_shutdown_handle) {

    const uint16_t port = 65535;
    const std::size_t buffer_size = 1024;

    try {
        const SocketDemo::Server server (port, buffer_size, server_shutdown_handle);
        loop->run();
    }

    catch (const std::exception& e) {
        spdlog::error("{}\n\tReason: {}", e.what(), std::strerror(errno));
    }

    return EXIT_SUCCESS;
}


int client_main() {

    const uint16_t port = 65535;
    const std::string address = "127.0.0.1";
    std::string message = "Hello from Client!";

    const int num_clients = 1024;

    std::vector<std::unique_ptr<SocketDemo::Client>> clients;
    clients.reserve(num_clients);

    try {

        for (int i = 0; i < num_clients; i++) {
            clients.emplace_back(std::make_unique<SocketDemo::Client>());
        }

        for (const auto &client : clients) {
            client->connect(port, address);
        }

        for (const auto &client : clients) {
            client->send(message.data(), message.size());
        }
    }

    catch (const std::exception& e) {
        spdlog::error("{}\n\tReason: {}", e.what(), std::strerror(errno));
    }

    return EXIT_SUCCESS;
}


int main() {

    try {
        ev::default_loop loop;
        ev::async server_shutdown_handle;

        std::thread server_thread (server_main, &loop, &server_shutdown_handle);
        std::thread client_thread (client_main);

        client_thread.join();
        server_shutdown_handle.send();
        server_thread.join();
    }

    catch (const std::exception &e) {
        spdlog::error("{}", e.what());
    }

    return EXIT_SUCCESS;
}
