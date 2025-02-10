#include <cstddef>
#include <cstdio>
#include <cstdlib>
#include <cstring>
#include <fstream>
#include <ios>
#include <iostream>
#include <sstream>
#include <string>
#include <sys/socket.h>
#include <unistd.h>

bool insideServer = false;
bool insideLocation = false;
void verifyBrackets(const std::string &serverConfig) {
  std::stack<char> brackets;

  int i = 0;
  while (i < serverConfig.size()) {
    if (serverConfig[i] == '{') {
      brackets.push('{');
    } else if (serverConfig[i] == '}') {

      if (brackets.empty()) {
        std::cerr << "Config error: Unmatched opening bracket '{' found.\n";
        std::exit(EXIT_FAILURE);
      }
      brackets.pop();
    }
    i++;
  }
  if (!brackets.empty()) {
    std::cerr << "Config error: Unmatched opening bracket '{' found.\n";
    std::exit(EXIT_FAILURE);
  }
}
std::string trim(std::string &str) {
  size_t start;
  size_t end;

  if (str.empty())
    return "";

  if ((start = str.find_first_not_of(" \n\t")) == std::string::npos)
    return "";
  end = str.find_last_not_of(" \n\t");
  return str.substr(start, end - start + 1);
}

void parseServerDirectives(std::string &buffer) {
  if (!insideServer)
    std::cerr << "Error: listen directive not inside server block\n";
  std::istringstream stream(buffer.c_str());
  std::string key, value;
  stream >> key >> value;
  std::cout << "key: " << key << " | value: " << value << "\n";
}

int main() {
  std::ifstream file("nginx.conf", std::ios::in | std::ios::out);
  std::ifstream brackets("nginx.conf", std::ios::in | std::ios::out);
  if (!brackets.is_open())
    std::cout << "Couldn't open this file\n", std::exit(EXIT_FAILURE);

  std::string buffer;
  std::string serverConfig;
  while (std::getline(brackets, buffer)) {
    serverConfig += buffer;
  }
  verifyBrackets(serverConfig);
  if (!file.is_open())
    std::cout << "Couldn't open this file\n", std::exit(EXIT_FAILURE);
  while (std::getline(file, buffer)) {

    buffer = trim(buffer);
    if (buffer.empty())
      continue;
    if (buffer == "server") {
      std::getline(file, buffer);
      buffer = trim(buffer);
      if (buffer[0] == '{' && std::strlen(buffer.c_str()) == 1)
        insideServer = true;
      else {
        std::cerr << "Error: Expected '{' after server\n";
        std::exit(EXIT_FAILURE);
      }
    } else if (!buffer.find("listen") || !buffer.find("server_name") ||
               !buffer.find("error_page") ||
               !buffer.find("client_max_body_size") || !buffer.find("root") ||
               !buffer.find("index") || !buffer.find("autoindex") ||
               !buffer.find("return")) {
      parseServerDirectives(buffer);
    } else if (!buffer.find("location")) {
      if (insideServer) {
        std::getline(file, buffer);
        buffer = trim(buffer);
        if (buffer[0] == '{' && std::strlen(buffer.c_str()) == 1)
          insideLocation = true;
        else {
          std::cerr << "Error: Expected '{' after location\n";
          std::exit(EXIT_FAILURE);
        }
        if (insideLocation) {
          while (getline(file, buffer)) {
            buffer = trim(buffer);
            if (buffer.find("autoindex") == 0 || buffer.find("path") == 0 ||
                buffer.find("allow_methods") == 0 ||
                buffer.find("return") == 0 || buffer.find("php-cgi") == 0 ||
                buffer.find("root") == 0 || buffer.find("index") == 0 ||
                buffer.find("upload_store") == 0) {
              parseServerDirectives(buffer);
            }
            if (buffer[0] == '}' && std::strlen(buffer.c_str()) == 1) {
              break;
            }
          }
          if (buffer[0] != '}' && std::strlen(buffer.c_str()) > 1) {
            std::cerr << "Error: Expected '}' after location\n";
            std::exit(EXIT_FAILURE);
          }
        }
      }
    } else if (buffer[0] == '}' && std::strlen(buffer.c_str()) == 1) {
      continue;
    } else {
      std::cerr << "Error: Unexpected directive\n";
      std::exit(EXIT_FAILURE);
    }
  }
}
