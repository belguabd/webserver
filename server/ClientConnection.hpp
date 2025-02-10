#pragma once 
#include <cstring>
#include <fcntl.h>
#include <iostream>
#include <netinet/in.h>
#include <sys/socket.h>
#include <sys/types.h>
#include <unistd.h>
#include <vector>

class ClientConnection {
private:
  int client_fd;
  std::string readBuffer;
  std::vector<char> writeBuffer;

public:
  ClientConnection(int client_fd);
  int readData();
  int writeData();
  int getfd() { return this->client_fd; }
};