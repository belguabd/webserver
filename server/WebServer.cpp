
#include "WebServer.hpp"
#include <algorithm>
#include <atomic>
#include <csignal>
#include <cstddef>
#include <cstdio>
#include <cstring>
#include <iostream>
#include <iterator>
#include <memory>
#include <ostream>
#include <ratio>
#include <stdexcept>
#include <sys/_types/_pid_t.h>
#include <sys/_types/_ssize_t.h>
#include <sys/event.h>
#include <sys/signal.h>
#include <sys/wait.h>
#include <type_traits>
#include <unistd.h>
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
void WebServer::addServerSocket(ServerConfig &conf) {
  // for(size_t i =0 ; i < conf.getPorts().size() ; i++){
  //   cout << conf.getPorts()[i] << "\n";;
  // }
  std::cout << conf.getPorts()[0] << "\n";
  for (size_t i = 0; i < conf.getPorts().size(); i++) {
    ServerSocket *newSocket = new ServerSocket(conf.getPorts()[i]);
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
    serverSockets.push_back(newSocket);
    map_configs[newSocket->getServer_fd()] = conf;
  }
}

WebServer::WebServer(string &str) : max_events(MAX_EVENTS) {
  initialize_kqueue();
  ifstream file(str);
  stringstream fileContent;
  if (!file.is_open())
    cout << "error file not opened " << endl;
  fileContent << file.rdbuf();
  this->_data = fileContent.str();
  this->dataConfigFile();

  // std::vector<int> ports;
  // ports.push_back(8585);

  // for (size_t i = 0; i < config.size(); ++i) {
  // puts("OK");
  // std::cout <<  "size of ports  : " <<  config[0].ports.size() << "\n";
  // std::cout << config.size() << "\n";
  for (size_t i = 0; i < config.size(); i++) {
    addServerSocket(config[i]);
  }
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
  connected_clients.push_back(
      new HttpRequest(client_fd, map_configs[server_fd]));
  // connected_clients[client_fd] = new HttpRequest(client_fd);
}

void WebServer::receive_from_client(int client_fd) {

  HttpRequest *client = NULL;
  std::vector<HttpRequest *>::iterator it;
  for (it = connected_clients.begin(); it != connected_clients.end(); ++it) {
    if ((*it)->getfd() == client_fd) {
      client = *it;
      break;
    }
  }
  client->setRequestStatus(0);
  ssize_t bytes_read = client->readData();
  if (bytes_read == -1) {
    std::cerr << "Error receiving data from client " << client_fd << ": "
              << strerror(errno) << "\n";
    close(client_fd);
    connected_clients.erase(it);
    return;
  }
  if (bytes_read == 0) {
    std::cout << "Client disconnected" << std::endl;
    close(client_fd);
    connected_clients.erase(it);
  }
  if (client->getRequestStatus() == 1) {
    struct kevent changes[1];
    EV_SET(&changes[0], client_fd, EVFILT_WRITE, EV_ADD | EV_ENABLE, 0, 0,
           client);
    if (kevent(kqueue_fd, changes, 1, NULL, 0, NULL) == -1) {
      std::cerr << "Error setting write event: " << strerror(errno)
                << std::endl;
      close(client_fd);
      connected_clients.erase(it);
      return;
    }
  }

  // if (bytes_read == -1) {
  //   std::cerr << "Error receiving data from client " << client_fd << ": "
  //             << strerror(errno) << "\n";
  //   return;
  // }
  // if (bytes_read == 0) {
  //   if (requestStatus == 0) {
  //     requestStatus = 1;
  //   }
  // }
  // if(requestStatus == 1) {
  //   struct kevent changes[1];
  //   EV_SET(&changes[0], client_fd, EVFILT_WRITE, EV_ADD | EV_ENABLE, 0, 0,
  //          NULL);
  //   if (kevent(kqueue_fd, changes, 1, NULL, 0, NULL) == -1) {
  //     std::cerr << "Error setting write event: " << strerror(errno)
  //               << std::endl;
  //     close(client_fd);
  //   }
  //   requestStatus = 0;
  // }

  // connected_clients.push_back(client);
  // if (bytes_read > 0) {
  //   struct kevent changes[1];
  //   EV_SET(&changes[0], client_fd, EVFILT_WRITE, EV_ADD | EV_ENABLE, 0, 0,
  //          NULL);
  //   if (kevent(kqueue_fd, changes, 1, NULL, 0, NULL) == -1) {
  //     std::cerr << "Error setting write event: " << strerror(errno)
  //               << std::endl;
  //     close(client_fd);
  //     // connected_clients.erase(client_fd);
  //     return;
  //   }

  // } else {
  //   // connected_clients.erase(client_fd);
  // }
}

void WebServer::respond_to_client(int client_fd) {

  HttpRequest *client;
  std::vector<HttpRequest *>::iterator it;
  for (it = connected_clients.begin(); it != connected_clients.end(); ++it) {
    if ((*it)->getfd() == client_fd) {
      client = *it;
      // connected_clients.erase(it);
      break;
    }
  }
  // std::cout << client->getServerConf().getHost() << "\n";
  HttpResponse *responseclient = new HttpResponse(client);
  //
  responseclient->writeData();
  struct kevent changes[1];
  EV_SET(&changes[0], client_fd, EVFILT_WRITE, EV_DELETE, 0, 0, NULL);
  kevent(kqueue_fd, changes, 1, NULL, 0, NULL);
  close(client_fd);
  connected_clients.erase(it);
  delete client;
  delete responseclient;
}
/*----------------------------------------------------*/

void WebServer::separateServer() {
  string strserv;
  string str;
  strserv = this->_data;
  if (strserv.length() == 0) {
    cout << "error file config empty" << endl;
    exit(0);
  }
  size_t i = 0;
  bool found = false;
  int sig = 0;
  while (i < strserv.length()) {
    size_t pos = strserv.find("server", i);
    // cout << "i = > "<<i<<endl;
    if (sig == 1 && pos == string::npos)
      break;
    if (pos == string::npos) {
      cout << "error server not found" << endl;
      exit(0);
    }
    if (sig == 0) {
      sig = 1;
      str = strserv.substr(0, pos);
    } else {
      str = strserv.substr(i - 1, pos - i);
      // cout << str<<endl;
      // cout <<"------------------------------------------"<<endl;
      ServerConfig conf(str);
      conf.validbrackets(str);
      conf.parseServerConfig(str);
      config.push_back(conf);
    }
    found = true;
    i = pos + 1;
  }
  if (found == false)
    return;
  size_t pos = strserv.rfind("server");
  str = strserv.substr(pos);
  // cout << str<<endl;
  // cout <<"------------------------------------------"<<endl;
  ServerConfig conf(str);
  conf.validbrackets(str);
  conf.parseServerConfig(str);
  config.push_back(conf);
}

void WebServer ::dataConfigFile() {
  this->_data = removeComments(this->_data);
  this->separateServer();
  // this->validbrackets();
  // this->parseServerConfig();
}

/*----------------------------------------------------*/

void WebServer::run_script(HttpRequest *request, std::vector<char *> args,
                           std::vector<char *> envp) {
  int fd[2];
  pipe(fd);
  set_non_blocking(fd[0]);
  pid_t pid = fork();
  if (pid < 0)
    throw std::runtime_error("Fork failed");
  if (pid == 0) {
    close(fd[0]);
    dup2(fd[1], STDOUT_FILENO);
    close(fd[1]);
    execve(args[0], args.data(), envp.data());
  } else {
    close(fd[1]);
    struct kevent ev;
    EV_SET(&ev, fd[0], EVFILT_READ, EV_ADD, 0, 0, NULL);
    kevent(kqueue_fd, &ev, 1, NULL, 0, NULL);
    std::cout << "Pipe FD: " << fd[0] << ", Client FD: " << request->getfd()
              << "\n";
    pipe_fds[fd[0]] = request;
    struct kevent kev;
    EV_SET(&kev, pid, EVFILT_PROC, EV_ADD, NOTE_EXIT, 0, NULL);
    if (kevent(kqueue_fd, &kev, 1, NULL, 0, NULL) == -1) {
      perror("kevent");
    }
    int timeout_seconds = 5;
    struct kevent timer_event;
    EV_SET(&timer_event, pid, EVFILT_TIMER, EV_ADD | EV_ONESHOT, NOTE_SECONDS,
           timeout_seconds, NULL);
    kevent(kqueue_fd, &timer_event, 1, NULL, 0, NULL);
  }
}

void WebServer::set_non_blocking(int fd) {
  int flags = fcntl(fd, F_GETFL, 0);
  fcntl(fd, F_SETFL, flags | O_NONBLOCK);
}

bool WebServer::isRequest(int fd) {

  HttpRequest *client = NULL;
  std::vector<HttpRequest *>::iterator it = connected_clients.begin();
  for (; it != connected_clients.end(); it++) {
    if ((*it)->getfd() == fd)
      client = *it;
  }
  return !client ? false : true;
}
bool isCGIRequest(int client_fd) { return true; };
void WebServer::handleCGIRequest(int client_fd) {
  HttpRequest *client = NULL;
  std::vector<HttpRequest *>::iterator it = connected_clients.begin();
  for (; it != connected_clients.end(); it++) {
    if ((*it)->getfd() == client_fd)
      client = *it;
  }

  // cout << "size of map is :" << client->getQueryParams().size() << "\n";
  map<string, string> env;
  env["PATH"] = "/usr/local/sbin:/usr/local/bin:/usr/sbin:/usr/bin:/sbin:/bin";
  env["PHP_PATH"] = "/usr/bin/php"; // Path to the PHP executable
  env["SCRIPT_FILENAME"] = "/path/to/your/script.php"; // Path to the PHP script
  env["REQUEST_METHOD"] = "GET";        // HTTP request method (e.g., GET, POST)
  env["QUERY_STRING"] = "";             // Query string (if any)
  env["CONTENT_TYPE"] = "text/html";    // Content type for the response
  env["SERVER_PROTOCOL"] = "HTTP/1.1";  // Server protocol
  env["GATEWAY_INTERFACE"] = "CGI/1.1"; // Gateway interface
  env["REMOTE_ADDR"] = "127.0.0.1";     // Remote client IP address
  env["SERVER_NAME"] = "localhost";     // Server name
  env["SERVER_PORT"] = "80";            // Server port
  env["SCRIPT_NAME"] =
      "/Users/belguabd/Desktop/webserver/hello.php"; // Script name

  std::map<string, string>::iterator iter = env.begin();

  std::vector<std::string> envp_map;
  std::vector<char *> envp;
  for (; iter != env.end(); iter++)
    envp_map.push_back(iter->first + "=" + iter->second);

  for (size_t i = 0; i < envp_map.size(); i++)
    envp.push_back(&envp_map[i][0]);
  envp.push_back(NULL);

  std::vector<char *> args;
  args.push_back((char *)env["PHP_PATH"].c_str());
  args.push_back((char *)env["SCRIPT_NAME"].c_str());
  args.push_back(NULL);

  run_script(new HttpRequest(*client), args, envp);
  // execve(args[0], args.data(), envp.data());
}

void WebServer::pipe_read(int fd) {
  char buffer[50];
  ssize_t n = read(fd, buffer, sizeof(buffer) - 1);
  if (n > 0) {

    cout << "\nn--------------" << n << "\n";
    buffer[n] = '\0';
    HttpRequest *request = pipe_fds[fd];
    request->setbufferCgi(buffer);
    cout << "This =>" << request->getCGIBuffer() << "\n";
  }
  if (n == 0) {
    // pipe_fds.erase(fd);
    struct kevent ev;
    EV_SET(&ev, fd, EVFILT_READ, EV_DELETE, 0, 0, NULL);
    if (kevent(kqueue_fd, &ev, 1, NULL, 0, NULL) == -1) {
      std::cerr << "Error removing EVFILT_READ event: " << strerror(errno)
                << "\n";
    }
    struct kevent evt;
    EV_SET(&evt, fd, EVFILT_WRITE, EV_ADD, 0, 0, NULL);
    if (kevent(kqueue_fd, &evt, 1, NULL, 0, NULL) == -1) {
      std::cerr << "Error adding EVFILT_WRITE event: " << strerror(errno)
                << "\n";
    }
  }
}
void send_cgi_response() { cout << "response\n"; }
void WebServer::run() {

  int nev = kevent(kqueue_fd, NULL, 0, events, MAX_EVENTS, NULL);
  bool is_server_socket = false;

  for (size_t i = 0; i < nev; i++) {
    int event_fd = events[i].ident;
    int filter = events[i].filter;
    for (size_t i = 0; i < serverSockets.size(); i++) {
      if (serverSockets[i]->getServer_fd() == event_fd)
        is_server_socket = true;
    }

    if (events[i].filter == EVFILT_TIMER) {
      kill(events[i].ident, SIGKILL);
    }
    if (events[i].fflags & NOTE_EXIT) {
      pid_t pid = events[i].ident;
      int status;
      pid_t reaped_pid = waitpid(pid, &status, 0);
      if (reaped_pid == -1) {
        perror("waitpid");
      }
    }
    if (is_server_socket)
      handle_new_connection(event_fd);
    else {
      if (filter == EVFILT_READ) {
        if (pipe_fds.find(event_fd) != pipe_fds.end()) {
          pipe_read(event_fd);
        } else {
          receive_from_client(event_fd);
          // if (isCGIRequest(event_fd)) {
          //   handleCGIRequest(event_fd);
          // }
        }
      } else if (filter == EVFILT_WRITE) {
        if (pipe_fds.find(event_fd) != pipe_fds.end()) {
          HttpRequest *request = pipe_fds[event_fd];
          cout << "request =>" << request->getCGIBuffer() << "\n";

          // pid_t pid = pid_processes[request->getfd()];
          cout << "\npid==================:" << "\n";
          // if (kill(pid, SIGKILL) == -1) {
          //   std::cerr << "Failed to kill process with PID: " << pid << " -
          //   "
          //             << strerror(errno) << "\n";
          // }
          struct kevent evt;
          EV_SET(&evt, event_fd, EVFILT_WRITE, EV_DELETE, 0, 0, NULL);
          if (kevent(kqueue_fd, &evt, 1, NULL, 0, NULL) == -1) {
            std::cerr << "Error deleting EVFILT_WRITE event: "
                      << strerror(errno) << "\n";
          }
        } else {
          respond_to_client(event_fd);
        }
      }
    }
  }
}
