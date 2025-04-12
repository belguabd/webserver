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
using namespace std;
#define REDCOLORE "\033[31m"
#define DEFAULTUPLOAD "doc/html"

struct LocationConfig {
  string _root;
  string _index;
  bool _autoindex;
  string _allowed_methods;
  string _upload_store;
  string _cgi_extension;
  string _return;
  size_t _client_max_body_size;
};

class ServerConfig
{
    private:
        string data;
        string host;//d
        string root;//d
        size_t client_max_body_size;
    public:
        map<string,string> errorpage;
        map<string, LocationConfig> location;
        int typeUrl;
        vector<int> ports; //d
        ServerConfig(){};
        ServerConfig(string &str);
        ~ServerConfig();
        void parseServerConfig(string &strdata);
        void checkGlobalConfig(string strConfig);
        void locationData(string &strlocat);
        void locationNormal(string &location);
        void setVal(string &str,string &val);
        void setValLocation(string &str,string &val,LocationConfig &config);
        string getdata() const {return this->data;}
        string getHost(){return  host;}
        // bool getAutoindex(){return  autoindex;}
        string getRoot() const {return this->root;}
        // string getServerName(){return  server_name;}
      std::vector<int> getPorts(){return ports;}
      // map <string ,LocationConfig> &getConfigUpload() {return configUpload;}
};

void validbrackets(string &str);
vector<string> splithost(string& input, char c);
string removeComments(string &input);
bool checkCharacter(string &substr, char c);
string removeLocationBlocks(string &configData);
string trim(string &str);
void checkcontent(string substr);
void checkContentServer(string substr);
void isNumber(string& str);
void numberHost(string& str);
bool directoryExists(const std::string &path);
void isNumberValid(string& str);
vector<string> splitstring(const string &str);
void chechAllowedMethodValid(string &str);
size_t checkValidBadySise(string str);