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
// class HttpRequest;

using namespace std;
class HttpRequest
{
    private:
        int client_fd;
        bool flags;
        string _path;
        std::vector<char> writeBuffer;
        std::string readBuffer;
        //int method;
        // class methodPost obj; hassn
        // class methodDELETE obj; hassn
        string buffer; 
    public:
        HttpRequest(int client_fd);
        ~HttpRequest();
        int readData();
        int writeData();
        int getfd() { return this->client_fd; }
        void  joinbuffer();
        string get_line(string line);
        int defineTypeMethod(const string firstline);
        string checkHeaders(const string& str);
        void validRequestHeaders();
        bool getflags() const {
            return this->flags;
        }
        string getbuffer() const {
            return this->buffer;
        }
        void display()
        {   
            std::cout << "Client fd: " << this->client_fd << std::endl;
            std::cout << "buffer: " << this->readBuffer << std::endl;
        }
};
vector<string> splitstring(const string &str);
