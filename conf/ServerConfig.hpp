#pragma once

#include <fstream>
#include <iostream>
#include <map>
#include <regex>
#include <sstream>
#include <string>
#include <unistd.h>
using namespace std;

// struct LocationConfig {
//   string root;
//   string index;
//   string allowed_methods;
//   bool autoindex;
// };
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
        string host;//d
        string root;//d
        string index;//d
        string _allowed_methods;
        bool autoindex;
        string server_name; //op 
        size_t client_max_body_size;
        // map<string, string> globalConfig;
    public:
        map<string,string> errorpage;
        map<string, LocationConfig> location;
        map<string, LocationUplaods> configUpload;
        map<string, LocationCgi> configcgi;
        int typeUrl;
        vector<int> ports; //d
        ServerConfig(){};
        ServerConfig(string &str);
        ~ServerConfig();
        void parseServerConfig(string &strdata);
        void checkGlobalConfig(string strConfig);
        void locationData(string &strlocat);
        void locationNormal(string &location);
        void nameBlocks(string &strdata);
        void setVal(string &str,string &val);
        void setValLocation(string &str,string &val,LocationConfig &config);
        string getdata() const {return this->data;}
        string getHost(){return  host;}
        bool getAutoindex(){return  autoindex;}
        string getRoot() const {return this->root;}
        string getIndex() const {return this->index;}
        string getServerName(){return  server_name;}
      std::vector<int> getPorts(){return ports;}
      map <string ,LocationUplaods> &getConfigUpload() {return configUpload;}
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