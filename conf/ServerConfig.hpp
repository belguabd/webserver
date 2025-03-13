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
  size_t client_max_body_size;
  string allowed_methods; 
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
        string index;
        bool autoindex;
        string server_name;
        size_t client_max_body_size;
        // map<string, string> globalConfig;
    public:
        map<string,string> errorpage;
        map<string, LocationConfig> configNormal;
        map<string, LocationUplaods> configUpload;
        map<string, LocationCgi> configcgi;
        map<string, string> configRedirection;
        int typeUrl;
        vector<int> ports;
        ServerConfig(){};
        ServerConfig(string &str);
        ~ServerConfig();
        void setGlobaleData(string &strConfig,string &str);
        void parseServerConfig(string &strdata);
        void checkGlobalConfig(string strConfig);
        void locationData(string &strlocat);
        void locationNormal(string &location);
        void locationUpload(string &location);
        void locationCgi(string &location);
        void locationRedirection(string &location);
        void nameBlocks(string &strdata);
        void setVal(string &str,string &val);
        string getdata() const {return this->data;}
        string getHost(){return  host;}
        string getRoot() const {return this->root;}
        string getIndex() const {return this->index;}
        string getServerName(){return  server_name;}
        map<string, LocationUplaods> &getConfigUpload(){return  configUpload;}
      std::vector<int> getPorts(){return ports;}
};

void validbrackets(string &str);
string removeComments(string &input);
bool checkCharacter(string &substr, char c);
string removeLocationBlocks(string &configData);
string trim(string &str);
void checkcontent(string substr);
void isNumber(string& str);
vector<string> splitstring(const string &str);
void chechAllowedMethodValid(string &str);
size_t checkValidBadySise(string str);
void    checkContentLocationUpload(string &str);
void    checkContentLocationNormal(string &str);
void    checkContentLocationCgi(string &str);