#include <cerrno>
#include <cstdlib>
#include <cstring>
#include <ev++.h>
#include <iostream>
#include <netinet/in.h>
#include <string>
#include <sys/socket.h>
#include <thread>
#include <unistd.h>
#include <vector>


namespace SocketDemo {

class Receiver {

public:

    Receiver(
        const int a_listener_socket,
        const std::size_t a_buffer_size
    )
    : buffer(a_buffer_size)
    {
        receiver_socket = accept(a_listener_socket, nullptr, nullptr);
        if (receiver_socket == -1) throw std::runtime_error("SERVER: Failed to accept connection.");
        receiver_socket_open = true;

        receiver_watcher.set<Receiver, &Receiver::watcher_cb>(this);
        receiver_watcher.start(receiver_socket, ev::READ);
    }

    ~Receiver() {
        if (receiver_watcher.active) receiver_watcher.stop();
        if (receiver_socket_open) close(receiver_socket);
    }

    Receiver(const Receiver& other) = delete;
    Receiver& operator=(const Receiver& other) = delete;

    void watcher_cb(ev::io &watcher, int revents) {

        bytes_received = recv(receiver_socket, buffer.data(), buffer.size(), 0);

        if (bytes_received == -1) throw std::runtime_error("SERVER: Failed to receive message.");

        if (bytes_received == 0) {
            std::cout << "SERVER: Client disconnected." << std::endl;

            receiver_watcher.stop();
            close(receiver_socket);
        }

        else {
            std::cout << "SERVER: Received message: " << std::string(buffer.data(), bytes_received) << std::endl;
        }
    }

private:

    int receiver_socket = -1;
    bool receiver_socket_open = false;
    ev::io receiver_watcher;

    std::vector<char> buffer;
    ssize_t bytes_received = 0;

}; // class Receiver


class Server {

public:

    Server(
        const uint16_t a_port,
        const std::size_t a_buffer_size,
        ev::loop_ref a_loop
    )
    : port(a_port)
    , loop(a_loop)
    , receiver_buffer_size(a_buffer_size)
    {
        server_address.sin_family = AF_INET;
        server_address.sin_port = htons(port);
        server_address.sin_addr.s_addr = INADDR_ANY;

        open();
    }

    ~Server() {
        close();
    }

    Server(const Server& other) = delete;
    Server& operator=(const Server& other) = delete;

    void run() {

        listener_watcher.set<Server, &Server::listener_cb>(this);
        listener_watcher.start(listener_socket, ev::READ);
        loop.run();
    }

    void listener_cb(ev::io &watcher, int revents) {

        if (!listener_open) {
            errno = 0;
            throw std::runtime_error("SERVER: No listener socket to accept connections on.");
        }

        receivers.emplace_back(
            std::make_unique<Receiver>(listener_socket, receiver_buffer_size)
        );
    }

private:

    void open() {

        listener_socket = socket(AF_INET, SOCK_STREAM, 0);
        if (listener_socket == -1) throw std::runtime_error("SERVER: Failed to open listener socket.");
        listener_open = true;

        err_status = bind(listener_socket, (struct sockaddr*) &server_address, sizeof(server_address));
        if (err_status == -1) throw std::runtime_error("SERVER: Failed to bind listener socket.");

        err_status = ::listen(listener_socket, 5);
        if (err_status == -1) throw std::runtime_error("SERVER: Failed to set listener socket status.");
    }

    void close() {

        if (listener_open) ::close(listener_socket);
    }

    const uint16_t port;
    sockaddr_in server_address;

    ev::loop_ref &loop;

    int listener_socket = -1;
    bool listener_open = false;
    ev::io listener_watcher;

    std::vector<std::unique_ptr<Receiver>> receivers;
    std::size_t receiver_buffer_size;

    int err_status = 0;

}; // class Server

} // namespace SocketDemo


int main() {

    try {
        ev::default_loop loop;
        SocketDemo::Server server(65535, 1024, loop);
        server.run();
    }

    catch (const std::exception& e) {
        std::cerr << e.what() << std::endl;
        std::cerr << "Reason: " << std::strerror(errno) << std::endl;
    }

    return EXIT_SUCCESS;
}
