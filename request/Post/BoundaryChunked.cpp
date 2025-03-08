#include "BoundaryChunked.hpp"

BoundaryChunked::BoundaryChunked(std::map<std::string, std::string> &queryParam, std::string &bufferBody, std::string &remainingBuffer, std::map<std::string, std::string> &headers, int &_status):
    _bufferBody(bufferBody), _remainingBuffer(remainingBuffer), _headers(headers), _status(_status)
{
    _boundary = new Boundary(queryParam, _boundaryBuffer, _remainingBoundaryBuffer, _headers, _status);
	_chunkSize = 0;
    setFileName("filePost");
	// initializeMimeTypes();
}

void BoundaryChunked::setFileName(std::string extention)
{
	struct stat b;
	std::string name = UPLOAD_FOLDER + std::string("filePost");
	int n = 0;
	while (stat((std::string(name + extention)).c_str(), &b) != -1)
		name.append("_");
	_fileName = name + extention;
}

int BoundaryChunked::handleChunkedBoundary()
{
    return handleChunkedRec();
}

int BoundaryChunked::boundaryPart()
{
	// std::string buffer = 
	// size_t pos = buffer.rfind("\n");
	// if (pos == std::string::npos)
	// {
	// 	_remainingBuffer += buffer;
	// 	return 1;
	// }
	// _remainingBuffer = buffer.substr(pos + 1);
	// _bufferBody += buffer.substr(0, pos + 1);
	
	_boundaryBuffer = _bufferBody.substr(0, _chunkSize);
	_bufferBody.erase(0, _chunkSize);
	_chunkSize -= (long)_boundaryBuffer.size();
	_boundaryBuffer.insert(0, _remainingBoundaryBuffer);
	_remainingBoundaryBuffer  = "";
	// std::cout << "{{{{{{{{{{_buffer in boundaryChunked}}}}}}}}\n";
    // printNonPrintableChars(_boundaryBuffer);
    // std::cout << "{{{{{{{{{{end _buffer in boundaryChunked}}}}}}}}}\n";
    _boundary->handleBoundary();
	return 1;
}

int BoundaryChunked::handleChunkedRec()
{
    // std::cout << "in buffer in chunkedBoundary============\n";
    // printNonPrintableChars(_bufferBody);
    // std::cout << "============\n";
	if (_chunkSize <= 0) // check is remending data
	{
		// std::cout << "buffer head : "; printNonPrintableChars(_bufferBody.substr(0, 12));
		_chunkSize = getChunkSize(_bufferBody);
		if (_chunkSize == 0)
			return 1;
	}

	
	boundaryPart();
	// pasteInFile(_fileName, _boundaryBuffer);
    // _boundary.handleBoundary();
    // std::cout << _boundaryBuffer << std::endl;

	// std::cout << "_chunkSize: " << _chunkSize << std::endl;
    if (_chunkSize > 0)
	{
		// std::cout << "uncompleted request\n";
		return (long)_boundaryBuffer.size(); // uncompleted request
	}
	return handleChunkedRec();
}

// size_t pasteInFile(std::string name, std::string &data)
// {
// 	std::ofstream file(name, std::ios::app);
// 	if (!file.is_open())
// 	{
// 		std::cout << "Failed to open file: " << name << std::endl;
// 		return 0;
// 	}
// 	file << data;
// 	file.close();
// 	return 1;
// }


size_t BoundaryChunked::getChunkSize(std::string &buffer)
{
	if (buffer == std::string("\r\n"))
	{
		// std::cout << "due i enter on this when the sizeChunk=0 which mean the [chunk is done] so we saved in [remainingBody] \n";
		// std::cout << "this crlf need set it in next part to make sure that is a valid chunkHead (\r\n0xN\r\n)\n";
		this->_remainingBuffer.insert(0, buffer);
		return 0;
	}
	if (buffer == std::string("\r\n0\r\n"))
	{
		// std::cout << "I need this to this to add it in next for bodyBuffer to make sure is completed\n";
		this->_remainingBuffer.insert(0, buffer);
		return 0;
	}
	if (std::string(buffer + _remainingBuffer) == std::string("\r\n0\r\n\r\n"))
	{
		std::cout << "end of req" << std::endl;
		_status = 1;
		std::cout << "check file: " << _fileName << std::endl;
		return 0;
	}

	if (buffer.substr(0,2) != std::string("\r\n"))
	{
		std::cout << "throw invalid head chunk1" << std::endl;
		return 0;
	}
	size_t pos = buffer.find("\r\n", 2);
	if (pos == std::string::npos)
	{
		std::cout << "throw invalid head chunk2\n" << std::endl;
		return 0;
	}
	for (int i = 2; i < pos; i++)
		if (!std::isxdigit(buffer[i]))
		{
			std::cout << "throw invalid head chunk3\n" << std::endl;
			return 0;
		}
    size_t n = strtol(buffer.substr(2, pos).c_str(), NULL, 16);
	buffer.erase(0, pos + 2);
	return n;
}