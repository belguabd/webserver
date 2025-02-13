#pragma once
#include "HttpRequest.hpp"
#include <map>
#include <cstdlib>
#include <fstream>
#include <sys/stat.h> 



enum Body
{
    boundary,
    chunked,
    contentLength,
    boundaryChunked
};

class Post
{
private:
    Body bodyType;
    map <string, string> &headers;
    string &bufferBody;
    string remainingBuffer;
    int handleChunked();
public:
    int status;
    void setBodyType();
    Body getBodyType() { return bodyType; }
    Post(map <string, string> &headers, string &bufferBody);
    int proseRequest();
    ~Post();
};

