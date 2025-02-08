#pragma once

#include <arpa/inet.h>
#include <iostream>
#include <netinet/in.h>
#include <sys/socket.h>
#include <sys/types.h>
#include <unistd.h>
#define BACKLOG 9
class ServerSocket {
private:
  int server_fd;
  int port;
  struct sockaddr_in address;

public:
  ServerSocket(int port);
  void bind_socket();
  void start_listen();
  int getServer_fd() const { return this->server_fd; }
  ~ServerSocket();
};