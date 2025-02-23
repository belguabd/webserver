#include "Boundary.hpp"


Boundary::Boundary(std::string &bufferBody, std::string &remainingBuffer, std::map<std::string, std::string> &headers, int &_status):
    _bufferBody(bufferBody), _remainingBuffer(remainingBuffer), _headers(headers), _status(_status)
{}

void Boundary::setBoundaryString()
{
    std::string str = _headers["Content-Type"];
    size_t pos = str.find("; boundary=") + 11;
    _boundaryString = std::string("\r\n--").append(str.substr(pos) + "\r\n");
    _boundaryStringEnd = std::string("\r\n--").append(str.substr(pos) + "--\r\n");
    // std::cout << "_boundaryString: " << _boundaryString << std::endl;
    // std::cout << "_boundaryString: " << _boundaryStringEnd << std::endl;
}

void Boundary::setBoundaryHeadMetadata(std::string subBuffer)
{
    size_t len = subBuffer.find(";");

    if (subBuffer[0] == '\r' && subBuffer[1] == '\n')
        return;
    size_t pos = subBuffer.find("=");
    if (pos > len)
    {
        std::cout << "throw\n"; // throw
        return ;
    }
    std::string key = subBuffer.substr(1, pos); // set 1 to skip `espace` " "
    size_t pos1 = subBuffer.find(";", pos);
    if (pos1 > len)
    {
        std::cout << "throw\n"; // throw
        return ;
    }
    std::string value = subBuffer.substr(pos + 1, pos1 - pos + 1);
    _boundaryHead.metadata[key] = value;
}



void Boundary::setBoundaryHead(std::string subBuffer)
{
    size_t len = subBuffer.find_first_of(";\r");

    if (subBuffer[0] == '\n')
        return;
    size_t pos = subBuffer.find("=");
    if (pos > len)
    {
        std::cout << "throw1\n"; // throw
        return ;
    }
    std::string key = subBuffer.substr(1, pos - 1); // set 1 to skip `espace` " "
    size_t pos1 = subBuffer.find_first_of(";\r", pos);
	if (pos1 > len)
    {
        std::cout << "throw2\n"; // throw
        return ;
    }
    std::string value = subBuffer.substr(pos + 2, pos1 - pos - 3);
    _boundaryHead.metadata[key] = value;
	std::string param = subBuffer.substr(len+1);

	setBoundaryHeadMetadata(param);
}

void Boundary::setMetaData(std::string &headBoundary, std::string key)
{
    std::string val;
    size_t posName = headBoundary.find(key + "=\"", _boundaryString.size());
    _boundaryHead.metadata[key] = "";
    if (posName != std::string::npos)
    {
        posName += key.length() + 2; // add ="
        size_t posNameVal = headBoundary.find("\"", posName + 1);
        if (posNameVal != std::string::npos)
            _boundaryHead.metadata[key] = headBoundary.substr(posName, posNameVal - posName);
    }
}

int Boundary::setBoundaryHeadAndEraseBuffer()
{
    if (_bufferBody.find(_boundaryString) != 0)
        return 0;
    if (_bufferBody.find(_boundaryString) == 0)
        std::cout << "is valid boundary\n";
    _boundaryHead.isDone = false;
    size_t sizeHeadBoundary = _bufferBody.find("\r\n\r\n");
    if (sizeHeadBoundary == std::string::npos)
    {
        std::cout << "sizeHeadBoundary: " << sizeHeadBoundary << std::endl;
        std::cout << "not a complete header boundary: " << std::endl;
        _remainingBuffer = _bufferBody + _remainingBuffer;
    }
    std::string headBoundary = _bufferBody.substr(0, sizeHeadBoundary + 4);
    _bufferBody.erase(0, sizeHeadBoundary + 4);
    // std::cout << "buffer after earase" << _bufferBody << "]]]]]]]\n";
    setMetaData(headBoundary, "name");
    setMetaData(headBoundary, "filename");
    
    _boundaryHead.indexNextBoundary = _bufferBody.find(_boundaryString);
    if (_boundaryHead.indexNextBoundary == std::string::npos)
    {
        // std::cout << "I am in _boundaryStringEnd\n";
        _boundaryHead.indexNextBoundary = _bufferBody.find(_boundaryStringEnd);
    }
    // std::cout << "headBoundary: ";
    // printNonPrintableChars(headBoundary);
    return 1;
}

int Boundary::checkBoundaryHead()
{
    // int n = std::count(_bufferBody.begin(), _bufferBody.end(), "\r\n");
    // if (n < 5)
    //     return 1;
    // n = _bufferBody.find(_boundaryString);
    // if (n  == std::string::npos && n != 2)
    //     return 1;
    // setBoundaryHeadAndErase();

    // std::string getContentDisposition(_bufferBody.substr(n + 1))
    return 1;
}

int Boundary::checkHeaderIsCompleted()
{
    // if ()
    return 1;
}

int Boundary::handleBoundary()
{
    // if (_bufferBody.find("\r") != std::string::npos)
    // checkHeaderIsCompleted();
    if (_bufferBody.empty() || _bufferBody.substr(0, _boundaryStringEnd.size()) == _boundaryStringEnd)
        return 1;
    setBoundaryHeadAndEraseBuffer();
    // std::cout << "_boundaryHead.indexNextBoundary: " << _boundaryHead.indexNextBoundary << std::endl;
    // std::cout << "_boundaryHead.metadata[\"filename\"] :" << _boundaryHead.metadata["filename"] << std::endl;
    if (!(_boundaryHead.metadata["filename"] == ""))
    {
        std::string fileData = _bufferBody.substr(0, _boundaryHead.indexNextBoundary);
        // std::cout << "fileData :" <<  fileData << "||||||\n";
        // std::ofstream file(std::string(PREFIXFILENAME) + _boundaryHead.metadata["filename"], std::ios::trunc);
        // pasteInFile(std::string("./data/") + _boundaryHead.metadata["filename"], fileData);
        pasteInFile(std::string("/Users/emagueri/goinfre/uploads/") + _boundaryHead.metadata["filename"], fileData);
        if (_boundaryHead.indexNextBoundary != std::string::npos)
        {
            _boundaryHead.isDone = true;
            _bufferBody.erase(0, _boundaryHead.indexNextBoundary);
            handleBoundary();
            // std::cout << "this is completed\n";
        }
        else
            _bufferBody.erase(0, _boundaryHead.indexNextBoundary);
    }
    // std::cout << "_bufferBody ->" << _bufferBody << std::endl;
    return 1;
}
