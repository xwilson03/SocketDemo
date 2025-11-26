#include <cstdint>
#include <string>


namespace SocketDemo {

class Client {

public:

    Client();
    ~Client();

    Client(const Client& other) = delete;
    Client& operator=(const Client& other) = delete;

    void connect(
        const uint16_t a_port,
        const std::string &a_server_address
    );

    void send(
        const void *data,
        const size_t length
    );

private:

    int sender_socket = -1;

    int err_status = 0;
    ssize_t bytes_sent = 0;

}; // class Client

} // namespace SocketDemo
