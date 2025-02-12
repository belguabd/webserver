#include <sys/types.h>
#include <sys/event.h>
#include <sys/time.h>
#include <unistd.h>
#include <fcntl.h>
#include <stdio.h>
#include <stdlib.h>
#include <vector>
#include <map>
#define MAX_EVENTS 1024
#include "ServerSocket.hpp"
#include "../request/httpRequest.hpp"

class WebServer
{
    private:
        int kqueue_fd;
        std::vector<ServerSocket> serverSockets;
        std::map<int, httpRequest *> connected_clients;
        struct kevent *events;
        int max_events;
        void initialize_kqueue();
    public:
        WebServer();
        void addServerSocket(int port);
        void handle_new_connection(int server_fd);
        void receive_from_client(int client_fd);
        void respond_to_client(int client_fd);
        void run();
        const std::map<int, httpRequest *>& getConnectedClients() const {
        return connected_clients;
    }
};