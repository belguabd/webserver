#pragma once
#include "../server/ClientConnection.hpp"
#include <string>
// class ClientConnection;

using namespace std;
class httpRequest
{
    private:
        string _path;
        ClientConnection *client;
        // string line;
        //int method;
        // class methodPost obj;
        // class methodDELETE obj;
        string buffer;
    public:
        httpRequest();
        ~httpRequest();
        // string joinbuffer(string line);
        int defineTypeMethod(const string firstline);
        string checkHeaders(const string& str);
        void validRequestHeaders(); 
};
vector<string> splitstring(const string &str);