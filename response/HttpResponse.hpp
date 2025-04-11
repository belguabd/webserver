#pragma once
#include "../request/HttpRequest.hpp"
#include <cstring>
#include <dirent.h>
#include <fcntl.h>
#include <fstream>
#include <iostream>
#include <map>
#include <netinet/in.h>
#include <sstream>
#include <string>
#include <sys/_types/_ssize_t.h>
#include <sys/socket.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <unistd.h>
#include <vector>
#include <unordered_map>
#define DEFAULTINDEX "/indexServer.html"
using namespace std;
class HttpResponse {
private:
public:
  HttpRequest *request;
  std::unordered_map<std::string, std::string> parseCgiHeaders;

  ssize_t totalSent ;
  ssize_t bytesSend;
  string bodycgi;
   int firstTimeResponse;
  int complete;
  ifstream file;
  string strLocation;
  streampos file_offset;
  size_t file_size;
  std::map<std::string, std::string> mimeType;
  HttpResponse(HttpRequest *re);
  ~HttpResponse();
  int writeData();
  bool methodIsValid(ServerConfig &config,string method);
  int checkFileAndSendData(string &data ,ServerConfig &config,string &index);
  void getResponse();
  string getMimeType(string &extension);
  void cgiResponse();
  void redirectionResponse(string &str,ServerConfig &config);
  void sendErrorPage(ServerConfig &config,int status);
  int checkDataResev();
  void getLocationResponse(LocationConfig &normal,string &str,ServerConfig &config);
  void fileDataSend(string &data,ServerConfig &config);
  void dirDataSend(string &data,string &root,LocationConfig &normal, ServerConfig &config);
  std::string extractBodyFromFile(const std::string &filename);
};
string findMatchingLocation(const string& uri, const map<string, LocationConfig>& locations);
int checkTypePath(string &path);
bool ExistFile(string &filePath);
string dirAutoindex(string &strlocation ,string &dirPath,string &root);
string errorPage(int statusCode);
string	status_line(int client_socket,int status);
string headersSending(int client_socket, string serverName);