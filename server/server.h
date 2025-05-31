#pragma once
#include <iostream>
#include <netinet/in.h>

class Server
{
    public:
        Server(int port);
        void start();

    private:
        int port;
        int server_fd;
        sockaddr_in address;

        void handle_client(int client_socket, sockaddr_in client_addr);
};
