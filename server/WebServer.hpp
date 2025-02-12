#include <fcntl.h>
#include <map>
#include <stdio.h>
#include <stdlib.h>
#include <sys/event.h>
#include <sys/time.h>
#include <sys/types.h>
#include <unistd.h>
#include <vector>
#define MAX_EVENTS 1024
#include "../request/httpRequest.hpp"
#include "ServerSocket.hpp"

class WebServer {
private:
  int kqueue_fd;
  std::vector<ServerSocket> serverSockets;
  std::map<int, httpRequest *> connected_clients;
  struct kevent *events;
  int max_events;
  void initialize_kqueue();

public:
  WebServer();
  std::map<int, httpRequest *> &getClients() { return connected_clients; }
  void addServerSocket(int port);
  void handle_new_connection(int server_fd);
  void receive_from_client(int client_fd);
  void respond_to_client(int client_fd);
  void run();
};
