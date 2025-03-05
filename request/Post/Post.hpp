#pragma once
// #include "HttpRequest.hpp"
#include <map>
#include <cstdlib>
#include <fstream>
#include <iostream>
#include <sys/stat.h> 
#define FILENAME "output.json"
#include "Chunked.hpp"
#include "Boundary.hpp"
#include "BoundaryChunked.hpp"


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
    Chunked chunk;
    Boundary *bound;
    BoundaryChunked *boundChunk;
    std::map <std::string, std::string> _headers;
    std::string _bufferBody;
    std::string _remainingBuffer;
    std::map<std::string, std::string> _queryParam;
    std::map<std::string, std::string> _mimeToExtension;
    int _status;
    std::string _fileName;
    size_t contentLengthSize;
    void setFileName(std::string extention);
    void initializeMimeTypes();
public:
    Post();
    int getStatus() {return _status;}
    void handleContentLength(std::string &buffer);
    Post &operator=(const Post &);
    ~Post();
    void setHeaders(std::map<std::string, std::string> &headers);
    void setBodyType();
    Body getBodyType() { return _bodyType; }
    int start(std::map<std::string, std::string> &headers, std::map<std::string, std::string> &queryParam, std::string &buffer);
    int proseRequest(std::string &buffer);
};
void printNonPrintableChars(const std::string &str);
int pasteInFile(std::string name, std::string &data);
 