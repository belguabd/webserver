#include "ServerSocket.hpp"

ServerSocket::ServerSocket(int port, ServerConfig conf) {
  int status;
  memset(&hints, 0, sizeof(hints));
  hints.ai_family = AF_INET;
  hints.ai_socktype = SOCK_STREAM;
  hints.ai_flags = AI_PASSIVE;
  std::string port_str = std::to_string(port);
  status = getaddrinfo(conf.getHost().c_str(), port_str.c_str(), &hints, &res);
  server_fd = socket(res->ai_family, res->ai_socktype, res->ai_protocol);
  if (server_fd < 0) {
    std::cerr << "Socket creation failed" << std::endl;
    std::exit(EXIT_FAILURE);
  }
  int apt = 1;
  if (setsockopt(server_fd, SOL_SOCKET, SO_REUSEPORT, &apt, sizeof(apt)) < 0) {
    std::cerr << "Failed to set SO_REUSEPORT: " << strerror(errno) << std::endl;
    return;
  }
  if (status != 0) {
    std::cerr << "getaddrinfo: " << gai_strerror(status) << std::endl;
    std::exit(EXIT_FAILURE);
  }
}

void ServerSocket::bind_socket() {
  if (bind(server_fd, res->ai_addr, res->ai_addrlen) == -1) {
    std::cerr << "Bind failed: " << strerror(errno) << std::endl;
    std::exit(EXIT_FAILURE);
  } 
}

void ServerSocket::start_listen() {
  if (listen(server_fd, BACKLOG) < 0) {
    std::cerr << "Listen failed" << std::endl;
    std::exit(EXIT_FAILURE);
  }
}

ServerSocket::~ServerSocket() {
  if (server_fd > 0)
    close(server_fd);
}