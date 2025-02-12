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
// class httpRequest;

using namespace std;
class httpRequest
{
    private:
        int client_fd;
        string _path;
        std::vector<char> writeBuffer;
        //int method;
        // class methodPost obj; hassn
        // class methodDELETE obj; hassn
        string buffer;
    public:
        std::string readBuffer;
        httpRequest(int client_fd);
        ~httpRequest();
        int readData();
        int writeData();
        int getfd() { return this->client_fd; }
        // string joinbuffer(string line);
        int defineTypeMethod(const string firstline);
        string checkHeaders(const string& str);
        void validRequestHeaders();
        void setclient(httpRequest d);
        void display()
        {
            std::cout << "Client fd: " << this->client_fd << std::endl;
            std::cout << "buffer: " << this->buffer << std::endl;
        }
};
vector<string> splitstring(const string &str);