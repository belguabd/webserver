#pragma once

#include <map>
#include <cstdlib>
#include <fstream>
#include <iostream>
#include <algorithm>
#include <string>
#include <sys/stat.h>

// #define UPLOAD_FOLDER "./upload/"
// #define UPLOAD_FOLDER "/Users/emagueri/goinfre/server/"
// #ifndef PREFIXFILENAME "./data/"

struct BoundaryHead
{
};

class Boundary
{
public:
    std::map<std::string, std::string> &_queryParam;
    std::string &_bufferBody;
    std::string &_remainingBuffer;
    std::map <std::string, std::string> &_headers;
    int &_status;
    std::string &_uploadStore;
    std::map <std::string, std::string> _metadata;
    size_t _indexNextBoundary;
    
    std::string _boundaryString;
    std::string _boundaryStringEnd;
    BoundaryHead _boundaryHead;

    // Boundary(std::string &bufferBody, std::string &remainingBuffer, std::map<std::string, std::string> &headers, int &_status);
    Boundary(std::map<std::string, std::string> &query, std::string &bufferBody, std::string &remainingBuffer,\
        std::map<std::string, std::string> &headers, int &_status, std::string &uploadStore);
    int setBoundaryHeadAndEraseBuffer();
    void setMetaData(std::string &headBoundary, std::string key);
    bool checkHeaderIsCompleted();
    int handleBoundary();
    int handleBoundaryRec();
    size_t getSizeOfBoundary();
    void setBoundaryString();
    void setFileName(std::string &fileName);
    std::string getBoundaryString();
};
void printNonPrintableChars(const std::string &str);
size_t pasteInFile(std::string name, std::string &data);
