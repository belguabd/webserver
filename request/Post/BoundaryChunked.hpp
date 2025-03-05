#pragma once
#include "./Macros.hpp"
#include <string>
#include <map>
#include <cstdlib>
#include <fstream>
#include <iostream>
#include <sys/stat.h> 
#include "Boundary.hpp"

class BoundaryChunked
{
public:
    std::string &_bufferBody;
    std::string &_remainingBuffer;
    std::map <std::string, std::string> &_headers;
    std::string _fileName;
    std::map<std::string, std::string> _mimeToExtension;
    size_t _chunkSize;
    Boundary *_boundary;
	std::string _boundaryBuffer;
	std::string _remainingBoundaryBuffer;
    int &_status;
    // BoundaryChunked();
    BoundaryChunked(std::string &bufferBody, std::string &remainingBuffer, std::map<std::string, std::string> &headers, int &_status);
    // BoundaryChunked &operator=(const BoundaryChunked &);
    int handleChunkedBoundary();
    size_t getChunkSize(std::string &buffer);
    void setFileName(std::string extention);
    int handleChunkedRec();
    int boundaryPart();
    void initializeMimeTypes();
};
size_t pasteInFile(std::string name, std::string &data);
void    printNonPrintableChars(const std::string &str);