#include "./Post.hpp"

Post::Post()
:chunk(_bufferBody, _remainingBuffer, _headers, _status),
bound(_bufferBody, _remainingBuffer, _headers, _status)
{
	setBodyType();
	_chunkSize = 0;
}

Post::Post(std::map<std::string, std::string> &headers, std::string &bufferBody)
: _headers(headers), _bufferBody(bufferBody), 
chunk(_bufferBody, _remainingBuffer, _headers, _status),
bound(_bufferBody, _remainingBuffer, _headers, _status)
{
	setBodyType();
	_chunkSize = 0;
}

Post::Post(std::map<std::string, std::string> &headers) 
	: _headers(headers), 
	chunk(_bufferBody, _remainingBuffer, _headers, _status),
	bound(_bufferBody, _remainingBuffer, _headers, _status)
{
	setBodyType();
	_chunkSize = -1;
}

Post& Post::operator=(const Post& other)
{
	if (this == &other)
		return *this;
	
	_bodyType = other._bodyType;
	_chunkSize = other._chunkSize;
	_headers = other._headers;
	_bufferBody = other._bufferBody;
	_remainingBuffer = other._remainingBuffer;
	_status = other._status;
	
	return *this;
}

void Post::setHeaders(std::map<std::string, std::string> &headers)
{
	this->_headers = headers;
}

void Post::setBodyType()
{
	if (_headers.find("Content-Type") != _headers.end() && _headers["Content-Type"].find("; boundary=") != std::string::npos)
	{
		if (_headers.find("Transfer-Encoding") != _headers.end() && _headers["Transfer-Encoding"] == "chunked")
			_bodyType = boundaryChunked;
		else
		{
			_bodyType = boundary;
			std::cout << "This is boundary\n";
		}
	}
	else if (_headers.find("Transfer-Encoding") != _headers.end() && _headers["Transfer-Encoding"] == "chunked")
		_bodyType = chunked;
	else
		_bodyType = contentLength;
}



void printNonPrintableChars(const std::string &str) {
	for (std::string::const_iterator it = str.begin(); it != str.end(); ++it) {
		if (!isprint(static_cast<unsigned char>(*it))) {
			std::cout << "(x" << std::hex << std::setw(2) << std::setfill('0') << (int)(unsigned char)(*it) << ")";
		} else {
			std::cout << *it;
		}
	}
	std::cout << std::endl;
}

bool fileExists(std::string &filePath)
{
	struct stat buffer;
	return (stat(filePath.c_str(), &buffer) == 0);
}

int Post::start( std::map<std::string, std::string> &headers, std::string &buffer)
{
	buffer = "\r\n" + buffer;
	_chunkSize = 0;
	setHeaders(headers);
	setBodyType();
	if (_bodyType == chunked)
		chunk.setFileName(chunk._mimeToExtension[chunk._headers["Content-Type"]]);
	if (_bodyType == boundary)
		bound.setBoundaryString();


	proseRequest(buffer);
	return 1;
}

int Post::proseRequest(std::string &buffer)
{

	size_t pos = buffer.rfind("\n");
	if (pos == std::string::npos)
	{
		_remainingBuffer += buffer;
		return 1;
	}
	_bufferBody = _remainingBuffer;
	_remainingBuffer = buffer.substr(pos + 1);
	_bufferBody += buffer.substr(0, pos + 1);
	// std::cout << "buffer now : " ;

	if (this->_bodyType == chunked)
	{
		chunk.handleChunked();
	}
	if (this->_bodyType == boundary)
	{
		bound.handleBoundary();
		// handleChunked();
	}
	if (this->_bodyType == contentLength)
	{
	}
	return 1;
}

Post::~Post()
{
}
