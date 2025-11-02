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

    int sender_socket;

    int err_status;

    try {

        sender_socket = socket(AF_INET, SOCK_STREAM, 0);
        if (sender_socket == -1) throw std::runtime_error("CLIENT: Failed to open socket.");

        err_status = connect(sender_socket, (struct sockaddr*) &server_address, sizeof(server_address));
        if (err_status == -1) throw std::runtime_error("CLIENT: Failed to connect to server.");

        std::string client_message = "Hello from client!";
        err_status = send(sender_socket, client_message.c_str(), client_message.size(), 0);
        if (err_status == -1) throw std::runtime_error("CLIENT: Failed to send message.");
    }

    catch (const std::exception& e) {
        std::cerr << e.what() << std::endl;
        std::cerr << "Reason: " << std::strerror(errno) << std::endl;
    }

    close(sender_socket);
    return EXIT_SUCCESS;
}
