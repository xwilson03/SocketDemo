#include "client.h"

#include <arpa/inet.h>
#include <unistd.h>
#include <stdexcept>

using namespace SocketDemo;


Client::Client()
: sender_socket(socket(AF_INET, SOCK_STREAM, 0))
{
    if (sender_socket == -1) {
        throw std::runtime_error("Failed to open socket.");
    }
}

Client::~Client()
{
    close(sender_socket);
}

void Client::connect(
    const uint16_t a_port,
    const std::string &a_server_address
)
{
    sockaddr_in server_address {};
    server_address.sin_family = AF_INET;
    server_address.sin_port = htons(a_port);

    err_status = inet_pton(AF_INET, a_server_address.data(), &server_address.sin_addr);
    if (err_status == -1) {
        throw std::runtime_error("Invalid server address.");
    }

    err_status = ::connect(
        sender_socket,
        reinterpret_cast<struct sockaddr*>(&server_address),
        sizeof(server_address)
    );
    if (err_status == -1) {
        throw std::runtime_error("Failed to connect to server.");
    }
}

void Client::send(
    const void *data,
    const size_t length
)
{
    const auto *current_byte = static_cast<const std::byte*>(data);

    ssize_t total_bytes_sent = 0;
    while (total_bytes_sent < length) {
        bytes_sent = ::send(
            sender_socket,
            current_byte,
            length - total_bytes_sent,
            0
        );
        if (bytes_sent == -1) {
            throw std::runtime_error("Failed to send message.");
        }

        total_bytes_sent += bytes_sent;
        std::advance(current_byte, bytes_sent);
    }
}
