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
#define DEFAULTROOT "default/html"
#define UPLOADSUCESSE "default/html/uploadsucsse.html"
#define DELETESUCESSE "default/html/deletesucess.html"
class HttpResponse {
private:
public:
  HttpRequest *request;
  std::unordered_map<std::string, std::string> parseCgiHeaders;
  int firstTimeResponse;
  std::streampos file_offset;
  int complete;
  ssize_t totalSent;
  ssize_t bytesSend;
  std::string bodycgi;
  std::ifstream file;
  std::string strLocation;
  size_t file_size;
  std::map<std::string, std::string> mimeType;
  HttpResponse(HttpRequest *re);
  ~HttpResponse();
  int writeData();
  int checkFileAndSendData(std::string &data ,ServerConfig &config,std::string &index);
  void getResponse();
  std::string getMimeType(std::string &extension);
  void cgiResponse();
  void redirectionResponse(std::string &str,ServerConfig &config);
  void sendErrorPage(ServerConfig &config,int status);
  int checkDataResev();
  void getLocationResponse(LocationConfig &normal,std::string &str,ServerConfig &config);
  void fileDataSend(std::string &data,ServerConfig &config);
  void dirDataSend(std::string &data,std::string &root,LocationConfig &normal, ServerConfig &config);
  std::string extractBodyFromFile(const std::string &filename); 
  std::string headersSending();
};
std::string findMatchingLocation(const std::string& uri, const std::map<std::string, LocationConfig>& locations);
int checkTypePath(std::string &path);
bool ExistFile(std::string &filePath);
std::string dirAutoindex(std::string &strlocation ,std::string &dirPath,std::string &root);
std::string errorPage(int statusCode);
std::string	status_line(int status);
