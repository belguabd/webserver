#pragma once

#include "../conf/ServerConfig.hpp"
#include <arpa/inet.h>
#include <iostream>
#include <netdb.h>
#include <netinet/in.h>
#include <sys/socket.h>
#include <sys/types.h>
#include <unistd.h>
#define BACKLOG 9
class ServerSocket {
private:
  int server_fd;
  int port;
  socklen_t addr_size;
  struct addrinfo hints, *res;

public:
  ServerSocket(int port, ServerConfig conf);
  void bind_socket();
  void start_listen();
  int getServer_fd() const { return this->server_fd; }
  ~ServerSocket();
};