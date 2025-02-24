#pragma once

#include <map>
#include <cstdlib>
#include <fstream>
#include <iostream>
#include <algorithm>
#include <string>
#include <sys/stat.h>
// #ifndef PREFIXFILENAME "./data/"

struct BoundaryHead
{
    std::map <std::string, std::string> metadata;
    std::string boundaryString;
    size_t indexNextBoundary;
    bool isDone;
};

class Boundary
{
public:
    std::string &_bufferBody;
    std::string &_remainingBuffer;
    std::map <std::string, std::string> &_headers;
    int &_status;
    
    std::string _boundaryString;
    std::string _boundaryStringEnd;
    BoundaryHead _boundaryHead;

    Boundary(std::string &bufferBody, std::string &remainingBuffer, std::map<std::string, std::string> &headers, int &_status);
    void setBoundaryHead(std::string subBuffer);
    int setBoundaryHeadAndEraseBuffer();
    void setMetaData(std::string &headBoundary, std::string key);
    void setBoundaryHeadMetadata(std::string subBuffer);
    bool checkHeaderIsCompleted();
    // int checkBoundaryHead();
    void handleBoundary();
    int handleBoundaryRec();
    size_t getSizeOfBoundary();
    void setBoundaryString();
};
void printNonPrintableChars(const std::string &str);
int pasteInFile(std::string name, std::string &data);
