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
#include "../../conf/ServerConfig.hpp"


class Post
{

enum Body
{
    boundary,
    chunked,
    contentLength,
    keyVal,
    boundaryChunked
};
private:

    Body _bodyType;
    Chunked *chunk;
    Boundary *bound;
    BoundaryChunked *boundChunk;
    std::map <std::string, std::string> &_headers;
    std::string _bufferBody; //!!
    std::string _remainingBuffer;
    std::map<std::string, std::string> &_queryParam;
    std::map<std::string, std::string> _mimeToExtension;
    int _status;
    std::string _fileName;
    std::string _uploadStore;
    size_t _contentLengthSize;
    size_t _bodySize;
    void setFileName(std::string extention);
    void initializeMimeTypes();
    size_t manipulateBuffer(std::string &buffer);
    void setContentLengthSize();
    LocationConfig &_configUpload;
    void createBodyTypeObject(std::string& buffer);
    int _isCgi;
public:
    Post(std::map<std::string, std::string> &headers, std::map<std::string, std::string> &queryParam, 
    std::string &buffer, LocationConfig &configUpload, int isCgi);
    std::string &getFileName();
    int getStatus() {return _status;}
    int handleKeyVal(std::string &buffer);
    int handleContentLength(std::string &buffer);
    Post &operator=(const Post &);
    ~Post();
    void setHeaders(std::map<std::string, std::string> &headers);
    void setBodyType();
    Body getBodyType() { return _bodyType; }
    int proseRequest(std::string &buffer);
};
size_t pasteInFile(std::string name, std::string &data);
 