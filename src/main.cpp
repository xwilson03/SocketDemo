#include <iostream>
#include <netinet/in.h>
#include <stdlib.h>
#include <string>
#include <sys/socket.h>
#include <unistd.h>
#include <vector>


int main()
{
    int serverListenerSocket = socket(AF_INET, SOCK_STREAM, 0);
    int clientSendSocket = socket(AF_INET, SOCK_STREAM, 0);

    sockaddr_in serverAddress;
    serverAddress.sin_family = AF_INET;
    serverAddress.sin_port = htons(65535);
    serverAddress.sin_addr.s_addr = INADDR_ANY;

    bind(serverListenerSocket, (struct sockaddr*) &serverAddress, sizeof(serverAddress));
    listen(serverListenerSocket, 5);

    connect(clientSendSocket, (struct sockaddr*) &serverAddress, sizeof(serverAddress));
    int serverReceiverSocket = accept(serverListenerSocket, nullptr, nullptr);

    std::string clientMessage = "hello from client!";
    send(clientSendSocket, clientMessage.c_str(), clientMessage.size(), 0);

    std::vector<char> buffer(1024);
    recv(serverReceiverSocket, buffer.data(), buffer.size(), 0);
    std::cout << "Server received message: " << std::string(buffer.data(), buffer.size()) << std::endl;

    close(clientSendSocket);
    close(serverReceiverSocket);
    close(serverListenerSocket);

    return EXIT_SUCCESS;
}
