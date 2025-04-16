#pragma once
#include "../conf/ServerConfig.hpp"
#include "../server/ServerSocket.hpp"
#include "./Post/Post.hpp"
#include <algorithm>
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
  string body_cgi;
  int client_fd;
  int cgi_fd;
  int firsttime;
  int requestStatus;
  int endHeaders;
  vector<string> dataFirstLine;
  map<string, string> queryParam;
  string queryString;
  std::string readBuffer;

  ServerConfig server_config;

  string file;
  std::string buffer_cgi;
  bool isCGi;

  // Delete _delete;
  string _buffer;
  void handleRequest();
  int handleDeleteRequest(std::string filePath);
  void handlePost();

public:
  ServerSocket server_socket;
  std::vector<ServerConfig> server_configs;
  string getBodyCgi() const { return body_cgi; }

  // Setter for body_cgi
  void setBodyCgi(const string &bodyCgi) { body_cgi = bodyCgi; }

  int checkCgi;
  int Is_open;

  int server_fd;
  string typeConnection;
  int cgiExtension;
  string rootcgi;
  string pathInfo;
  bool start_cgi;
  bool is_client_disconnected;
  int status_code;

  string filename;

  int parseFiledLine(std::string &headers);
  std::string &parseFiledLineName(std::string &filedLine);
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
  HttpRequest(int client_fd, ServerSocket server_socket);
  ~HttpRequest();
  /*    --------------*/
  int setDataCgi(string data, ServerConfig &config,
                 LocationConfig &structConfig);
  /*    --------------*/
  int getRequestStatus() { return this->requestStatus; }
  int readData();
  int getfd() const { return this->client_fd; }
  void joinBuffer();
  string partRquest();
  int validHeadres(map<string, string> &headrs);
  int defineTypeMethod(string firstline);
  void parsePartRequest(string str_parse);
  int getFirstTimeFlag() const { return this->firsttime; }
  int getendHeaders() const { return this->endHeaders; }
  void setFirstTimeFlag(int i) { this->firsttime = i; }
  void setRequestStatus(int i) { this->requestStatus = i; }
  const string &getbuffer() const { return this->_buffer; }
  string getreadbuffer() const { return this->readBuffer; }
  string getFileName();
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
  string &getQueryString() { return queryString; }
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
LocationConfig getValueMap(map<string, LocationConfig> &configNormal,
                           map<string, LocationConfig>::const_iterator it);
// void    checkHeaders(string& str, map<string, string>& headersMap);
void printNonPrintableChars(const std::string &str);
char characterEncodeing(string &tmp);
string encodeUrl(string &str);
int indexValidPath(string str);
bool fileExists(std::string &filePath);
string convertToUpper(string str);
string findMatchingLocation(const string &uri,
                            const map<string, LocationConfig> &locations);
bool pathExists(string &path);