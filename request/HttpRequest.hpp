#pragma once
#include <string>
#include <cstring>
#include <fcntl.h>
#include <iostream>
#include <fstream>
#include <netinet/in.h>
#include <sys/socket.h>
#include <sys/types.h>
#include <unistd.h>
#include <vector>
#include <map>
#include "./Post.hpp"
#define CURREQ "curentReq"
// class HttpRequest;

using namespace std;
class HttpRequest
{
    private:
        int client_fd;
        int firsttime;
        int endHeaders;
        bool flags;
        string _path;
        std::vector<char> writeBuffer;
        std::string readBuffer;
        //int method;
        // Delete _delete; 
        // class methodDELETE obj; hassn
        string _buffer; 
    public:
        Post _post;
        map<string, string> mapHeaders;
        HttpRequest(int client_fd);
        ~HttpRequest();
        int readData();
        int writeData();
        int getfd() { return this->client_fd; }
        void  joinBuffer();
        string partRquest();
        int defineTypeMethod(const string firstline);
        void postBodyHandler();

        void parsePartRequest(string str_parse);
        int getFirstTimeFlag() const {
            return this->firsttime;
        }
        int getendHeaders() const {
            return this->endHeaders;
        }
        void setFirstTimeFlag(int i) {
            this->firsttime = i;
        }
        bool getflags() const {
            return this->flags;
        }
        string getbuffer() const {
            return this->_buffer;
        }
        string getreadbuffer() const {
            return this->readBuffer;
        }
        void    checkHeaders(string& str);
        void display()
        {   
            std::cout << "Client fd: " << this->client_fd << std::endl;
            std::cout << "buffer: " << this->readBuffer << std::endl;
        }
};
vector<string> splitstring(const string &str);
void    checkHeaders(string& str, map<string, string>& headersMap);
