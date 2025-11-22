#include <arpa/inet.h>
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

#include "client.h"

using namespace SocketDemo;


Client::Client(
    const uint16_t a_port,
    const std::string &a_server_address
)
: port(a_port)
{
    server_address.sin_family = AF_INET;
    server_address.sin_port = htons(a_port);

    err_status = inet_pton(AF_INET, a_server_address.data(), &server_address.sin_addr);
    if (err_status == -1) throw std::runtime_error("CLIENT: Invalid server address.");

    open();
}

Client::~Client()
{
    close();
}

void Client::connect()
{
    if (!sender_open) {
        errno = 0;
        throw std::runtime_error("CLIENT: Socket not opened.");
    }

    err_status = ::connect(sender_socket, (struct sockaddr*) &server_address, sizeof(server_address));
    if (err_status == -1) throw std::runtime_error("CLIENT: Failed to connect to server.");
}

void Client::send(
    const void *data,
    const size_t length
)
{
    if (!sender_open) {
        errno = 0;
        throw std::runtime_error("CLIENT: Socket not opened.");
    }

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

void Client::open()
{
    sender_socket = socket(AF_INET, SOCK_STREAM, 0);
    if (sender_socket == -1) throw std::runtime_error("CLIENT: Failed to open socket.");
    sender_open = true;
}

void Client::close()
{
    if (sender_open) ::close(sender_socket);
}
