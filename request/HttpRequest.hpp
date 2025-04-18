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

class HttpRequest {
private:
  int client_fd;
  int firsttime;
  int endHeaders;
  int _timeout;
  int _method;
  bool isCGi;
  int checkCgi;
  int cgi_for_test;
  int status_code;

  
  std::string body_cgi;
  int cgi_fd;
  int requestStatus;
  std::vector<std::string> dataFirstLine;
  std::map<std::string, std::string> queryParam;
  std::string queryString;
  std::string readBuffer;

  ServerConfig server_config;

  std::string file;
  std::string buffer_cgi;

  // Delete _delete;
  std::string _buffer;
  void handleRequest();
  int handleDeleteRequest(std::string filePath);
  void handlePost();

public:
  ServerSocket server_socket;
  std::vector<ServerConfig> server_configs;
  std::string getBodyCgi() const { return body_cgi; }

  // Setter for body_cgi
  void setBodyCgi(const std::string &bodyCgi) { body_cgi = bodyCgi; }

  int Is_open;

  int server_fd;
  std::string typeConnection;
  int cgiExtension;
  std::string rootcgi;
  std::string pathInfo;
  bool start_cgi;
  bool is_client_disconnected;
  int getMethod() const { return this->_method; }
  int getCheckCgi() { return this->checkCgi; }
  int getCgiForTest() { return this->cgi_for_test; }
  void setCgiForTest(int cgi) { this->cgi_for_test = cgi; }

  std::string filename;

  int parseFiledLine(std::string &headers);
  std::string &parseFiledLineName(std::string &filedLine);
  void setCgi(int fd) { this->cgi_fd = fd; }
  void setCGI(bool cgi) { this->isCGi = cgi; }
  // Post _post;
  Post *_post;
  ServerConfig &getServerConf() { return this->server_config; }
  std::map<std::string, std::string> mapheaders;
  HttpRequest(int client_fd, ServerSocket server_socket);
  ~HttpRequest();
  /*    --------------*/
  int setDataCgi(std::string data, ServerConfig &config,
                 LocationConfig &structConfig);
  /*    --------------*/
  void setServerConfig(ServerConfig config) { this->server_config = config; }
  int getRequestStatus() { return this->requestStatus; }
  int readData();
  int getfd() const { return this->client_fd; }
  void joinBuffer();
  std::string partRquest();
  int validHeadres(std::map<std::string, std::string> &headrs);
  int defineTypeMethod(std::string firstline);
  void parsePartRequest(std::string str_parse);
  int getFirstTimeFlag() const { return this->firsttime; }
  int getendHeaders() const { return this->endHeaders; }
  void setFirstTimeFlag(int i) { this->firsttime = i; }
  void setRequestStatus(int i) { this->requestStatus = i; }
  const std::string &getbuffer() const { return this->_buffer; }
  std::string getreadbuffer() const { return this->readBuffer; }
  std::string getFileName();
  void checkHeaders(std::string &str);
  void checkPathIscgi(std::string &path);
  void requestLine();
  ServerConfig validServerConfig();
  const std::map<std::string, std::string> &getHeaders() const {
    return mapheaders;
  }
  std::map<std::string, std::string> &getQueryParams() { return queryParam; }
  std::string &getQueryString() { return queryString; }
  const std::vector<std::string> &getDataFirstLine() const {
    return dataFirstLine;
  }
  // Getter for _timeout
  int getTimeout() const { return _timeout; }

  // Setter for _timeout
  void setTimeout(int timeout) { _timeout = timeout; }
  ServerConfig &getServerConfig() { return server_config; }
  void setbufferCgi(char *buffer) { this->buffer_cgi.assign(buffer); }
  const std::string &getCGIBuffer() { return this->buffer_cgi; }
};
std::vector<std::string> splitstring(const std::string &str);
void checkHeaders(std::string &str, std::map<std::string, std::string> &headersMap);
LocationConfig getValueMap(std::map<std::string, LocationConfig> &configNormal,
                           std::map<std::string, LocationConfig>::const_iterator it);
// void    checkHeaders(string& str, map<string, string>& headersMap);
char characterEncodeing(std::string &tmp);
std::string encodeUrl(std::string &str);
ServerConfig validServerConfig();
int indexValidPath(std::string str);
bool fileExists(std::string &filePath);
std::string convertToUpper(std::string str);
std::string findMatchingLocation(const std::string &uri,
                            const std::map<std::string, LocationConfig> &locations);
bool pathExists(std::string &path);
ServerConfig validServerConfig(std::vector<ServerConfig *>configs, std::map<std::string, std::string> headers);
