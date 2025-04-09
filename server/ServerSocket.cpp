#include "ServerSocket.hpp"
#include <cstdio>
#include <netdb.h>
#include <unistd.h>

ServerSocket::ServerSocket(int port, ServerConfig conf) {
  int status;
  memset(&hints, 0, sizeof(hints));
  hints.ai_family = AF_INET;
  hints.ai_socktype = SOCK_STREAM;
  hints.ai_flags = AI_PASSIVE;
  std::string port_str = std::to_string(port);
  status = getaddrinfo(conf.getHost().c_str(), port_str.c_str(), &hints, &res);
  if (status != 0) {
    throw std::runtime_error("getaddrinfo: " +
                             std::string(gai_strerror(status)));
  }
  server_fd = socket(res->ai_family, res->ai_socktype, res->ai_protocol);
  if (server_fd < 0) {
    throw std::runtime_error("Socket creation failed");
  }
  int apt = 1;
  if (setsockopt(server_fd, SOL_SOCKET, SO_REUSEPORT, &apt, sizeof(apt)) < 0) {
    close(server_fd);
    throw std::runtime_error("Failed to set SO_REUSEPORT: " +
                             std::string(strerror(errno)));
  }
}

void ServerSocket::bind_socket() {
  if (bind(server_fd, res->ai_addr, res->ai_addrlen) == -1) {
    close(server_fd);
    throw std::runtime_error("Bind failed: " + std::string(strerror(errno)));
  }
}

void ServerSocket::start_listen() {
  if (listen(server_fd, BACKLOG) < 0) {
    close(server_fd);
    throw std::runtime_error("Listen failed: " + std::string(strerror(errno)));
  }
}

ServerSocket::~ServerSocket() {}