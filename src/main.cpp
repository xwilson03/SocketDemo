#include <cerrno>
#include <cstring>
#include <iostream>
#include <netinet/in.h>
#include <stdlib.h>
#include <string>
#include <sys/socket.h>
#include <thread>
#include <unistd.h>
#include <vector>


int server_main()
{

    sockaddr_in serverAddress;
    serverAddress.sin_family = AF_INET;
    serverAddress.sin_port = htons(65535);
    serverAddress.sin_addr.s_addr = INADDR_ANY;

    int listenerSocket;
    int receiverSocket;

    bool listenerOpen = false;
    bool receiverOpen = false;

    int errStatus;

    try {

        listenerSocket = socket(AF_INET, SOCK_STREAM, 0);
        if (listenerSocket == -1) throw std::runtime_error("SERVER: Failed to open listener socket.");
        listenerOpen = true;

        errStatus = bind(listenerSocket, (struct sockaddr*) &serverAddress, sizeof(serverAddress));
        if (errStatus == -1) throw std::runtime_error("SERVER: Failed to bind listener socket.");

        errStatus = listen(listenerSocket, 5);
        if (errStatus == -1) throw std::runtime_error("SERVER: Failed to set listener socket status.");

        receiverSocket = accept(listenerSocket, nullptr, nullptr);
        if (receiverSocket == -1) throw std::runtime_error("SERVER: Failed to accept connection.");
        receiverOpen = true;

        std::vector<char> buffer(1024);

        while (true) {

            ssize_t bytesReceived = recv(receiverSocket, buffer.data(), buffer.size(), 0);

            if (bytesReceived == -1) throw std::runtime_error("SERVER: Failed to receive message.");
            if (bytesReceived == 0) {
                std::cout << "SERVER: Client disconnected." << std::endl;
                break;
            }

            std::cout << "SERVER: Received message: " << std::string(buffer.data(), bytesReceived) << std::endl;
        }
    }

    catch (const std::exception& e) {
        std::cerr << e.what() << std::endl;
        std::cerr << "Reason: " << std::strerror(errno) << std::endl;
    }

    if (receiverOpen) close(receiverSocket);
    if (listenerOpen) close(listenerSocket);

    return EXIT_SUCCESS;
}

int client_main()
{

    sockaddr_in serverAddress;
    serverAddress.sin_family = AF_INET;
    serverAddress.sin_port = htons(65535);
    serverAddress.sin_addr.s_addr = INADDR_ANY;

    int senderSocket;

    int errStatus;

    try {

        senderSocket = socket(AF_INET, SOCK_STREAM, 0);
        if (senderSocket == -1) throw std::runtime_error("CLIENT: Failed to open socket.");

        errStatus = connect(senderSocket, (struct sockaddr*) &serverAddress, sizeof(serverAddress));
        if (errStatus == -1) throw std::runtime_error("CLIENT: Failed to connect to server.");

        std::string clientMessage = "Hello from client!";
        errStatus = send(senderSocket, clientMessage.c_str(), clientMessage.size(), 0);
        if (errStatus == -1) throw std::runtime_error("CLIENT: Failed to send message.");
    }

    catch (const std::exception& e) {
        std::cerr << e.what() << std::endl;
        std::cerr << "Reason: " << std::strerror(errno) << std::endl;
    }

    close(senderSocket);
    return EXIT_SUCCESS;
}


int main()
{
    std::thread server_thread (server_main);
    std::thread client_thread (client_main);

    client_thread.join();
    server_thread.join();

    return EXIT_SUCCESS;
}
