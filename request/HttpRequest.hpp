#pragma once
#include <cstring>
#include <fcntl.h>
#include <fstream>
#include <iostream>
#include <map>
#include <netinet/in.h>
#include <string>
#include <sys/socket.h>
#include <sys/types.h>
#include <unistd.h>
#include <vector>
#include "./Post/Post.hpp"
#include "../conf/ServerConfig.hpp"
// class HttpRequest;

using namespace std;
class HttpRequest {
private:
  int client_fd;
  int firsttime;
  int requestStatus;
  int endHeaders;
  vector<string> dataFirstLine;
  map<string, string> queryParam;
  std::string readBuffer;
  ServerConfig server_config;

  // Delete _delete;
  string _buffer;

public:
  Post _post;
  ServerConfig &getServerConf() {return  this->server_config;}
  map<string, string> mapheaders;
  int sig;
  int firstPartBody;
  HttpRequest(int client_fd , ServerConfig &server_config);
  ~HttpRequest();
  int getRequestStatus() { return this->requestStatus; }
  int readData();
  int getfd() const { return this->client_fd; }
  void joinBuffer();
  string partRquest();
  int defineTypeMethod(string firstline);
  void parsePartRequest(string str_parse);
  int getFirstTimeFlag() const { return this->firsttime; }
  int getendHeaders() const { return this->endHeaders; }
  void setFirstTimeFlag(int i) { this->firsttime = i; }
  void setRequestStatus(int i) { this->requestStatus = i; }
  const string &getbuffer() const { return this->_buffer; }
  string getreadbuffer() const { return this->readBuffer; }
  void checkHeaders(string &str);
  void requestLine();
  void display() {
    std::cout << "Client fd: " << this->client_fd << std::endl;
    std::cout << "buffer: " << this->readBuffer << std::endl;
  }
  const std::map<std::string, std::string> &getHeaders() const {
    return mapheaders;
  }
  const std::map<std::string, std::string> &getQueryParams() const {
    return queryParam;
  }
  const std::vector<std::string> &getDataFirstLine() const {
    return dataFirstLine;
  }
  ServerConfig &getServerConfig()  {return server_config; }
};
vector<string> splitstring(const string &str);
void    checkHeaders(string& str, map<string, string>& headersMap);
void    printNonPrintableChars(const std::string &str);
char characterEncodeing(string &tmp);
string encodeUrl(string &str);