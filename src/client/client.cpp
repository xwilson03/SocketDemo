#include <cerrno>
#include <cstdlib>
#include <cstring>
#include <iostream>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <string>
#include <sys/socket.h>
#include <thread>
#include <unistd.h>
#include <vector>


namespace SocketDemo {

class Client {

public:

    Client(
        uint16_t a_port,
        std::string a_server_address
    ) {
        server_address.sin_family = AF_INET;
        server_address.sin_port = htons(a_port);

        try {
            err_status = inet_pton(AF_INET, a_server_address.data(), &server_address.sin_addr);
            if (err_status == -1) throw std::runtime_error("CLIENT: Invalid server address.");
        }

        catch (const std::exception& e) {
            std::cerr << e.what() << std::endl;
            std::cerr << "Reason: " << std::strerror(errno) << std::endl;
        }

        open();
    }

    ~Client() {
        close();
    }


    void connect() {
        if (!sender_open) {
            std::cerr << "CLIENT: Socket not opened." << std::endl;
            return;
        }

        try {
            err_status = ::connect(sender_socket, (struct sockaddr*) &server_address, sizeof(server_address));
            if (err_status == -1) throw std::runtime_error("CLIENT: Failed to connect to server.");
        }

        catch (const std::exception& e) {
            std::cerr << e.what() << std::endl;
            std::cerr << "Reason: " << std::strerror(errno) << std::endl;
        }
    }

    void send(
        const void *data,
        const size_t length
    ) {
        if (!sender_open) {
            std::cerr << "CLIENT: Socket not opened." << std::endl;
            return;
        }

        try {
            ssize_t total_bytes_sent = 0;
            while (total_bytes_sent < length) {
                bytes_sent = ::send(
                    sender_socket,
                    data + total_bytes_sent,
                    length - total_bytes_sent,
                    0
                );
                if (bytes_sent == -1) throw std::runtime_error("CLIENT: Failed to send message.");
                total_bytes_sent += bytes_sent;
            }
        }

        catch (const std::exception& e) {
            std::cerr << e.what() << std::endl;
            std::cerr << "Reason: " << std::strerror(errno) << std::endl;
        }
    }

private:

    void open() {

        if (sender_open) {
            std::cerr << "CLIENT: Socket already open." << std::endl;
            return;
        }

        try {
            sender_socket = socket(AF_INET, SOCK_STREAM, 0);
            if (sender_socket == -1) throw std::runtime_error("CLIENT: Failed to open socket.");
            sender_open = true;
        }

        catch (const std::exception& e) {
            std::cerr << e.what() << std::endl;
            std::cerr << "Reason: " << std::strerror(errno) << std::endl;
        }
    }

    void close() {
        if (!sender_open) {
            std::cerr << "CLIENT: No socket to close." << std::endl;
            return;
        }

        ::close(sender_socket);
    }

    sockaddr_in server_address;

    int sender_socket;

    bool sender_open = false;

    int err_status;
    ssize_t bytes_sent;

}; // class Client

} // namespace SocketDemo


int main() {

    SocketDemo::Client client(65535, "127.0.0.1");

    std::string message = "Hello from Client!";

    client.connect();
    client.send(message.data(), message.size());

    return EXIT_SUCCESS;
}
