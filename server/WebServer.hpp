#include <fcntl.h>
#include <map>
#include <stdio.h>
#include <stdlib.h>
#include <sys/_types/_pid_t.h>
#include <sys/event.h>
#include <sys/time.h>
#include <sys/types.h>
#include <unistd.h>
#include <vector>
#include "../conf/ServerConfig.hpp"
#include "../request/HttpRequest.hpp"
#include "../response/HttpResponse.hpp"
#include "ServerSocket.hpp"
#define MAX_EVENTS (1024 * 2)

class WebServer {
private:
  int kqueue_fd;
  std::string _data;

  std::vector<ServerSocket> serverSockets;
  std::vector<ServerConfig> config;
  std::map<int, ServerConfig> map_configs;
  std::vector<pid_t> pids_cgi;
  std::map<ServerSocket, std::vector<ServerConfig> > socket_configs;

  std::map<int, HttpRequest *> pipe_fds;
  std::map<int, pid_t> pid_processes;

  struct kevent events[MAX_EVENTS];
  int max_events;
  void initialize_kqueue();

public:
  int getKqueue_fd() { return kqueue_fd; }
  std::vector<std::string> fileNamesCgi;
  void closeAllSockets();
  bool is_request(int fd);
  bool checkPid(pid_t pid);
  WebServer(std::string &str);
  std::vector<HttpRequest *> connected_clients;
  std::vector<HttpResponse *> responses_clients;
  void addServerSocket(ServerConfig &conf);
  void handle_new_connection(int server_fd);
  void receive_from_client(int client_fd);
  void respond_to_client(int client_fd);
  void handleCGIRequest(int client_fd);
  void set_non_blocking(int fd);
  bool isRequest(int fd);
  bool isCGIRequest(int client_fd);
  void pipe_read(int fd);
  void keepClientConnectionOpen(HttpRequest *request, HttpResponse *response,
                                std::vector<HttpRequest *>::iterator iter_req,
                                std::vector<HttpResponse *>::iterator it);
  void terminateClientConnection(HttpRequest *request, HttpResponse *response,
                                 std::vector<HttpRequest *>::iterator iter_req,
                                 std::vector<HttpResponse *>::iterator it);
  void run_script(HttpRequest *request, std::vector<char *> args,
                  std::vector<char *> envp);
  void run();
  void dataConfigFile();
  void separateServer();
};
int checkports(std::vector <int>&portserver1,std::vector <int>&portserver2);