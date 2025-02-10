#include "ClientConnection.hpp"
#include <fstream>

ClientConnection::ClientConnection(int client_fd) : client_fd(client_fd) {
  int flags = fcntl(client_fd, F_GETFL, 0);
  fcntl(client_fd, F_SETFL, flags | O_NONBLOCK);
}

int ClientConnection::readData() {
  char buffer[4024];
  ssize_t bytes_received = recv(client_fd, buffer, sizeof(buffer), 0);
  if (bytes_received > 0) {
    buffer[bytes_received] = '\0';
    readBuffer.append(buffer, bytes_received);
  }
  std::cout << readBuffer << "\n";
  if (bytes_received == 0)
    std::cout << "Client disconnected" << std::endl;
  return bytes_received;
}

int ClientConnection::writeData() {
  const char *msg = "Hi I am server";
  ssize_t bytes_send = send(client_fd, msg, strlen(msg), 0);
  if (bytes_send == -1)
    std::cerr << "Error sending message to client" << std::endl;
  return bytes_send;
}