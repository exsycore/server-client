#include "server.h"

int main()
{
    int port;
    std::cout << "Enter port number (1-65535): ";
    std::cin >> port;

    if (port <= 0 || port > 65535)
    {
        std::cerr << "Invalid port number. Please enter a port between 1 and 65535." << std::endl;
        return 1;
    }
    std::cin.ignore();

    Server server(port);
    server.start();
    
    return 0;
}
