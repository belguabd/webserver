#pragma once
// #include "HttpRequest.hpp"
#include <map>
#include <cstdlib>
#include <fstream>
#include <iostream>
#include <sys/stat.h> 

class Post
{

enum Body
{
    boundary,
    chunked,
    contentLength,
    boundaryChunked
};
private:
    Body _bodyType;
    long _chunkSize;
    std::map <std::string, std::string> _headers;
    std::map <std::string, std::string> _files;
    std::string _bufferBody;
    std::string _remainingBuffer;
    int handleChunked();
public:
    Post();
    Post(std::map <std::string, std::string> &headers, std::string &bufferBody);
    Post(std::map<std::string, std::string> &headers);
    Post &operator=(const Post &);
    ~Post();
    void setHeaders(std::map<std::string, std::string> &headers);
    int _status;
    void setBodyType();
    Body getBodyType() { return _bodyType; }
    int proseRequest(std::string &buffer);
};
