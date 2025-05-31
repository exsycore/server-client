#include <iostream>
#include <string>
#include <unistd.h>
#include <arpa/inet.h>
#include <cstring>
#include <csignal>
#include <thread>
#include <chrono>

int connect_to_server(const std::string& ip, int port)
{
    int sock = socket(AF_INET, SOCK_STREAM, 0);
    if (sock < 0)
    {
        perror("[Client] socket");
        return -1;
    }

    sockaddr_in serv_addr{};
    serv_addr.sin_family = AF_INET;
    serv_addr.sin_port = htons(port);

    if (inet_pton(AF_INET, ip.c_str(), &serv_addr.sin_addr) <= 0)
    {
        std::cerr << "[Client] Invalid address\n";
        return -1;
    }

    if (connect(sock, (struct sockaddr *)&serv_addr, sizeof(serv_addr)) < 0)
    {
        close(sock);
        return -1;
    }

    return sock;
}

int main()
{
    std::string server_ip;
    int server_port;
    std::cout << "Enter server IP: ";
    std::getline(std::cin, server_ip);
    std::cout << "Enter server port: ";
    std::cin >> server_port;
    std::cin.ignore();

    int sock = -1;

    bool condition = false;

    while (!condition)
    {
        sock = connect_to_server(server_ip, server_port);
        if (sock < 0)
        {
            std::cout << "[Client] Cannot connect. Retrying in 3 seconds...\n";
            std::this_thread::sleep_for(std::chrono::seconds(3));
            continue;
        }

        std::cout << "[Client] Connected to " << server_ip << ":" << server_port << "\n";

        fd_set readfds;
        char buffer[1024];

        while (true)
        {
            std::cout << "> ";
            std::string input;
            std::getline(std::cin, input);

            if (input == "/quit") {
                condition = true;
                break;
            }

            send(sock, input.c_str(), input.length(), 0);

            FD_ZERO(&readfds);
            FD_SET(sock, &readfds);

            timeval timeout {1, 0};
            int activity = select(sock + 1, &readfds, nullptr, nullptr, &timeout);

            if (activity > 0 && FD_ISSET(sock, &readfds))
            {
                memset(buffer, 0, sizeof(buffer));
                int bytes = read(sock, buffer, sizeof(buffer));

                if (bytes == 0)
                {
                    std::cout << "[Client] Server disconnected.\n";
                    close(sock);
                    break;
                }
                else if (bytes < 0)
                {
                    perror("[Client] Read error");
                    close(sock);
                    break;
                }

                std::cout << "[Server] " << buffer << std::endl;
            }
        }
    }
    
    if (sock >= 0)
        close(sock);

    std::cout << "[Client] Disconnected.\n";

    return 0;
}
