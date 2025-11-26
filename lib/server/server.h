#include <ev++.h>
#include <functional>
#include <cstdint>
#include <netinet/in.h>
#include <memory>


namespace SocketDemo {

class Receiver {

public:

    Receiver(
        const int a_receiver_socket,
        const std::size_t a_buffer_size,
        const std::function<void(const int)> a_disconnect_callback
    );

    ~Receiver();

    Receiver(const Receiver& other) = delete;
    Receiver& operator=(const Receiver& other) = delete;

    void watcher_cb(
        ev::io &watcher,
        int revents
    );

private:

    const int receiver_socket = -1;
    ev::io receiver_watcher;

    const std::function<void(const int)> disconnect_callback;

    std::vector<char> buffer;
    ssize_t bytes_received = 0;

}; // class Receiver


class Server {

public:

    Server(
        const uint16_t a_port,
        const std::size_t a_buffer_size,
        ev::async *a_shutdown_handle
    );

    ~Server();

    Server(const Server& other) = delete;
    Server& operator=(const Server& other) = delete;

    void listener_cb(
        ev::io &watcher,
        int revents
    );

    void collector_cb(
        ev::timer &watcher,
        int revents
    );

    void queue_free(
        const int receiver_id
    );

    void shutdown();

private:

    const uint16_t port;
    sockaddr_in server_address;

    int listener_socket = -1;
    ev::io listener_watcher;

    ev::timer collector_watcher;
    std::vector<int> stale_receivers;

    std::unordered_map<int, std::unique_ptr<Receiver>> receivers;
    std::size_t receiver_buffer_size;

    ev::async *shutdown_handle;

    int err_status = 0;

}; // class Server

} // namespace SocketDemo
