#include "client.h"


int main() {

    std::string message = "Hello from Client!";

    try {
        SocketDemo::Client client(65535, "127.0.0.1");

        client.connect();
        client.send(message.data(), message.size());
    }

    catch (const std::exception& e) {
        std::cerr << e.what() << std::endl;
        std::cerr << "Reason: " << std::strerror(errno) << std::endl;
    }

    return EXIT_SUCCESS;
}
