#include "ClientConnection.hpp"
#include <fstream>

ClientConnection::ClientConnection(int client_fd) : client_fd(client_fd) {
  int flags = fcntl(client_fd, F_GETFL, 0);
  fcntl(client_fd, F_SETFL, flags | O_NONBLOCK);
}

int ClientConnection::readData() {
  char buffer[4024];
  ssize_t bytes_received = recv(client_fd, buffer, sizeof(buffer), 0);
  if (bytes_received > 0)
    readBuffer.insert(readBuffer.begin(), buffer, buffer + bytes_received);
  for (size_t i = 0; i < readBuffer.size(); i++)
    std::cout << readBuffer[i];
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