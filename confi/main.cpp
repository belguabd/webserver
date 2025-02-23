#include <fstream>
#include <iostream>
#include <sstream>
#include <string>
#include <regex>
#include <unistd.h>
#include <map>
using namespace std;
std::string remove_hash_comments(const std::string& input) {
    std::stringstream result;
    
    for (size_t i = 0; i < input.size(); ++i) {
        if (input[i] == '#') {
            while (i < input.size() && input[i] != '\n') {
                i++;
            }
        }
        else {
            result << input[i];
        }
    }

    return result.str();
}

struct LocationConfig {
    std::string root;
    std::string index;
    std::string allowed_methods;
    bool autoindex;
};
struct LocationUplaods {
    std::string root;
    std::string index;
    std::string upload_store;
    std::string client_max_body_size;
    std::string allowed_methods;
    bool autoindex;
};
struct LocationCgi {
    std::string root;
    std::string allowed_methods;
    std::string cgi_extension;
    std::string cgi_handler;
};
std::string trim(const std::string& str) {
    size_t start = 0;
    size_t end = str.length();
    while (start < end && (str[start] == ' ' || str[start] == '\n' || str[start] == '\t')) {
        ++start;
    }
    while (end > start && (str[end - 1] == ' ' || str[end - 1] == '\n' || str[end - 1] == '\t')) {
        --end;
    }
    return str.substr(start, end - start);
}
bool checkCharacter(string substr,char c) {
    int i =0 ;
    while(i < substr.length()) {
        if (substr[i]!='\t' &&  substr[i]!=' '&& substr[i]!='\n' && substr[i]!=c)
            return true;
        i++;
    }
    return false;
}
void    checkcontent(string substr)
{
    int i =0 ;
    while(i < substr.length()) {
        if (substr[i]!='\t' &&  substr[i]!=' '&& substr[i]!='\n')
            return ;
        i++;
    }
    cout<<"error no content "<<endl;
    exit(0);
}
void    validbrackets(string str) {
    int sig = 0;
    size_t firstpos =0;
    size_t lastBra = str.rfind("}");
    if (lastBra != string::npos && lastBra < str.length() - 1) {
        if (checkCharacter(str.substr(lastBra + 1),'}')) {
            cout << "error data after last bracket " << endl;
            exit(1);
        }
    }
    while (firstpos < str.length()) {
        size_t pos = str.find("{", firstpos);
        size_t pos1 = str.find("}", firstpos);
        if (pos==string::npos) {
            // cout<<"error "<<endl;
            break ;
        }
        checkcontent(str.substr(pos + 1, pos1 - pos - 1));
        if (checkCharacter(str.substr(firstpos,pos - firstpos),'}'))
            sig++;
        else{
            cout<<"error bracket whitout name"<<endl;
            exit(1);
        }
        firstpos = pos + 1;
        
    }
    // cout <<" brackets  ---> = " <<sig<<endl;
    size_t lastpos =0;
    while (lastpos < str.length()) {
        size_t pos = str.find("}", lastpos);
        if (pos==string::npos) {
            break ;
        }
        else {
            sig--;
        }
        lastpos = pos + 1;
    }
    // cout <<" brackets  ++ = " <<sig<<endl;
    if (sig!=0) {
        cout<<"error brackets"<<endl;
        exit(1);
    }
}
void  checkGlobalConfig(string strConfig) {
    map<string, string> globalConfig;
    // cout << strConfig<<endl;
    std::string list[] = {"listen","host","client_max_body_size","error_page 4","error_page 5","root","index"};
    size_t listSize = sizeof(list) / sizeof(list[0]);
    for (size_t i = 0; i < listSize; i++) {
        if (strConfig.find(list[i]) == string::npos) {
            cout << "Error in global config" << endl;
            exit(0) ;
        }
        // cout <<"-->"<<list[i]<<endl;
    }
    for (size_t i = 0; i < listSize; i++) {
        size_t pos = strConfig.find(list[i]);
        size_t endKey = strConfig.find_first_of(" \t", pos);
        string key = strConfig.substr(pos, endKey - pos);
        if (key.find(";")!=string::npos) {
            cout <<"error"<<endl;
            exit(0) ;
        }
        size_t valStart = strConfig.find_first_not_of(" \t", endKey);
        size_t lastpos = strConfig.find(';',valStart);
        if (lastpos==string::npos) {
            cout <<"error"<<endl;
            exit(0) ;
        }
        string val = strConfig.substr(valStart,lastpos - valStart);
        if (val.find("\n")!=string::npos) {
            cout <<"error"<<endl;
            exit(0) ;
        }
        globalConfig[key]=val;
        // cout << "key = "<<key<<",    val = "<<val <<endl;
    }
}

void locationUpload(const std::string &location) {
    std::map<std::string, LocationUplaods> upload;
    size_t pos = location.find("{");
    std::string key = trim(location.substr(8, pos - 8));
    LocationUplaods config;
    size_t rootPos = location.find("root");
    size_t allowedMethodsPos = location.find("allowed_methods");
    size_t uploadStorePos = location.find("upload_store");
    size_t clientMaxBodySizePos = location.find("client_max_body_size");

    if (rootPos != std::string::npos) {
        size_t valueStart = location.find(" ", rootPos + 4);
        size_t valueEnd = location.find(";", valueStart);
        config.root = trim(location.substr(valueStart + 1, valueEnd - valueStart - 1));
    }

    if (allowedMethodsPos != std::string::npos) {
        size_t valueStart = location.find(" ", allowedMethodsPos + 15);
        size_t valueEnd = location.find(";", valueStart);
        config.allowed_methods = trim(location.substr(valueStart + 1, valueEnd - valueStart - 1));
    }

    if (uploadStorePos != std::string::npos) {
        size_t valueStart = location.find(" ", uploadStorePos + 12);
        size_t valueEnd = location.find(";", valueStart);
        config.upload_store = trim(location.substr(valueStart + 1, valueEnd - valueStart - 1));
    }

    if (clientMaxBodySizePos != std::string::npos) {
        size_t valueStart = location.find(" ", clientMaxBodySizePos + 18);
        size_t valueEnd = location.find(";", valueStart);
        config.client_max_body_size = trim(location.substr(valueStart + 1, valueEnd - valueStart - 1));
    }

    upload[key] = config;
    std::cout << "Location: " << key << "\n";
    std::cout << "Root: " << config.root << "\n";
    std::cout << "Allowed Methods: " << config.allowed_methods << "\n";
    std::cout << "Upload Store: " << config.upload_store << "\n";
    std::cout << "Client Max Body Size: " << config.client_max_body_size << "\n";
    std::cout <<"----------------------------------\n";
}
void locationRedirection(const std::string &location)
{
    std::cout <<"redirection     ??" <<endl;
}
void    locationCgi(const std::string &location)
{
    std::map<std::string, LocationCgi> cgiconfig;
    size_t pos = location.find("{");
    std::string key = trim(location.substr(8, pos - 8));
    LocationCgi cgi;
    size_t rootPos = location.find("root");
    size_t allowedMethodsPos = location.find("allowed_methods");
    size_t extensionPos = location.find("cgi_extension");
    size_t cgihandlerPos = location.find("cgi_handler");
    if (rootPos != std::string::npos) {
        size_t valueStart = location.find(" ", rootPos + 4);
        size_t valueEnd = location.find(";", valueStart);
        cgi.root = trim(location.substr(valueStart + 1, valueEnd - valueStart - 1));
    }
    if (allowedMethodsPos != std::string::npos) {
        size_t valueStart = location.find(" ", allowedMethodsPos + 15);
        size_t valueEnd = location.find(";", valueStart);
        cgi.allowed_methods = trim(location.substr(valueStart + 1, valueEnd - valueStart - 1));
    }
    if (extensionPos != std::string::npos) {
        size_t valueStart = location.find(" ", extensionPos + 13);
        size_t valueEnd = location.find(";", valueStart);
        cgi.cgi_extension = trim(location.substr(valueStart + 1, valueEnd - valueStart - 1));
    }
    if (cgihandlerPos != std::string::npos) {
        size_t valueStart = location.find(" ", cgihandlerPos + 11);
        size_t valueEnd = location.find(";", valueStart);
        cgi.cgi_handler = trim(location.substr(valueStart + 1, valueEnd - valueStart - 1));
    }
    cgiconfig[key] = cgi;
    std::cout << "Location: " << key << "\n";
    std::cout << "Root: " << cgi.root << "\n";
    std::cout << "Allowed Methods: " << cgi.allowed_methods << "\n";
    std::cout << "Upload Store: " << cgi.cgi_extension << "\n";
    std::cout << "Client Max Body Size: " << cgi.cgi_handler << "\n";
    std::cout <<"----------------------------------\n";
}

void    locationNormal(const std::string &location)
{
    std::map<std::string, LocationConfig> configNormal;
    size_t pos = location.find("{");
    std::string key = trim(location.substr(8, pos - 8));
    LocationConfig config;
    size_t rootPos = location.find("root");
    size_t allowedMethodsPos = location.find("allowed_methods");
    size_t indexPos = location.find("index");
    if (rootPos != std::string::npos) {
        size_t valueStart = location.find(" ", rootPos + 4);
        size_t valueEnd = location.find(";", valueStart);
        config.root = trim(location.substr(valueStart + 1, valueEnd - valueStart - 1));
    }
    if (allowedMethodsPos != std::string::npos) {
        size_t valueStart = location.find(" ", allowedMethodsPos + 15);
        size_t valueEnd = location.find(";", valueStart);
        config.allowed_methods = trim(location.substr(valueStart + 1, valueEnd - valueStart - 1));
    }
    if (indexPos != std::string::npos) {
        size_t valueStart = location.find(" ", indexPos + 5);
        size_t valueEnd = location.find(";", valueStart);
        config.index = trim(location.substr(valueStart + 1, valueEnd - valueStart - 1));
    }
    configNormal[key] = config;
    std::cout << "Location: " << key << "\n";
    std::cout << "Root: " << config.root << "\n";
    std::cout << "Allowed Methods: " << config.allowed_methods << "\n";
    std::cout << "index : " << config.index<< "\n";
    std::cout <<"----------------------------------\n";
}




void    locationData(string strlocat) {
    size_t i =0;
    while(i<strlocat.length()) {
        size_t firstpos = strlocat.find("location",i);
        if (firstpos == string::npos) {
            return ;
        }
        size_t lastpos = strlocat.find("}",i);
        string location = strlocat.substr(firstpos,lastpos - firstpos);
        if(location.find("upload_store")!=string::npos) {
            locationUpload(location);
        } else if (location.find("returne")!=string::npos) {
            locationRedirection(location);
        } else if (location.find("cgi_handler")!=string::npos) {
            locationCgi(location);
        } else {
            locationNormal(location);
        }
        i = lastpos+1;
    }
}
std::string removeLocationBlocksFromServer(const std::string& configData) {
    std::string result = configData;
    // Regex pattern to match "location { ... }" blocks
    std::string locationRegexPattern = "\\s*location\\s+[^}]*\\{[^}]*\\}\\s*";
    std::regex locationRegex(locationRegexPattern);
    // Replace all location blocks with an empty string
    result = std::regex_replace(result, locationRegex, "");  
    return result;
}
void  parseServerConfig(string str) {
    size_t pos = str.find("server");
    if (pos== string::npos ||checkCharacter(str.substr(0,pos),'}')) {
        cout << "error" << endl;
        return;
    }
    size_t i = pos + 6;
    while(i < str.length() && str[i]!='{') {
        if (str[i]!='\t' &&  str[i]!=' '&& str[i]!='\n') {
        cout << "error" << endl;
        return;
        }
        i++;
    }
    i++;
    
    
    size_t  pos1 = str.rfind('}',i);
    string strConfig = removeLocationBlocksFromServer(str.substr(i,pos1 - i));
    checkGlobalConfig(strConfig);
    pos1 = str.find("location",i);
    if (pos1 != string::npos)
        locationData(str.substr(pos1));
}
int main () {
    ifstream file("Myconfig.conf");
    std::stringstream fileContent;
    if (!file.is_open())
        cout <<"error file"<<endl;
    fileContent << file.rdbuf();
    string str = fileContent.str();
    str = remove_hash_comments(str);
    validbrackets(str);
    parseServerConfig(str);
}