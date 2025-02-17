#pragma once

#include <iostream>
#include <string>
#include <vector>
#include <map>

using namespace std;
class HttpResponse
{
    private:
    int client_fd;
    vector <string> firstline;
    string writebuffer;
    map <string ,string >mapheaders;
    public:
    HttpResponse();
    ~HttpResponse();
    int writeData();

};