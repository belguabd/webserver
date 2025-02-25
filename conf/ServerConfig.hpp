#pragma once

#include <fstream>
#include <iostream>
#include <map>
#include <regex>
#include <sstream>
#include <string>
#include <unistd.h>
using namespace std;

struct LocationConfig {
  string root;
  string index;
  string allowed_methods;
  bool autoindex;
};
struct LocationUplaods {
  string root;
  string index;
  string upload_store;
  string client_max_body_size;
  string allowed_methods;
  bool autoindex;
};
struct LocationCgi {
  string root;
  string allowed_methods;
  string cgi_extension;
  string cgi_handler;
};

class ServerConfig {
private:
  string data;
  map<string, string> globalConfig;
  vector<int> ports;
  map<string, LocationConfig> configNormal;
  map<string, LocationUplaods> configUpload;
  map<string, LocationCgi> configcgi;

public:
  ServerConfig(string &str);
  ~ServerConfig();
  void dataConfigFile();
  void validbrackets();
  void parseServerConfig();
  void checkGlobalConfig(string strConfig);
  void locationData(string &strlocat);
  void locationNormal(string &location);
  void locationUpload(string &location);
  void locationCgi(string &location);
  void locationRedirection(string &location);
};

string removeComments(string &input);
bool checkCharacter(string &substr, char c);
string removeLocationBlocks(string &configData);
string trim(string &str);
void checkcontent(string substr);
