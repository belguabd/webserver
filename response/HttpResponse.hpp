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
  void notFound(int client_socket,ServerConfig &config);
  void badRequest(int client_socket,ServerConfig &config);
  void HttpVersionNotSupported(int client_socket,ServerConfig &config);
  int checkFileAndSendData(string &data ,ServerConfig &config,string &index);
  void getResponse();
  template <typename K, typename V>
  V getValueFromMap(std::map<K, V>& map, typename std::map<K, V>::iterator it) {
      V val;
      if (it != map.end()) {
          val= it->second;
      }
      return val;
  }
  void postResponse();
  void defautlRoot(ServerConfig &config);
  int checkDataResev();
  void getLocationResponse(LocationConfig &normal,string &str,ServerConfig &config);
  void getLocationResponse(LocationUplaods &upload,string &str,ServerConfig &config);
  void fileDataSend(string &data,ServerConfig &config);
  void dirDataSend(string &data,string &root,LocationConfig &normal, ServerConfig &config);
  void dirDataSend(string &data, string &root,LocationUplaods &upload, ServerConfig &config);
  void forbidden(int client_socket,ServerConfig &config);
};

int checkTypePath(string &path);
bool ExistFile(string &filePath);
string dirAutoindex(string &dirPath,string &root);