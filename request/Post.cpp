#include "./Post.hpp"

Post::Post(map<string, string> &headers, string &bufferBody) : _headers(headers), _bufferBody(bufferBody)
{
    setBodyType();
    _chunkSize = -1;
}

void Post::setBodyType()
{
    if (_headers.find("content") != _headers.end() && _headers["Content-Type"].find("boundary") != string::npos)
    {
        if (_headers.find("Transfer-Encoding") != _headers.end() && _headers["Transfer-Encoding"] == "chunked")
            _bodyType = boundaryChunked;
        else
            _bodyType = boundary;
    }
    else if (_headers.find("Transfer-Encoding") != _headers.end() && _headers["Transfer-Encoding"] == "chunked")
        _bodyType = chunked;
    else
        _bodyType = contentLength;
}

long strToLong(std::string &str, int base)
{
    char *end;
    return strtol(str.c_str(), &end, base);
}

bool fileExists(std::string &filePath)
{
    struct stat buffer;
    return (stat(filePath.c_str(), &buffer) == 0);
}

int pasteInFile(std::string name, std::string &data)
{
    // if (fileExists(name))
    // {
    //     name += "_";
    //     return pasteInFile(name, data);
    // }
    cout << data;
    std::ofstream file(name, std::ios::app);
    file << data;
    return 1;
}

int Post::handleChunked()
{
    cout << "_sizeChunk: " << _chunkSize;
    if (_chunkSize <= 0) // check is remending data
    {
        int i = _bufferBody.find("\n");
        std::string sizeChunkStr = _bufferBody.substr(0, i);
        _bufferBody.erase(0, i + 1); // + 1 for \n
        // get the size chunk erase it in the _bufferBody
        _chunkSize = strToLong(sizeChunkStr, 16);
    }

    if (_chunkSize == 0) // && _bufferBody.size() < 0)
        return 1;
    
    std::string fileData = _bufferBody.substr(0, _chunkSize);
    _bufferBody.erase(0, _chunkSize + 1);
    // std::cout << "bufferbody :\n" << _bufferBody;
    
    pasteInFile("output", fileData);
    _chunkSize -= (long)fileData.size();
    cout << "left size in chunk: " << _chunkSize << "\n";
    
    if (_chunkSize > 0)
    {
        cout << "uncompleted request\n";
        cout << "left size in chunk: " << _chunkSize << "\n";
        return (long)fileData.size(); // uncompleted request
    }
    
    return handleChunked();
}

int Post::proseRequest()
{
    if (this->_bodyType == chunked)
    {
        handleChunked();

    }
    return 1;
}

Post::~Post()
{
}
