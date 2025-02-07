#pragma once

#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <unistd.h>
#include <errno.h>
#include <string.h>
#include <iostream>

class ServerSocket
{
private:
    int server_fd;
    int port;
    struct sockaddr_in address;

public:
    ServerSocket(int port);
    void bind_socket();
    void start_listen();
    ~ServerSocket();
};