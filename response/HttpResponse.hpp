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
#include <sys/socket.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <unistd.h>
#include <vector>
using namespace std;
class HttpResponse {
private:
public:
  HttpRequest *request;
  HttpResponse(HttpRequest *re);
  ~HttpResponse();
  int writeData();
  // void notFound(int client_socket);
  void getResponse();
  // void postResponse();
  void defautlRoot();
  void checkDataResev();
  void getLocationNormalResponse(LocationConfig &normal,string &str,ServerConfig &config);
  void fileDataSend(string &data,ServerConfig &config);
  void dirDataSend(string &data,LocationConfig &normal,ServerConfig &config);
  // void forbidden(int client_socket);
};

int checkTypePath(string &path);
bool ExistFile(string &filePath);
string dirAutoindex(string &dirPath);