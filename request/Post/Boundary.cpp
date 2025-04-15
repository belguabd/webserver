#include "Boundary.hpp"


// Boundary::Boundary(std::string &bufferBody, std::string &remainingBuffer, std::map<std::string, std::string> &headers, int &_status):
//     _bufferBody(bufferBody), _remainingBuffer(remainingBuffer), _headers(headers), _status(_status)
// {
//     setBoundaryString();
// }

Boundary::Boundary(std::map<std::string, std::string> &queryParam, std::string &bufferBody, \
    std::string &remainingBuffer, std::map<std::string, std::string> &headers, int &status, std::string &uploadStore):
    _queryParam(queryParam), _bufferBody(bufferBody),
    _remainingBuffer(remainingBuffer), _headers(headers), _status(status)
    ,_uploadStore(uploadStore)
{
    _status = 0;
    setBoundaryString();
}

void Boundary::setBoundaryString()
{
    std::string str = _headers["CONTENT_TYPE"];
    size_t pos = str.find("; boundary=") + 11;
    _boundaryString = std::string("\r\n--").append(str.substr(pos) + "\r\n");
    _boundaryStringEnd = std::string("\r\n--").append(str.substr(pos) + "--\r\n");           
}


void Boundary::setFileName(std::string &fileName)
{
    fileName.insert(0, _uploadStore + "/");
    if (fileName == "") return ;
	struct stat b;
    // fileName = fileName;
    size_t pos =  fileName.find(".");
	std::string name = fileName.substr(0, pos);
    std::string extention ;
    if (pos < fileName.size())
        extention = fileName.substr(pos);
	int n = 0;
	while (stat((std::string(name + extention)).c_str(), &b) != -1)
		name.append("_");
	fileName = name + extention;
    std::cout << "fileName : " << fileName << std::endl;

}

std::string Boundary::getBoundaryString()
{
    return _boundaryString;
}

void Boundary::setMetaData(std::string &headBoundary, std::string key)
{
    std::string val;
    size_t posName = headBoundary.find(key + "=\"", _boundaryString.size());
    _metadata[key] = "";
    if (posName != std::string::npos)
    {
        posName += key.length() + 2;
        size_t posNameVal = headBoundary.find("\"", posName + 1);
        if (posNameVal != std::string::npos)
            _metadata[key] = headBoundary.substr(posName, posNameVal - posName);
    }
    if (key == "filename")
    {
        setFileName(_metadata[key]);

    }
}

size_t Boundary::getSizeOfBoundary()
{
    _indexNextBoundary = _bufferBody.find(_boundaryString);
     if (_indexNextBoundary == std::string::npos)
    {
        _indexNextBoundary = _bufferBody.find(_boundaryStringEnd);
        if (_indexNextBoundary == std::string::npos)
            _indexNextBoundary = _bufferBody.size() - 2;
    }
    return _indexNextBoundary; 
}

int Boundary::setBoundaryHeadAndEraseBuffer()
{
    if ((_bufferBody.find(_boundaryString)) != 0)
        return 0;

    if (_bufferBody.find(_boundaryString) == 0)
    {
        std::cout << "filename: " << _metadata["filename"] << std::endl;
        // std::cout << "is valid boundary\n";
    }

    size_t sizeHeadBoundary = _bufferBody.find("\r\n\r\n");
    if (sizeHeadBoundary == std::string::npos)
        return -1;

    std::string headBoundary = _bufferBody.substr(0, sizeHeadBoundary + 4);

    _bufferBody.erase(0, sizeHeadBoundary + 4);
    setMetaData(headBoundary, "filename");
    setMetaData(headBoundary, "name");
    return 1;
}

bool Boundary::checkHeaderIsCompleted()
{
    if (_bufferBody[0] != '\r')
        return true;
    size_t countCrlf = std::count(_bufferBody.begin(), _bufferBody.end(), '\r');
    if (countCrlf >= 5)
        return true;
    else 
    return false;
}

int Boundary::handleBoundary()
{
    
    // if (_bufferBody.empty())
    // {
    //     _status = 1; // 404
    //     std::cout << "adsdasda\n";
    //     return _status;
    // }
    // if (_bufferBody.size() < 5120)
    // {
    //     if (_bufferBody.find(_boundaryStringEnd) == std::string::npos)
    //     {
    //         _status = 1; // 404
    //         return _status;
    //     }
    // }
    handleBoundaryRec();
    return _status;
}
int Boundary::handleBoundaryRec()
{
    if (_bufferBody.size() <= 2)
    {
        // std::cout << "is not completed\n";
        _remainingBuffer = _bufferBody + _remainingBuffer;
        return _status;
    }
    if (_bufferBody.substr(0, _boundaryStringEnd.size()) == _boundaryStringEnd)
    {
        std::cout << "end boundary" << std::endl;
        _status = 201;
        _bufferBody = "";
        return _status;
    }
    
    if (setBoundaryHeadAndEraseBuffer() == -1)
    {
        // std::cout <<  "since we don't has \\r\\n\\r\\n and we have boundaryString I wait until the next read \n";
        if (_bufferBody.find(_boundaryStringEnd) != std::string::npos)
            _status = 404; // 404
        _remainingBuffer = _bufferBody + _remainingBuffer;
        return -1;
    }
    _indexNextBoundary = getSizeOfBoundary();
    std::string content = _bufferBody.substr(0, _indexNextBoundary);
    if (!(_metadata["filename"] == "")) // filename
    {
        pasteInFile( _metadata["filename"], content);
        // this is can be something
    }
    else
    {
        std::string key = _metadata["name"];
        _queryParam[key].append(content);
    }
    _bufferBody.erase(0, _indexNextBoundary);
    handleBoundaryRec();
    return _status;
}
