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
using namespace std;
class HttpResponse {
private:
public:
  HttpRequest *request;
  std::unordered_map<std::string, std::string> parseCgiHeaders;

  ssize_t totalSent ;
   int firstTimeResponse;
  int complete;
  ifstream file;
  streampos file_offset;
  size_t file_size;
  std::map<std::string, std::string> mimeType;
  HttpResponse(HttpRequest *re);
  ~HttpResponse();
  int writeData();
  bool methodIsValid(ServerConfig &config,string method);
  int checkFileAndSendData(string &data ,ServerConfig &config,string &index);
  void getResponse();
  void postResponse();
  void deleteResponse();
  string getMimeType(string &extension);
  void cgiResponse();
  void defautlRoot(ServerConfig &config);
  void redirectionResponse(string &str,ServerConfig &config);
  void sendErrorPage(ServerConfig &config,int status);
  int checkDataResev();
  void getLocationResponse(LocationConfig &normal,string &str,ServerConfig &config);
  // void getLocationResponse(LocationUplaods &upload,string &str,ServerConfig &config);
  void fileDataSend(string &data,ServerConfig &config);
  void dirDataSend(string &data,string &root,LocationConfig &normal, ServerConfig &config);
  // void dirDataSend(string &data, string &root,LocationUplaods &upload, ServerConfig &config);
  void dirDataSend(string &data, ServerConfig &config);
  void forbidden(int client_socket,ServerConfig &config);
  std::string extractBodyFromFile(const std::string &filename);
};

int checkTypePath(string &path);
bool ExistFile(string &filePath);
string dirAutoindex(string &dirPath,string &root);
string errorPage(int statusCode);
void	status_line(int client_socket,int status);