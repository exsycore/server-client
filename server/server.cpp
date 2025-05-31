#include "server.h"
#include <iostream>
#include <unistd.h>
#include <cstring>
#include <arpa/inet.h>
#include <thread>

Server::Server(int port) : port(port)
{
    server_fd = socket(AF_INET, SOCK_STREAM, 0);

    if (server_fd == 0)
    {
        perror("Socket failed");
        exit(EXIT_FAILURE);
    }

    int opt = 1;
    setsockopt(server_fd, SOL_SOCKET, SO_REUSEADDR | SO_REUSEPORT, &opt, sizeof(opt));

    address.sin_family = AF_INET;
    address.sin_addr.s_addr = INADDR_ANY;
    address.sin_port = htons(port);

    if (bind(server_fd, (struct sockaddr *)&address, sizeof(address)) < 0)
    {
        perror("Bind failed");
        exit(EXIT_FAILURE);
    }
}

void Server::start()
{
    if (listen(server_fd, 10) < 0)
    {
        perror("Listen failed");
        exit(EXIT_FAILURE);
    }

    std::cout << "[Server] Listening on port " << port << "..." << std::endl;

    while (true)
    {
        sockaddr_in client_addr;
        socklen_t addrlen = sizeof(client_addr);
        int client_socket = accept(server_fd, (struct sockaddr *)&client_addr, &addrlen);
        if (client_socket < 0)
        {
            perror("Accept failed");
            continue;
        }

        std::thread(&Server::handle_client, this, client_socket, client_addr).detach();
    }
}

void Server::handle_client(int client_socket, sockaddr_in client_addr)
{
    std::string client_ip = inet_ntoa(client_addr.sin_addr);
    int client_port = ntohs(client_addr.sin_port);
    std::cout << "[Client Connected] " << client_ip << ":" << client_port << std::endl;

    char buffer[1024] = {0};

    while (true)
    {
        memset(buffer, 0, sizeof(buffer));
        int valread = read(client_socket, buffer, sizeof(buffer));
        
        if (valread <= 0)
        {
            std::cout << "[Client Disconnected] " << client_ip << ":" << client_port << std::endl;
            break;
        }

        std::cout << "[Client " << client_ip << "] " << buffer;

        std::string reply = "[Echo] ";
        reply += buffer;
        send(client_socket, reply.c_str(), reply.size(), 0);
    }

    close(client_socket);
}
