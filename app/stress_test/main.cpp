#include "client.h"
#include "server.h"
#include <functional>


int server_main(ev::default_loop *loop, ev::async *server_shutdown_handle) {

    try {
        SocketDemo::Server server (65535, 1024, server_shutdown_handle);
        loop->run();
    }

    catch (const std::exception& e) {
        std::cerr << e.what() << std::endl;
        std::cerr << "Reason: " << std::strerror(errno) << std::endl;
    }

    return EXIT_SUCCESS;
}


int client_main() {

    const int N = 1024;
    std::string message = "Hello from Client!";

    std::vector<std::unique_ptr<SocketDemo::Client>> clients;
    clients.reserve(N);

    try {

        for (int i = 0; i < N; i++) {
            clients.emplace_back(std::make_unique<SocketDemo::Client>(65535, "127.0.0.1"));
        }

        for (const auto &client : clients) {
            client->connect();
        }

        for (const auto &client : clients) {
            client->send(message.data(), message.size());
        }
    }

    catch (const std::exception& e) {
        std::cerr << e.what() << std::endl;
        std::cerr << "Reason: " << std::strerror(errno) << std::endl;
    }

    return EXIT_SUCCESS;
}


int main() {

    ev::default_loop loop;
    ev::async server_shutdown_handle;

    std::thread server_thread (server_main, &loop, &server_shutdown_handle);
    std::thread client_thread (client_main);

    client_thread.join();
    server_shutdown_handle.send();
    server_thread.join();

    return EXIT_SUCCESS;
}
