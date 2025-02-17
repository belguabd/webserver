#include "./Post.hpp"

Post::Post()
{
	setBodyType();
	_chunkSize = -1;
}

Post::Post(std::map<std::string, std::string> &headers, std::string &bufferBody) : _headers(headers), _bufferBody(bufferBody)
{
	setBodyType();
	_chunkSize = -1;
}

Post::Post(std::map<std::string, std::string> &headers) : _headers(headers)
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
	if (_headers.find("Content-Type") != _headers.end() && _headers["Content-Type"].find("boundary") != std::string::npos)
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
	
	if (str.back() == '\r')
		str.pop_back();
	if (str.back() == '\n')
		str.pop_back();
	for (int i = 0; i < str.size() ; i++)
	{
		if (!std::isdigit(str[i]) && (str[i] < 'a' || str[i] > 'f'))
			return 0;
	}
	return strtol(str.c_str(), &end, base);
}

bool fileExists(std::string &filePath)
{
	struct stat buffer;
	return (stat(filePath.c_str(), &buffer) == 0);
}

// int pasteInFile(std::string name, std::string &data)
// {
// 	// if (fileExists(name))
// 	// {
// 	//     name += "_";
// 	//     return pasteInFile(name, data);
// 	// }
// 	std::ofstream file(name, std::ios::app);
// 	file << data;
// 	return 1;
// }

int Post::pasteInFile(std::string fileName, std::string &data)
{
	_files[fileName].append(data);
	return (1);
}

int Post::handleChunked()
{
	std::string fileData;
	if (_chunkSize <= 0) // check is remending data
	{
		int i = _bufferBody.find("\n");
		std::string sizeChunkStr = _bufferBody.substr(0, i);
		// std::cout << "sizeChunkStr : " << sizeChunkStr << std::endl;
		_bufferBody.erase(0, i + 1); // + 1 for \n
		// get the size chunk erase it in the _bufferBody
		_chunkSize = strToLong(sizeChunkStr, 16);
		// std::cout << "_chunkSize: " << _chunkSize << std::endl;
	}

	if (_chunkSize == 0)
	{
		return 1;
	} // && _bufferBody.size() < 0)
	fileData = _bufferBody.substr(0, _chunkSize);

	// std::cout << "bufferbody :\n" << _bufferBody << std::endl;
	// std::cout<< "filedata :\n|" << fileData << "\n|";;
	_bufferBody.erase(0, _chunkSize + 1);

	
	pasteInFile("output", fileData);
	_chunkSize -= (long)fileData.size();
	// std::cout << "left size in chunk: " << _chunkSize << "\n";
	
	if (_chunkSize > 0) // uncompleted request
		return (long)fileData.size(); // uncompleted request
	// std::cout << "-----------\n";
	return handleChunked();
}

int Post::start( std::map<std::string, std::string> &headers, std::string &buffer)
{
	setHeaders(headers);
	setBodyType();
	proseRequest(buffer);
}

int Post::proseRequest(std::string &buffer)
{
	// size_t ;
	// _bufferBody = buffer.substr(buffer.rfind("\n"));
	// _remainingBuffer = bu
	_bufferBody = buffer;
	std::cout << _bufferBody ;

	if (this->_bodyType == chunked)
	{
		handleChunked();
	}
	if (this->_bodyType == contentLength)
	{
		handleChunked();
	}
	return 1;
}

Post::~Post()
{
}
