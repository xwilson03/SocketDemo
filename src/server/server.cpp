#include <cerrno>
#include <cstdlib>
#include <cstring>
#include <iostream>
#include <netinet/in.h>
#include <string>
#include <sys/socket.h>
#include <thread>
#include <unistd.h>
#include <vector>


namespace SocketDemo {

class Server {

public:

    Server(
        uint16_t a_port,
        std::size_t a_buffer_size
    ):
    port(a_port)
    {
        server_address.sin_family = AF_INET;
        server_address.sin_port = htons(port);
        server_address.sin_addr.s_addr = INADDR_ANY;

        buffer.resize(a_buffer_size);
    }

    ~Server() {}

    void open() {

        if (listener_open) {
            std::cerr << "SERVER: Listener socket already open." << std::endl;
            return;
        }

        try {
            listener_socket = socket(AF_INET, SOCK_STREAM, 0);
            if (listener_socket == -1) throw std::runtime_error("SERVER: Failed to open listener socket.");
            listener_open = true;

            err_status = bind(listener_socket, (struct sockaddr*) &server_address, sizeof(server_address));
            if (err_status == -1) throw std::runtime_error("SERVER: Failed to bind listener socket.");

            err_status = ::listen(listener_socket, 5);
            if (err_status == -1) throw std::runtime_error("SERVER: Failed to set listener socket status.");
        }

        catch (const std::exception& e) {
            std::cerr << e.what() << std::endl;
            std::cerr << "Reason: " << std::strerror(errno) << std::endl;
        }
    }

    void accept() {

        if (!listener_open) {
            std::cerr << "SERVER: No listener socket to accept connections on." << std::endl;
            return;
        }

        try {
            receiver_socket = ::accept(listener_socket, nullptr, nullptr);
            if (receiver_socket == -1) throw std::runtime_error("SERVER: Failed to accept connection.");
            receiver_open = true;
        }

        catch (const std::exception& e) {
            std::cerr << e.what() << std::endl;
            std::cerr << "Reason: " << std::strerror(errno) << std::endl;
        }
    }

    int poll() {

        if (!listener_open) {
            std::cerr << "SERVER: No listener socket to accept connections on." << std::endl;
            return -1;
        }

        if (!receiver_open) {
            std::cerr << "SERVER: No receiver socket to poll." << std::endl;
            return -1;
        }

        try {
            bytes_received = recv(receiver_socket, buffer.data(), buffer.size(), 0);

            if (bytes_received == -1) throw std::runtime_error("SERVER: Failed to receive message.");
            if (bytes_received == 0) {
                std::cout << "SERVER: Client disconnected." << std::endl;
                return 1;
            }
        }

        catch (const std::exception& e) {
            std::cerr << e.what() << std::endl;
            std::cerr << "Reason: " << std::strerror(errno) << std::endl;
            return -1;
        }

        std::cout << "SERVER: Received message: " << std::string(buffer.data(), bytes_received) << std::endl;
        return 0;
    }

    void close() {
        if (receiver_open) ::close(receiver_socket);
        if (listener_open) ::close(listener_socket);
    }

private:

    const uint16_t port;

    sockaddr_in server_address;

    int listener_socket;
    int receiver_socket;

    bool listener_open = false;
    bool receiver_open = false;

    int err_status;
    ssize_t bytes_received;

    std::vector<char> buffer;

}; // class Server

} // namespace SocketDemo


int main() {

    SocketDemo::Server server(65535, 1024);

    server.open();
    server.accept();

    int err_status;
    while(true) {
        err_status = server.poll();
        if (err_status != 0) break;
    }

    server.close();

    return err_status;
}
