#include <fcntl.h>
#include <iostream>
#include <map>
#include <ostream>
#include <stdio.h>
#include <stdlib.h>
#include <sys/event.h>
#include <sys/time.h>
#include <sys/types.h>
#include <unistd.h>
#include <utility>
#include <vector>
#define MAX_EVENTS 1024
#include "../request/httpRequest.hpp"
#include "ServerSocket.hpp"

class WebServer {
private:
  int kqueue_fd;
  std::vector<ServerSocket> serverSockets;
  std::vector<httpRequest *> connected_clients;
  struct kevent *events;
  int max_events;
  void initialize_kqueue();

public:
  WebServer();

  void addServerSocket(int port);
  void handle_new_connection(int server_fd);
  void receive_from_client(int client_fd);
  void respond_to_client(int client_fd);

  void displayAllClients() {
    for (httpRequest *client : connected_clients) {
        client->display();
    }
  }
  void run();
};
