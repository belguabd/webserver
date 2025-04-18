#include "./conf/ServerConfig.hpp"
#include "./request/HttpRequest.hpp"
#include "./request/Post/Post.hpp"
#include "./server/WebServer.hpp"
#include <cstdlib>
#include <signal.h>
#include <system_error>

void makeFileEmpty(const std::string filename) {
  std::ofstream file(filename, std::ios::trunc);
  if (file.is_open()) {
    std::cout<< "File " << filename << " has been emptied." << std::endl;
    file.close();
  } else {
    std::cerr << "Unable to open file: " << filename << std::endl;
  }
}

void f() { system("leaks webserver"); }

int main(int arc, char **arv) {
  (void)arc;
  if (!arv[1])
    return 0;
  // atexit(f);
  signal(SIGPIPE, SIG_IGN);
  std::string str = arv[1];
  WebServer server(str);
  // makeFileEmpty(CURREQ);
  while (1337)
    server.run();
  for (size_t i = 0; i < server.connected_clients.size(); i++) {
    struct kevent change;
    EV_SET(&change, server.connected_clients[i]->getfd(), EVFILT_READ,
           EV_DELETE, 0, 0, NULL);
    kevent(server.getKqueue_fd(), &change, 1, NULL, 0, NULL);
    EV_SET(&change, server.connected_clients[i]->getfd(), EVFILT_WRITE,
           EV_DELETE, 0, 0, NULL);
    kevent(server.getKqueue_fd(), &change, 1, NULL, 0, NULL);
    EV_SET(&change, server.connected_clients[i]->getfd(), EVFILT_TIMER,
           EV_DELETE, 0, 0, NULL);
    kevent(server.getKqueue_fd(), &change, 1, NULL, 0, NULL);
    EV_SET(&change, server.connected_clients[i]->getfd(), EVFILT_PROC,
           EV_DELETE, 0, 0, NULL);
    kevent(server.getKqueue_fd(), &change, 1, NULL, 0, NULL);
    std::vector<HttpRequest *>::iterator it =
        std::find(server.connected_clients.begin(),
                  server.connected_clients.end(), server.connected_clients[i]);
    if (it != server.connected_clients.end()) {
      close(server.connected_clients[i]->getfd());
      delete *it;
      server.connected_clients.erase(it);
    }
  }
  for (size_t i = 0; i < server.fileNamesCgi.size(); i++) {
    unlink(server.fileNamesCgi[i].c_str());
  }

  for (size_t i = 0; i < server.responses_clients.size(); i++) {
    std::vector<HttpResponse *>::iterator it =
        std::find(server.responses_clients.begin(),
                  server.responses_clients.end(), server.responses_clients[i]);
    if (it != server.responses_clients.end()) {
      delete *it;
      server.responses_clients.erase(it);
    }
  }
  server.closeAllSockets();
  close(server.getKqueue_fd());
  // puts("Server stopped");
}
