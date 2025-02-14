#pragma once
#include "HttpRequest.hpp"
#include <map>
#include <cstdlib>
#include <fstream>
#include <sys/stat.h> 

struct Chunked
{
    int sizeData;

};

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
    Body _bodyType;
    long _chunkSize;
    map <string, string> &_headers;
    string &_bufferBody;
    string _remainingBuffer;
    int handleChunked();
public:
    int status;
    void setBodyType();
    Body getBodyType() { return _bodyType; }
    Post(map <string, string> &headers, string &bufferBody);
    int proseRequest();
    ~Post();
};

