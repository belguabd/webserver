#include "ServerSocket.hpp"

ServerSocket::ServerSocket(int port)
{
    server_fd = socket(AF_INET, SOCK_STREAM, 0);
    if (server_fd < 0)
    {
        std::cerr << "Socket creation failed" << std::endl;
        std::exit(EXIT_FAILURE);
    }
    int apt = 1;
    if (setsockopt(server_fd, SOL_SOCKET, SO_REUSEADDR | SO_REUSEPORT, &apt, sizeof(apt)))
    {
        std::cerr << "Socket option failed" << std::endl;
        std::exit(EXIT_FAILURE);
    }
    address.sin_family = AF_INET;
    address.sin_addr.s_addr = INADDR_ANY;
    address.sin_port = htons(port);
}

void ServerSocket::bind_socket()
{
    if (bind(server_fd, (struct sockaddr *)&address, sizeof(address)) < 0)
    {
        std::cerr << "Bind failed" << std::endl;
        std::exit(EXIT_FAILURE);
    }
}

void ServerSocket::start_listen()
{
    if (listen(server_fd, BACKLOG) < 0)
    {
        std::cerr << "Listen failed" << std::endl;
        std::exit(EXIT_FAILURE);
    }
}

ServerSocket::~ServerSocket()
{
    if (server_fd > 0)
        close(server_fd);
}