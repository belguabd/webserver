#pragma once
#include "../conf/ServerConfig.hpp"
#include "./Post/Post.hpp"
#include <cstring>
#include <fcntl.h>
#include <filesystem>
#include <fstream>
#include <iostream>
#include <map>
#include <netinet/in.h>
#include <string>
#include <sys/socket.h>
#include <sys/types.h>
#include <unistd.h>
#include <vector>
// class HttpRequest;

enum Method { NONE = 0, GET = 1, POST = 2, DELETE = 3 };

using namespace std;
class HttpRequest {
private:
  int client_fd;
  int cgi_fd;
  int firsttime;
  int requestStatus;
  int endHeaders;
  vector<string> dataFirstLine;
  map<string, string> queryParam;
  std::string readBuffer;
  ServerConfig server_config;
  string file;
  std::string buffer_cgi;
  bool isCGi;

  // Delete _delete;
  string _buffer;
  void handleRequest();
  int handleDeleteRequest(std::string filePath);

public:


  int checkCgi;
  int cgiExtension;
  string rootcgi;
  string pathInfo;



  string filename;
  int getCgi() { return cgi_fd; }
  void setCgi(int fd) { this->cgi_fd = fd; }
  int cgi_for_test;
  const bool getCGI() { return isCGi; }
  void setCGI(bool cgi) { this->isCGi = cgi; }
  // Post _post;
  Post *_post;
  ServerConfig &getServerConf() { return this->server_config; }
  map<string, string> mapheaders;
  int _method;
  HttpRequest(int client_fd, ServerConfig &server_config);
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
  void checkPathIscgi(string &path);
  void requestLine();
  void display() {
    std::cout << "Client fd: " << this->client_fd << std::endl;
    std::cout << "buffer: " << this->readBuffer << std::endl;
  }
  const std::map<std::string, std::string> &getHeaders() const {
    return mapheaders;
  }
  std::map<std::string, std::string> &getQueryParams() { return queryParam; }
  const std::vector<std::string> &getDataFirstLine() const {
    return dataFirstLine;
  }
  ServerConfig &getServerConfig() { return server_config; }
  void setbufferCgi(char *buffer) { this->buffer_cgi.assign(buffer); }
  const std::string &getCGIBuffer() { return this->buffer_cgi; }
};
vector<string> splitstring(const string &str);
void checkHeaders(string &str, map<string, string> &headersMap);
void printNonPrintableChars(const std::string &str);
LocationCgi getValueMapcgi(map<string, LocationCgi> & configNormal,map<string, LocationCgi> ::const_iterator it);
// void    checkHeaders(string& str, map<string, string>& headersMap);
void    printNonPrintableChars(const std::string &str);
char characterEncodeing(string &tmp);
string encodeUrl(string &str);
int indexValidPath(string str);
bool fileExists(std::string &filePath);
