
#include "WebServer.hpp"
#include <algorithm>
#include <atomic>
#include <cctype>
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
#include <string>
#include <sys/_types/_pid_t.h>
#include <sys/_types/_ssize_t.h>
#include <sys/event.h>
#include <sys/fcntl.h>
#include <sys/signal.h>
#include <sys/wait.h>
#include <system_error>
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
  if (client->getRequestStatus()) {
    client->cgi_for_test = client->checkCgi;
    struct kevent changes[1];
    EV_SET(&changes[0], client_fd, EVFILT_READ, EV_DELETE, 0, 0, client);
    if (kevent(kqueue_fd, changes, 1, NULL, 0, NULL) == -1) {
      std::cerr << "Error deleting read event: " << strerror(errno)
                << std::endl;
      close(client_fd);
      connected_clients.erase(it);
      return;
    }
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
  HttpRequest *client = NULL;
  std::vector<HttpRequest *>::iterator it;
  for (it = connected_clients.begin(); it != connected_clients.end(); ++it) {
    if ((*it)->getfd() == client_fd) {
      client = *it;
      break;
    }
  }
  HttpResponse *responseclient = new HttpResponse(client);
  responseclient->writeData();
  struct kevent changes[1];
  EV_SET(&changes[0], client_fd, EVFILT_WRITE, EV_DELETE, 0, 0, NULL);
  kevent(kqueue_fd, changes, 1, NULL, 0, NULL);
  // unlink(client->filename.c_str());

  // puts("OK");
  close(client_fd);
  connected_clients.erase(it);
  delete client;
  delete responseclient;
}
/*----------------------------------------------------*/
int beforStart(string str) {
  if (str.empty())
    return 0;
  int i = 0;
  while (i < str.length()) {
    if (str[i] != '\t' && str[i] != ' ' && str[i] != '\n')
      return 1;
    i++;
  }
  return 0;
}
void WebServer::separateServer() {
  string strserv = this->_data;
  validbrackets(strserv);
  if (strserv.empty()) {
    cout << "error: file config is empty" << endl;
    exit(0);
  }
  size_t pos = 0;
  bool found = false;
  bool sig;
  while (!strserv.empty() &&
         (pos = strserv.find("server", pos)) != string::npos) {
    sig = false;
    if (beforStart(strserv.substr(0, pos)) == 1) {
      cout << "error : data befor server " << endl;
      exit(0);
    }
    size_t start = strserv.find("{", pos);
    if (start == string::npos) {
      cout << "error: server block missing opening '{'" << endl;
      exit(0);
    }
    size_t end = start;
    int brakets = 1;
    while (end < strserv.length() && brakets > 0) {
      end++;
      if (strserv[end] == '{')
        brakets++;
      if (strserv[end] == '}')
        brakets--;
    }
    string server = strserv.substr(pos, end - pos + 1);
    strserv = strserv.substr(end + 1);
    ServerConfig conf(server);
    conf.parseServerConfig(server);
    for (size_t i = 0; i < config.size(); i++) {
      if ((config[i].getHost() == conf.getHost()) &&
          (config[i].getServerName() == conf.getServerName())) {
        sig = true;
      }
    }
    if (!sig)
      this->config.push_back(conf);
    found = true;
    pos = 0;
  }

  if (!found) {
    cout << "error: no server blocks found" << endl;
    exit(0);
  }
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
  std::stringstream ss;
  ss << "cgi" << request->getfd();
  request->filename = ss.str();

  int fd = open(request->filename.c_str(), O_RDWR | O_CREAT, 0644);
  if (fd < 0) {
    std::cerr << "Failed to open file: " << strerror(errno) << std::endl;
    return;
  }
  request->setCgi(fd);
  pid_t pid = fork();
  if (pid < 0) {
    std::cerr << "Failed to fork process: " << strerror(errno) << std::endl;
    close(fd);
    return;
  }
  if (pid == 0) {
    alarm(10);
    dup2(fd, STDOUT_FILENO);
    close(fd);
    if (execve(args[0], &args[0], &envp[0]) == -1) {
      std::cerr << "Failed to execute script: " << strerror(errno) << std::endl;
      exit(1);
    }
  } else {
    close(fd);
    struct kevent events[2];

    // Monitor process exit (NOTE_EXIT)
    EV_SET(&events[0], pid, EVFILT_PROC, EV_ADD | EV_ENABLE, NOTE_EXIT, 0,
           NULL);

    // Add timeout event (EVFILT_TIMER)
    EV_SET(&events[1], pid, EVFILT_TIMER, EV_ADD | EV_ENABLE, 0, 10000, NULL);

    kevent(kqueue_fd, events, 2, NULL, 0, NULL); // Register both events

    cgi_requests[pid] = request;
    // int status;
    // waitpid(pid, &status, 0);

    // if (WIFSIGNALED(status) && WTERMSIG(status) == SIGALRM) {
    //   puts("OK");
    // }

    // if (WIFEXITED(status)) {
    //   std::cout << "Child process exited with status: " <<
    //   WEXITSTATUS(status)
    //             << std::endl;
    // } else {
    //   std::cerr << "Child process did not exit normally" << std::endl;
    // }
  }
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
bool WebServer::isCGIRequest(int client_fd) {
  HttpRequest *client = NULL;
  std::vector<HttpRequest *>::iterator it = connected_clients.begin();
  for (; it != connected_clients.end(); it++) {
    if ((*it)->getfd() == client_fd)
      client = *it;
  }
  // cout << client->cgi_for_test << "\n";
  if (client->cgi_for_test) {
    client->cgi_for_test = 0;
    return true;
  }
  return false;
};
void WebServer::handleCGIRequest(int client_fd) {

  HttpRequest *client = NULL;
  std::vector<HttpRequest *>::iterator it = connected_clients.begin();
  for (; it != connected_clients.end(); it++) {
    if ((*it)->getfd() == client_fd)
      client = *it;
  }

  map<string, string>::iterator iter_headers = client->mapheaders.begin();

  map<string, string> env;
  for (; iter_headers != client->mapheaders.end(); iter_headers++) {
    string key = (*iter_headers).first;
    transform(key.begin(), key.end(), key.begin(), ::toupper);
    std::replace(key.begin(), key.end(), '-', '_');
    env[key] = (*iter_headers).second;
  }

  // _method ?
  // client->_method == 1 ? env["REQUEST_METHOD"] = "GET"
  //                      : env["REQUEST_METHOD"] = "POST";
  // if (client->cgiExtension == PHP) {
  env["INTERPRETER"] = "/usr/bin/php";
  // } else {
  // env["INTERPRETER"] = "/usr/bin/python";
  // }
  // cout << client->rootcgi << "\n";
  // client->rootcgi = "/Users/belguabd/Desktop/webserver/hello.php";
  env["SCRIPT_NAME"] = "/Users/belguabd/Desktop/webserver/hello.php";
  std::map<string, string>::iterator iter = env.begin();
  std::vector<std::string> envp_map;
  std::vector<char *> envp;
  for (; iter != env.end(); iter++)
    envp_map.push_back(iter->first + "=" + iter->second);

  for (size_t i = 0; i < envp_map.size(); i++)
    envp.push_back(&envp_map[i][0]);
  envp.push_back(NULL);

  std::vector<char *> args;
  args.push_back((char *)env["INTERPRETER"].c_str());
  args.push_back((char *)env["SCRIPT_NAME"].c_str());
  args.push_back(NULL);
  run_script(client, args, envp);
}

// void WebServer::pipe_read(int fd) {
//   char buffer[1024];
//   ssize_t n = read(fd, buffer, sizeof(buffer) - 1);
//   if (n > 0) {
//     cout << "\nn--------------" << n << "\n";
//     buffer[n] = '\0';
//     HttpRequest *request = pipe_fds[fd];
//     request->setbufferCgi(buffer);
//     cout << "This =>" << request->getCGIBuffer() << "\n";
//   }
//   if (n == 0) {
//     HttpRequest *request = pipe_fds[fd];
//     request->cgi_for_test = 0;
//     struct kevent ev;
//     EV_SET(&ev, fd, EVFILT_READ, EV_DELETE, 0, 0, NULL);
//     if (kevent(kqueue_fd, &ev, 1, NULL, 0, NULL) == -1) {
//       std::cerr << "Error removing EVFILT_READ event: " << strerror(errno)
//                 << "\n";
//     }
//     struct kevent evt;
//     EV_SET(&evt, fd, EVFILT_WRITE, EV_ADD, 0, 0, NULL);
//     if (kevent(kqueue_fd, &evt, 1, NULL, 0, NULL) == -1) {
//       std::cerr << "Error adding EVFILT_WRITE event: " << strerror(errno)
//                 << "\n";
//     }
//   }
// }
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

    // if (events[i].filter == EVFILT_TIMER) {
    //   kill(events[i].ident, SIGKILL);
    // }
    // if (events[i].fflags & NOTE_EXIT) {
    //   pid_t pid = events[i].ident;
    //   int status;
    //   pid_t reaped_pid = waitpid(pid, &status, 0);
    //   if (reaped_pid == -1) {
    //     perror("waitpid");
    //   }
    // }
    if (is_server_socket)
      handle_new_connection(event_fd);
    else {
      if (events[i].filter == EVFILT_PROC && (events[i].fflags & NOTE_EXIT)) {
        pid_t pid = events[i].ident;
        std::cout << "[SERVER] CGI process " << pid << " exited." << std::endl;
        int status;
        waitpid(pid, &status, 0);
        cgi_requests.erase(pid);
        
      } else if (filter == EVFILT_TIMER) {
        pid_t pid = events[i].ident;
        std::cout << "[SERVER] CGI process " << pid << " timed out! Killing..."
                  << std::endl;
        kill(pid, SIGKILL);
        waitpid(pid, NULL, 0);

        cgi_requests.erase(pid);
      } else if (filter == EVFILT_READ) {
        receive_from_client(event_fd);
        if (isCGIRequest(event_fd)) {
          handleCGIRequest(event_fd);
        }
      } else if (filter == EVFILT_WRITE) {
        respond_to_client(event_fd);
      }
    }
  }
}
