
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
  // puts("here");
  connected_clients.push_back(
      new HttpRequest(client_fd, map_configs[server_fd]));
  // connected_clients[client_fd] = new HttpRequest(client_fd);
}

void WebServer::receive_from_client(int client_fd) {
  bool already_handled = false;
  HttpRequest *client = NULL;
  std::vector<HttpRequest *>::iterator it;
  for (it = connected_clients.begin(); it != connected_clients.end(); ++it) {
    if ((*it)->getfd() == client_fd) {
      client = *it;
      break;
    }
  }
  if (!client)
    return;

  client->setRequestStatus(0);
  ssize_t bytes_read = client->readData();
  if (bytes_read == -1) {
    std::cerr << "Error receiving data from client " << client_fd << ": "
              << strerror(errno) << std::endl;
    already_handled = true;
  } else if (bytes_read == 0) {
    std::cout << "Client disconnected (client_fd: " << client_fd << ")"
              << std::endl;
    already_handled = true;
  }
  if (already_handled) {
    struct kevent changes[1];
    EV_SET(&changes[0], client_fd, EVFILT_READ, EV_DELETE, 0, 0, client);
    if (kevent(kqueue_fd, changes, 1, NULL, 0, NULL) == -1) {
      std::cerr << "Error deleting read event for client " << client_fd << ": "
                << strerror(errno) << std::endl;
    }

    close(client_fd);
    connected_clients.erase(it);
    delete client;
    return;
  }
  if (client->getRequestStatus()) {
    HttpResponse *responseclient = new HttpResponse(client);
    responses_clients.push_back(responseclient);
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
std::string readFileAndRemoveHeaders(const std::string &filename) {
  std::ifstream file(filename);
  if (!file.is_open()) {
    throw std::runtime_error("Failed to open file: " + filename);
  }

  std::stringstream buffer;
  buffer << file.rdbuf();
  file.close();

  std::string fileContent = buffer.str();
  size_t header_end = fileContent.find("\r\n\r\n");

  if (header_end != std::string::npos) {
    return fileContent.substr(header_end + 4);
  }

  return fileContent;
}
void WebServer::respond_to_client(int client_fd) {
  HttpResponse *response = NULL;
  HttpRequest *request = NULL;
  std::vector<HttpResponse *>::iterator it;
  std::vector<HttpRequest *>::iterator iter_req;

  for (it = responses_clients.begin(); it != responses_clients.end(); ++it) {
    if ((*it)->request->getfd() == client_fd) {
      response = *it;
      break;
    }
  }
  for (iter_req = connected_clients.begin();
       iter_req != connected_clients.end(); ++iter_req) {
    if ((*iter_req)->getfd() == client_fd) {
      request = *iter_req;
      break;
    }
  }
  if (request->checkCgi) {
    request->setBodyCgi(readFileAndRemoveHeaders(request->filename));
  }
  ssize_t bytes_written = response->writeData();
  // cout << request->getBodyCgi() << "\n";
  // struct kevent changes[1];
  // EV_SET(&changes[0], client_fd, EVFILT_WRITE, EV_DELETE, 0, 0, NULL);
  // kevent(kqueue_fd, changes, 1, NULL, 0, NULL);
  // close(client_fd);
  // responses_clients.erase(it);
  // connected_clients.erase(iter_req);
  // delete request;
  // delete response;
  // response = NULL;
  // request = NULL;

  // ssize_t bytes_written = response->writeData();
  if (response->complete == 1) {
    puts("complete");
    struct kevent changes[2];
    EV_SET(&changes[0], (*it)->request->getfd(), EVFILT_WRITE, EV_DELETE, 0, 0,
           NULL);
    kevent(kqueue_fd, changes, 1, NULL, 0, NULL);
    EV_SET(&changes[1], (*it)->request->getfd(), EVFILT_READ, EV_DELETE, 0, 0,
           NULL);
    kevent(kqueue_fd, changes, 1, NULL, 0, NULL);
    responses_clients.erase(it);
    connected_clients.erase(iter_req);
    delete request;
    delete response;
    response = NULL;
    request = NULL;
    // close(client_fd);
    // responses_clients.erase(it);
    // delete response;
    // connected_clients.erase(it);
    // if (to_erase != responses_clients.end()) {
    //     responses_clients.erase(to_erase);  // Erase only if found
    //   }
    // delete response;
  }
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
  int fd = open(request->filename.c_str(), O_RDWR | O_CREAT | O_TRUNC, 0600);

  if (fd < 0) {
    std::cerr << "Failed to open file: " << strerror(errno) << std::endl;
    return;
  }
  cout << request->_method << "\n";
  // int fd_body = open("")
  request->setCgi(fd);
  pid_t pid = fork();
  if (pid < 0) {
    std::cerr << "Failed to fork process: " << strerror(errno) << std::endl;
    close(fd);
    return;
  }
  if (pid == 0) {
    dup2(fd, STDOUT_FILENO);
    close(fd);
    if (request->_method == POST) {
      int fd_body = open(request->getFileName().c_str(), O_RDWR, 0644);
      if (fd_body < 0) {
        std::cerr << "Error: failed to open file: " << strerror(errno)
                  << std::endl;
        close(fd);
        exit(EXIT_FAILURE);
      }
      dup2(fd_body, STDIN_FILENO);
      close(fd_body);
    }
    if (execve(args[0], &args[0], &envp[0]) == -1) {
      perror("execve failed");
      _exit(EXIT_FAILURE); // Use _exit to avoid flushing parent buffers
    }
  } else {
    close(fd);
    struct kevent changes[2];

    // Remove write event
    EV_SET(&changes[0], request->getfd(), EVFILT_WRITE, EV_DELETE, 0, 0,
           request);
    if (kevent(kqueue_fd, changes, 1, NULL, 0, NULL) == -1) {
      std::cerr << "Error removing write event: " << strerror(errno)
                << std::endl;
      // Handle cleanup if needed (e.g., close client_fd, erase client from
      // connected_clients)
      return;
    }

    // Monitor process exit (NOTE_EXIT)
    EV_SET(&changes[1], pid, EVFILT_PROC, EV_ADD | EV_ENABLE, NOTE_EXIT, 0,
           request);

    // Add a timeout event (EVFILT_TIMER)
    EV_SET(&changes[0], pid, EVFILT_TIMER, EV_ADD | EV_ENABLE, 0, 10000 / 2,
           request);

    // Register both events with kevent
    if (kevent(kqueue_fd, changes, 2, NULL, 0, NULL) == -1) {
      std::cerr << "Error setting events in kevent: " << strerror(errno)
                << std::endl;
      return;
    }
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
  if (!client)
    return false;
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
    env["HTTP_" + key] = (*iter_headers).second;
  }
  if (client->_method == GET) {
    env["REQUEST_METHOD"] = "GET";
  } else if (client->_method == POST) {
    env["REQUEST_METHOD"] = "POST";
  }
  env["SCRIPT_NAME"] = client->rootcgi;     // Path to script
  env["SCRIPT_FILENAME"] = client->rootcgi; // Path to script
  env["PATH_INFO"] = client->pathInfo;      // Path info from URL
  env["REDIRECT_STATUS"] = "1";             // Security feature for CGI
  env["CONTENT_LENGTH"] = env["HTTP_CONTENT_LENGTH"]; // Set content length
  env["INTERPRETER"] = "./cgi/php-cgi";
  // env["QUERY_STRING"] = ;
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
  for (size_t i = 0; i < nev; i++) {
    bool is_server_socket = false;
    int event_fd = events[i].ident;
    int filter = events[i].filter;
    for (size_t i = 0; i < serverSockets.size(); i++) {
      if (serverSockets[i]->getServer_fd() == event_fd)
        is_server_socket = true;
    }
    if (is_server_socket) {
      handle_new_connection(event_fd);
    } else {
      if (events[i].filter == EVFILT_PROC && (events[i].fflags & NOTE_EXIT)) {
        pid_t pid = events[i].ident;
        HttpRequest *req = static_cast<HttpRequest *>(events[i].udata);
        struct kevent changes[2];
        EV_SET(&changes[0], req->getfd(), EVFILT_WRITE, EV_ADD | EV_ENABLE, 0,
               0, NULL);
        if (kevent(kqueue_fd, changes, 1, NULL, 0, NULL) == -1) {
          std::cerr << "Error setting write event: " << strerror(errno)
                    << std::endl;
          return;
        }
        std::cout << "[SERVER] CGI process " << pid << " exited." << std::endl;
        int status;
        waitpid(pid, &status, 0);
        EV_SET(&changes[0], pid, EVFILT_TIMER, EV_DELETE, 0, 0, NULL);
        if (kevent(kqueue_fd, changes, 1, NULL, 0, NULL) == -1) {
          std::cerr << "Error deleting timer event: " << strerror(errno)
                    << std::endl;
          return;
        }
        // req->status_code = 504;
        std::cout << "[SERVER] CGI process " << pid
                  << " timed out. Status set to 504." << std::endl;
      } else if (filter == EVFILT_TIMER) {
        HttpRequest *req = static_cast<HttpRequest *>(events[i].udata);
        req->status_code = 504;
        pid_t pid = events[i].ident;
        std::cout << "[SERVER] CGI process " << pid << " timed out! Killing..."
                  << std::endl;
        kill(pid, SIGKILL);
        int status;
        waitpid(pid, &status, 0);
      }
      else if (filter == EVFILT_READ) {
        receive_from_client(event_fd);
        if (isCGIRequest(event_fd))
          handleCGIRequest(event_fd);
      } else if (filter == EVFILT_WRITE) {
        respond_to_client(event_fd);
      }
    }
  }
}
