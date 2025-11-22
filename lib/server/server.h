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


namespace SocketDemo {

class Receiver {

public:

    Receiver(
        const int a_listener_socket,
        const std::size_t a_buffer_size
    );

    ~Receiver();

    Receiver(const Receiver& other) = delete;
    Receiver& operator=(const Receiver& other) = delete;

    void watcher_cb(
        ev::io &watcher,
        int revents
    );

private:

    int receiver_socket = -1;
    bool receiver_socket_open = false;
    ev::io receiver_watcher;

    std::vector<char> buffer;
    ssize_t bytes_received = 0;

}; // class Receiver


class Server {

public:

    Server(
        const uint16_t a_port,
        const std::size_t a_buffer_size,
        ev::loop_ref &a_loop
    );

    ~Server();

    Server(const Server& other) = delete;
    Server& operator=(const Server& other) = delete;

    void run();

    void listener_cb(
        ev::io &watcher,
        int revents
    );

private:

    void open();
    void close();

    const uint16_t port;
    sockaddr_in server_address;

    ev::loop_ref &loop;

    int listener_socket = -1;
    bool listener_open = false;
    ev::io listener_watcher;

    std::vector<std::unique_ptr<Receiver>> receivers;
    std::size_t receiver_buffer_size;

    int err_status = 0;

}; // class Server

} // namespace SocketDemo
