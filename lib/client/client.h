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


namespace SocketDemo {

class Client {

public:

    Client(
        const uint16_t a_port,
        const std::string &a_server_address
    );

    ~Client();

    Client(const Client& other) = delete;
    Client& operator=(const Client& other) = delete;

    void connect();

    void send(
        const void *data,
        const size_t length
    );

private:

    const uint16_t port;
    sockaddr_in server_address;

    int sender_socket = -1;

    int err_status = 0;
    ssize_t bytes_sent = 0;

}; // class Client

} // namespace SocketDemo
