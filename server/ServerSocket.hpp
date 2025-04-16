#pragma once

#include "../conf/ServerConfig.hpp"
#include <arpa/inet.h>
#include <iostream>
#include <netdb.h>
#include <netinet/in.h>
#include <sys/socket.h>
#include <sys/types.h>
#include <unistd.h>
#define BACKLOG 126
class ServerSocket {
private:
  int server_fd;
  int _port;
  string _host;
  socklen_t addr_size;
  struct addrinfo hints, *res;

public:
  ServerSocket();
  std::vector<ServerConfig> configs;
  ServerSocket(int port, ServerConfig conf);
  void bind_socket();
  void start_listen();
  int getPort() const;
  std::string getHost() const;
  int getServer_fd() const { return this->server_fd; }
  ~ServerSocket();
};