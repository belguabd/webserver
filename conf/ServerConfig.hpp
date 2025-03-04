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
class ServerConfig
{
    private:
        string data;
        string host;
        string root;
        vector<string> index;
        bool autoindex;
        string server_name;
        string errorServer;
        string errorClient;
        string client_max_body_size;
        // map<string, string> globalConfig;
        map<string, LocationConfig> configNormal;
        map<string, LocationUplaods> configUpload;
        map<string, LocationCgi> configcgi;
    public:
        vector<int> ports;
        ServerConfig(){};
        ServerConfig(string &str);
        ~ServerConfig();
        // void dataConfigFile();
        // void separateServer();
        void setGlobaleData(string &strConfig,string &str);
        void parseServerConfig(string &strdata);
        void checkGlobalConfig(string strConfig);
        void locationData(string &strlocat);
        void locationNormal(string &location);
        void locationUpload(string &location);
        void locationCgi(string &location);
        void locationRedirection(string &location);
        void nameBlocks(string &strdata);
        string getdata() const {return this->data;}
        string getHost(){return  host;}
      std::vector<int> getPorts(){return ports;}
};

void validbrackets(string &str);
string removeComments(string &input);
bool checkCharacter(string &substr, char c);
string removeLocationBlocks(string &configData);
string trim(string &str);
void checkcontent(string substr);
