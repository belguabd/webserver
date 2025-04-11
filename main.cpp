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
    std::cout << "File " << filename << " has been emptied." << std::endl;
    file.close();
  } else {
    std::cerr << "Unable to open file: " << filename << std::endl;
  }
}

// void f() { system("leaks webserver"); }

int main(int arc, char **arv) {
  if (!arv[1])
    return 0;
  // atexit(f);
  signal(SIGPIPE, SIG_IGN);
  string str = arv[1];
  WebServer server(str);
  // makeFileEmpty(CURREQ);
  while (true)
    server.run();
}
