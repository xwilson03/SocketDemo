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

#include "server.h"

using namespace SocketDemo;


Receiver::Receiver(
    const int a_receiver_socket,
    const std::size_t a_buffer_size,
    const std::function<void(const int)> a_disconnect_callback
)
: buffer(a_buffer_size)
, receiver_socket(a_receiver_socket)
, disconnect_callback(a_disconnect_callback)
{
    receiver_watcher.set<Receiver, &Receiver::watcher_cb>(this);
    receiver_watcher.start(receiver_socket, ev::READ);
}

Receiver::~Receiver()
{
    receiver_watcher.stop();
    close(receiver_socket);
}

void Receiver::watcher_cb(
    ev::io &watcher,
    int revents
)
{
    bytes_received = recv(receiver_socket, buffer.data(), buffer.size(), 0);

    if (bytes_received == -1) throw std::runtime_error("SERVER: Failed to receive message.");

    if (bytes_received == 0) {
        std::cout << "SERVER: Client disconnected." << std::endl;
        disconnect_callback(receiver_socket);
    }

    else {
        std::cout << "SERVER: Received message: " << std::string(buffer.data(), bytes_received) << std::endl;
    }
}


Server::Server(
    const uint16_t a_port,
    const std::size_t a_buffer_size
)
: port(a_port)
, receiver_buffer_size(a_buffer_size)
{
    server_address.sin_family = AF_INET;
    server_address.sin_port = htons(port);
    server_address.sin_addr.s_addr = INADDR_ANY;

    listener_socket = socket(AF_INET, SOCK_STREAM, 0);
    if (listener_socket == -1) throw std::runtime_error("SERVER: Failed to open listener socket.");

    err_status = bind(listener_socket, (struct sockaddr*) &server_address, sizeof(server_address));
    if (err_status == -1) throw std::runtime_error("SERVER: Failed to bind listener socket.");

    err_status = ::listen(listener_socket, 5);
    if (err_status == -1) throw std::runtime_error("SERVER: Failed to set listener socket status.");

    listener_watcher.set<Server, &Server::listener_cb>(this);
    listener_watcher.start(listener_socket, ev::READ);

    collector_watcher.set<Server, &Server::collector_cb>(this);
}

Server::~Server()
{
    close(listener_socket);
}

void Server::listener_cb(
    ev::io &watcher,
    int revents
)
{
    int receiver_socket = accept(listener_socket, nullptr, nullptr);
    if (receiver_socket == -1) throw std::runtime_error("SERVER: Failed to accept connection.");

    receivers.emplace(receiver_socket, std::make_unique<Receiver>(
        receiver_socket,
        receiver_buffer_size,
        [this](const int receiver_id){
            this->queue_free(receiver_id);
        }
    ));
}

void Server::collector_cb(
    ev::timer &watcher,
    int revents
)
{
    std::cout << "Freeing " << stale_receivers.size() << " stale receivers!" << std::endl;

    std::cout << "Map size before: " << receivers.size() << std::endl;
    for (const int receiver_id : stale_receivers) {
        receivers.erase(receiver_id);
    }
    std::cout << "Map size after: "  << receivers.size() << std::endl;

    stale_receivers.clear();
    collector_watcher.stop();
}

void Server::queue_free(
    const int receiver_id
)
{
    stale_receivers.push_back(receiver_id);
    collector_watcher.start();
}
