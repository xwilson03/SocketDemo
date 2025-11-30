#include "server.h"
#include "spdlog/spdlog.h"

using namespace SocketDemo;


Receiver::Receiver(
    const int a_receiver_socket,
    const std::size_t a_buffer_size,
    const std::function<void(const int)> &a_disconnect_callback
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
    if ((revents & EV_ERROR) != 0) {
        spdlog::error("Error with ev::io in Receiver::watcher_cb.");
        watcher.stop();
        return;
    }

    bytes_received = recv(watcher.fd, buffer.data(), buffer.size(), 0);

    if (bytes_received == -1) {
        throw std::runtime_error("Failed to receive message.");
    }

    if (bytes_received == 0) {
        spdlog::info("Client disconnected.");
        disconnect_callback(receiver_socket);
    }

    else {
        spdlog::info("Received message: {}", std::string(buffer.data(), bytes_received));
    }
}


Server::Server(
    const uint16_t a_port,
    const std::size_t a_buffer_size,
    ev::async *a_shutdown_handle
)
: port(a_port)
, receiver_buffer_size(a_buffer_size)
, shutdown_handle(a_shutdown_handle)
, listener_socket(socket(AF_INET, SOCK_STREAM, 0))
, server_address()
{
    server_address.sin_family = AF_INET;
    server_address.sin_port = htons(port);
    server_address.sin_addr.s_addr = INADDR_ANY;

    if (listener_socket == -1) {
        throw std::runtime_error("Failed to open listener socket.");
    }

    err_status = bind(
        listener_socket,
        reinterpret_cast<struct sockaddr*>(&server_address),
        sizeof(server_address)
    );
    if (err_status == -1) {
        throw std::runtime_error("Failed to bind listener socket.");
    }

    const int connection_queue_size = 5;
    err_status = ::listen(listener_socket, connection_queue_size);
    if (err_status == -1) {
        throw std::runtime_error("Failed to set listener socket status.");
    }

    listener_watcher.set<Server, &Server::listener_cb>(this);
    listener_watcher.start(listener_socket, ev::READ);

    collector_watcher.set<Server, &Server::collector_cb>(this);

    if (shutdown_handle != nullptr) {
        shutdown_handle->set<Server, &Server::shutdown>(this);
        shutdown_handle->start();
    }
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
    if ((revents & EV_ERROR) != 0) {
        spdlog::error("Error with ev::io in Server::listener_cb.");
        watcher.stop();
        return;
    }

    const int receiver_socket = accept(watcher.fd, nullptr, nullptr);
    if (receiver_socket == -1) {
        throw std::runtime_error("Failed to accept connection.");
    }

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
    if ((revents & EV_ERROR) != 0) {
        spdlog::error("Error with ev::timer in Server::collector_cb.");
        watcher.stop();
        return;
    }

    spdlog::debug("Freeing {} stale receivers!", stale_receivers.size());

    spdlog::debug("Map size before: {}", receivers.size());
    for (const int receiver_id : stale_receivers) {
        receivers.erase(receiver_id);
    }
    spdlog::debug("Map size after: {}", receivers.size());

    stale_receivers.clear();
    watcher.stop();
}

void Server::queue_free(
    const int receiver_id
)
{
    stale_receivers.push_back(receiver_id);
    collector_watcher.start();
}

void Server::shutdown()
{
    shutdown_handle->stop();

    listener_watcher.stop();
    receivers.clear();
    collector_watcher.stop();
}
