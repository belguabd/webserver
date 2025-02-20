#pragma once
// #include "HttpRequest.hpp"
#include <map>
#include <cstdlib>
#include <fstream>
#include <iostream>
#include <sys/stat.h> 
#define FILENAME "output.json"

// struct Chunked
// {
//     size_t size;
//     std::string name;
//     std::string extention;
//     std::string &_bufferBody
// };

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

    std::map<std::string, std::string> _mimeToExtension;
    Body _bodyType;
    size_t _chunkSize;
    std::string _fileName;
    std::map <std::string, std::string> _headers;
    std::map <std::string, std::string> _files;
    std::string _bufferBody;
    std::string _remainingBuffer;
    int handleChunked();
public:
    Post();
    long getChunkSize(std::string &buffer);
    Post(std::map <std::string, std::string> &headers, std::string &bufferBody);
    Post(std::map<std::string, std::string> &headers);
    void initializeMimeTypes();
    Post &operator=(const Post &);
    ~Post();
    void setHeaders(std::map<std::string, std::string> &headers);
    int _status;
    void setBodyType();
    Body getBodyType() { return _bodyType; }
    void setFileName(std::string &extention);
    int start(std::map<std::string, std::string> &headers, std::string &buffer);
    int proseRequest(std::string &buffer);
};
void printNonPrintableChars(const std::string &str);
int pasteInFile(std::string name, std::string &data);
 