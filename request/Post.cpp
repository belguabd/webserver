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

int pasteInFile(std::string name, std::string &data)
{
	// if (fileExists(name))
	// {
	//     name += "_";
	//     return pasteInFile(name, data);
	// }
	std::ofstream file(name, std::ios::app);
	file << data;
	return 1;
}

// int Post::pasteInFile(std::string fileName, std::string &data)
// {
// 	_files[fileName].append(data);
// 	return (1);
// }

void printNonPrintableChars(const std::string &str) {
    for (char ch : str) {
        if (!isprint(static_cast<unsigned char>(ch))) {
            std::cout << "(x" << std::hex << std::setw(2) << std::setfill('0') << (int)(unsigned char)ch << ")";
        } else {
            std::cout << ch;
        }
    }
    std::cout << std::endl;
}

void eraseCrnl(std::string &str, int size)
{

	if (str.size() >= size + 2 && str[size + 1] == '\n' && str[size + 2] == '\r')
		str.erase(0, size + 2);
	str.erase(0, size );
}

int Post::handleChunked()
{
	std::string fileData;
	if (_chunkSize <= 0) // check is remending data
	{
		size_t i = _bufferBody.find("\n");
		if (i == std::string::npos)
			return 1;
		// std::cout << "bufferBody: \n" ;
		// printNonPrintableChars(_bufferBody);
		// std::cout << "end of sizechunk: " << i << "\n";
		std::string sizeChunkStr = _bufferBody.substr(0, i);
		_bufferBody.erase(0, i + 2); // + 1 for \n
		// std::cout << "after erase: \n";
		// printNonPrintableChars(_bufferBody);
		// get the size chunk erase it in the _bufferBody
		_chunkSize = strToLong(sizeChunkStr, 16);
		// std::cout << "_chunkSize: " << _chunkSize << std::endl;
	}

	if (_chunkSize == 0)
	{
		std::cout << "||";
		// printNonPrintableChars(_bufferBody);
		if (_bufferBody == "\r\n")
		{

			std::cout << "end of chunk\n";
		}
		return 1;
	}
	// && _bufferBody.size() < 0)
	fileData = _bufferBody.substr(0, _chunkSize);

	std::cout << "fileData : " << fileData << "\n";
 	eraseCrnl(_bufferBody, _chunkSize); // !!!!!!! \r\n
	// std::cout << 
	// _bufferBody.erase(0, _chunkSize + 2);
	pasteInFile(FILENAME, fileData);
	_chunkSize -= (long)fileData.size();
	// std::cout << "left size in chunk: " << _chunkSize << "\n";
	
	if (_chunkSize > 0) // uncompleted request
	{
		std::cout << "uncompleted request\n";
		return (long)fileData.size(); // uncompleted request
	}
	std::cout << "next chunk ================== \n";
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
	size_t pos = buffer.rfind("\n");
	if (pos == std::string::npos)
	{
		_remainingBuffer += buffer;
		return 1;
	}
	_bufferBody = _remainingBuffer;
	// if (buffer.size() > pos + 1 && buffer.at(pos + 1) == '\r')
	_remainingBuffer = buffer.substr(pos + 1);
	_bufferBody += buffer.substr(0, pos + 1);

	// _bufferBody = buffer;
	// printNonPrintableChars(_bufferBody);

	// std::cout << _bufferBody ;

	if (this->_bodyType == chunked)
	{
		// check that chunk size is valid
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
