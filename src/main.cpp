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


    int listenerSocket = socket(AF_INET, SOCK_STREAM, 0);
    bind(listenerSocket, (struct sockaddr*) &serverAddress, sizeof(serverAddress));
    listen(listenerSocket, 5);

    int receiverSocket = accept(listenerSocket, nullptr, nullptr);

    std::vector<char> buffer(1024);
    recv(receiverSocket, buffer.data(), buffer.size(), 0);
    std::cout << "Server received message: " << std::string(buffer.data(), buffer.size()) << std::endl;

    close(receiverSocket);
    close(listenerSocket);
    return EXIT_SUCCESS;
}


int client_main()
{

    sockaddr_in serverAddress;
    serverAddress.sin_family = AF_INET;
    serverAddress.sin_port = htons(65535);
    serverAddress.sin_addr.s_addr = INADDR_ANY;


    int sendSocket = socket(AF_INET, SOCK_STREAM, 0);
    connect(sendSocket, (struct sockaddr*) &serverAddress, sizeof(serverAddress));

    std::string clientMessage = "hello from client!";
    send(sendSocket, clientMessage.c_str(), clientMessage.size(), 0);

    close(sendSocket);
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
