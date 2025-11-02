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

class Client {

public:

    Client() {
        server_address.sin_family = AF_INET;
        server_address.sin_port = htons(65535);
        server_address.sin_addr.s_addr = INADDR_ANY;
    }

    ~Client() {}

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

    void close() {
        if (!sender_open) {
            std::cerr << "CLIENT: No socket to close." << std::endl;
            return;
        }

        ::close(sender_socket);
    }

    void send() {
        if (!sender_open) {
            std::cerr << "CLIENT: Socket not opened." << std::endl;
            return;
        }

        try {
            std::string client_message = "Hello from client!";

            ssize_t total_bytes_sent = 0;
            while (total_bytes_sent < client_message.size()) {
                bytes_sent = ::send(
                    sender_socket,
                    client_message.c_str() + total_bytes_sent,
                    client_message.size() - total_bytes_sent,
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

    sockaddr_in server_address;

    int sender_socket;

    bool sender_open = false;

    int err_status;
    ssize_t bytes_sent;

}; // class Client

} // namespace SocketDemo


int main() {

    SocketDemo::Client client;

    client.open();
    client.connect();
    client.send();
    client.close();

    return EXIT_SUCCESS;
}
