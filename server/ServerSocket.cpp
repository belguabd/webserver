#include "ServerSocket.hpp"
#include <cstdio>
#include <fcntl.h>
#include <netdb.h>
#include <unistd.h>

#define RESET   "\033[0m"
#define BOLD    "\033[1m"
#define GREEN   "\033[1;32m"
#define CYAN    "\033[1;36m"

ServerSocket::ServerSocket(int port, ServerConfig conf) {
  int status;
  std::memset(&hints, 0, sizeof(hints));
  hints.ai_family = AF_INET;
  hints.ai_socktype = SOCK_STREAM;
  std::string port_str = std::to_string(port);
  status = getaddrinfo(conf.getHost().c_str(), port_str.c_str(), &hints, &res);
  if (status != 0) {
    throw std::runtime_error("getaddrinfo: " +
                             std::string(gai_strerror(status)));
  }
  server_fd = socket(res->ai_family, res->ai_socktype, res->ai_protocol);
  this->_port = port;
  this->_host = conf.getHost();
  if (server_fd < 0) {
    throw std::runtime_error("Socket creation failed");
  }
  int apt = 1;
  if (setsockopt(server_fd, SOL_SOCKET, SO_REUSEPORT, &apt, sizeof(apt)) < 0) {
    close(server_fd);
    throw std::runtime_error("Failed to set SO_REUSEPORT: " +
                             std::string(strerror(errno)));
  }

  std::cout << BOLD << GREEN << "◎ Server listening on " 
              << CYAN << "http://" << _host << ":" << _port 
              << RESET << std::endl;

}

int ServerSocket::getPort() const { return _port; }
std::string ServerSocket::getHost() const { return _host; }
void ServerSocket::bind_socket() {
  if (bind(server_fd, res->ai_addr, res->ai_addrlen) == -1) {
    close(server_fd);
    throw std::runtime_error("Bind failed: " + std::string(strerror(errno)));
  }
  freeaddrinfo(res);
}

void ServerSocket::start_listen() {
  if (listen(server_fd, BACKLOG) < 0) {
    close(server_fd);
    throw std::runtime_error("Listen failed: " + std::string(strerror(errno)));
  }
}

ServerSocket::ServerSocket() {}
ServerSocket::~ServerSocket() {}