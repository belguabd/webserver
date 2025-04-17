#pragma once

#include <fstream>
#include <iostream>
#include <filesystem>
#include <map>
#include <regex>
#include <sstream>
#include <string>
#include <unistd.h>
#include <dirent.h>
#define REDCOLORE "\033[31m"
#define DEFAULTUPLOAD "default/upload"

struct LocationConfig {
  std::string _root;
  std::string _index;
  bool _autoindex;
  std::string _allowed_methods;
  std::string _upload_store;
  std::string _cgi_extension;
  std::string _return;
  size_t _client_max_body_size;
};

class ServerConfig
{
    private:
        std::string data;
        std::string host;
        std::string root;
        size_t client_max_body_size;
    public:
        std::map<std::string,std::string> errorpage;
        std::map<std::string, LocationConfig> location;
        int typeUrl;
        std::string serverName;
        std::vector<int> ports; //d
        ServerConfig(){};
        ServerConfig(std::string &str);
        ~ServerConfig();
        void parseServerConfig(std::string &strdata);
        void checkGlobalConfig(std::string strConfig);
        void locationData(std::string &strlocat);
        void locationNormal(std::string &location);
        void setVal(std::string &str,std::string &val);
        void setValLocation(std::string &str,std::string &val,LocationConfig &config);
        std::string getdata() const {return this->data;}
        std::string getHost(){return  host;}
        // bool getAutoindex(){return  autoindex;}
        std::string getRoot() const {return this->root;}
        void  setHost(std::string _host) { this->host = _host;}
        // string getServerName(){return  server_name;}
      std::vector<int> getPorts(){return ports;}
      // std::map <string ,LocationConfig> &getConfigUpload() {return configUpload;}
};

void validbrackets(std::string &str);
std::vector<std::string> splithost(std::string& input, char c);
std::string removeComments(std::string &input);
bool checkCharacter(std::string &substr, char c);
std::string removeLocationBlocks(std::string &configData);
std::string trim(std::string &str);
void checkcontent(std::string substr);
void checkContentServer(std::string substr);
void isNumber(std::string& str);
void numberHost(std::string& str);
bool directoryExists(const std::string &path);
void isNumberValid(std::string& str);
std::vector<std::string> splitstring(const std::string &str);
void chechAllowedMethodValid(std::string &str);
size_t checkValidBadySise(std::string str);