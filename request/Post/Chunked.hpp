#pragma once
#include "./Macros.hpp"
#include <string>
#include <map>
#include <cstdlib>
#include <fstream>
#include <iostream>
#include <algorithm>
#include <sys/stat.h> 
#include <fcntl.h>
#include <unistd.h>

class Chunked
{
public:
    std::string &_bufferBody;
    std::string &_remainingBuffer;
    std::map <std::string, std::string> &_headers;
    std::string _fileName;
    std::map<std::string, std::string> _mimeToExtension;
    size_t _chunkSize;
    std::string &_uploadStore;
    int &_status;
    // Chunked();
    Chunked(std::string &bufferBody, std::string &remainingBuffer, std::map<std::string, std::string> &headers, int &_status, std::string &uploadStore);
    Chunked &operator=(const Chunked &);
    size_t getChunkSize(std::string &buffer);
    void setFileName(std::string extention);
    int handleChunked();
    void initializeMimeTypes();
};
size_t pasteInFile(std::string name, std::string &data);