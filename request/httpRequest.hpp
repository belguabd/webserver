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
// class HttpRequest;

using namespace std;
class HttpRequest
{
    private:
        int client_fd;
        int firsttime;
        int endHeaders;
        string _path;
        std::string readBuffer;
        // Post _post;
        // Delete _delete; 
        string _buffer; 
    public:
        map<string, string> mapheaders;
        HttpRequest(int client_fd);
        ~HttpRequest();
        int readData();
        int getfd() { return this->client_fd; }
        void  joinBuffer();
        string partRquest();
        int defineTypeMethod(const string firstline);
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
