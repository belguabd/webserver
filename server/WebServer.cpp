
#include "WebServer.hpp"
#include <cstdio>
#include <iostream>
#include <sys/_types/_ssize_t.h>
#include <sys/event.h>
#include <utility>
#include <vector>



void WebServer::initialize_kqueue() {
  kqueue_fd = kqueue();
  if (kqueue_fd < 0) {
    std::cerr << "Error creating kqueue" << std::endl;
    std::exit(EXIT_FAILURE);
  }
  events = new struct kevent[MAX_EVENTS];
}
void WebServer::addServerSocket(int port) {
  ServerSocket *newSocket = new ServerSocket(port);
  newSocket->bind_socket();
  newSocket->start_listen();

  struct kevent monitor_socket;
  EV_SET(&monitor_socket, newSocket->getServer_fd(), EVFILT_READ,
         EV_ADD | EV_ENABLE, 0, 0, NULL);
  if (kevent(kqueue_fd, &monitor_socket, 1, NULL, 0, NULL) == -1) {
    std::cerr << "Error monitoring socket" << std::endl;
    close(newSocket->getServer_fd());
    std::exit(EXIT_FAILURE);
  }
  serverSockets.push_back(*newSocket);
}

WebServer::WebServer() : max_events(MAX_EVENTS) {
  initialize_kqueue();
  std::vector<int> ports;
  ports.push_back(8585);

  addServerSocket(ports[0]);
  //   for (size_t i = 0; i < ports.size(); ++i) {
  // addServerSocket(ports[i]);
  //   }
}

void WebServer::handle_new_connection(int server_fd) {
  int client_fd = accept(server_fd, NULL, NULL);
  if (client_fd == -1) {
    std::cerr << "Error accepting client connection: " << strerror(errno)
              << std::endl;
    return;
  }
  struct kevent changes[1];
  EV_SET(&changes[0], client_fd, EVFILT_READ, EV_ADD | EV_ENABLE, 0, 0, NULL);
  if (kevent(kqueue_fd, changes, 1, NULL, 0, NULL) == -1) {
    std::cerr << "Error monitoring client socket: " << strerror(errno)
              << std::endl;
    close(client_fd);
    return;
  }
  connected_clients.push_back(new HttpRequest(client_fd));
  // connected_clients[client_fd] = new HttpRequest(client_fd);
}

void WebServer::receive_from_client(int client_fd) {
  HttpRequest *client;

  std::vector<HttpRequest *>::iterator it;
  for (it = connected_clients.begin(); it != connected_clients.end(); ++it) {
    if ((*it)->getfd() == client_fd) {
      client = *it;
      // connected_clients.erase(it);
      break;
    }
  }
  ssize_t bytes_read = client->readData();
  // connected_clients.push_back(client);
  if (bytes_read > 0) {
    struct kevent changes[1];
    EV_SET(&changes[0], client_fd, EVFILT_WRITE, EV_ADD | EV_ENABLE, 0, 0,
           NULL);
    if (kevent(kqueue_fd, changes, 1, NULL, 0, NULL) == -1) {
      std::cerr << "Error setting write event: " << strerror(errno)
                << std::endl;
      close(client_fd);
      // connected_clients.erase(client_fd);
      return;
    }
    
  } else {
    // connected_clients.erase(client_fd);
  }
}

void WebServer::respond_to_client(int client_fd) {
  HttpRequest *client = connected_clients[client_fd];
  
  client->writeData();
  struct kevent changes[1];
  EV_SET(&changes[0], client_fd, EVFILT_WRITE, EV_DELETE, 0, 0, NULL);
  kevent(kqueue_fd, changes, 1, NULL, 0, NULL);
  close(client_fd);
}



void WebServer::run() {

  int nev = kevent(kqueue_fd, NULL, 0, events, MAX_EVENTS, NULL);
  bool is_server_socket = false;
  for (size_t i = 0; i < nev; i++) {
    int event_fd = events[i].ident;
    int filter = events[i].filter;
    for (size_t i = 0; i < serverSockets.size(); i++) {
      if (serverSockets[i].getServer_fd() == event_fd)
        is_server_socket = true;
    }
    if (is_server_socket)
      handle_new_connection(event_fd);
    else {
      if (filter == EVFILT_READ) {
        // puts("Reading from client");
        receive_from_client(event_fd);
      } else if (filter == EVFILT_WRITE) {
        puts("Responding to client");
        respond_to_client(event_fd);
      }
    }
  }
}
