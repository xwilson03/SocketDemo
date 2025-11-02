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


int main() {

    sockaddr_in server_address;
    server_address.sin_family = AF_INET;
    server_address.sin_port = htons(65535);
    server_address.sin_addr.s_addr = INADDR_ANY;

    int listener_socket;
    int receiver_socket;

    bool listener_open = false;
    bool receiver_open = false;

    int err_status;

    try {

        listener_socket = socket(AF_INET, SOCK_STREAM, 0);
        if (listener_socket == -1) throw std::runtime_error("SERVER: Failed to open listener socket.");
        listener_open = true;

        err_status = bind(listener_socket, (struct sockaddr*) &server_address, sizeof(server_address));
        if (err_status == -1) throw std::runtime_error("SERVER: Failed to bind listener socket.");

        err_status = listen(listener_socket, 5);
        if (err_status == -1) throw std::runtime_error("SERVER: Failed to set listener socket status.");

        receiver_socket = accept(listener_socket, nullptr, nullptr);
        if (receiver_socket == -1) throw std::runtime_error("SERVER: Failed to accept connection.");
        receiver_open = true;

        std::vector<char> buffer(1024);

        while (true) {

            ssize_t bytes_received = recv(receiver_socket, buffer.data(), buffer.size(), 0);

            if (bytes_received == -1) throw std::runtime_error("SERVER: Failed to receive message.");
            if (bytes_received == 0) {
                std::cout << "SERVER: Client disconnected." << std::endl;
                break;
            }

            std::cout << "SERVER: Received message: " << std::string(buffer.data(), bytes_received) << std::endl;
        }
    }

    catch (const std::exception& e) {
        std::cerr << e.what() << std::endl;
        std::cerr << "Reason: " << std::strerror(errno) << std::endl;
    }

    if (receiver_open) close(receiver_socket);
    if (listener_open) close(listener_socket);

    return EXIT_SUCCESS;
}
