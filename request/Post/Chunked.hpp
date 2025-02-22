#pragma once
#define PREFIXFILENAME "./data/filePost"
#include <string>
#include <map>
#include <cstdlib>
#include <fstream>
#include <iostream>
#include <sys/stat.h> 

class Chunked
{
public:
    std::string &_bufferBody;
    std::string &_remainingBuffer;
    std::map <std::string, std::string> &_headers;
    std::string _fileName;
    std::map<std::string, std::string> _mimeToExtension;
    size_t _chunkSize;
    int _status;
    // Chunked();
    Chunked(std::string &bufferBody, std::string &remainingBuffer, std::map<std::string, std::string> &headers);
    Chunked &operator=(const Chunked &);
    size_t getChunkSize(std::string &buffer);
    void setFileName(std::string extention);
    int handleChunked();
    void initializeMimeTypes();
};
int pasteInFile(std::string name, std::string &data);