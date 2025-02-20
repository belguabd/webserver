// }

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

long Post::getChunkSize(std::string &buffer)
{
	if (buffer == std::string("\r\n"))
	{
		// std::cout << "due i enter on this when the sizeChunk=0 which mean the [chunk is done] so we saved in [remainingBody] \n";
		// std::cout << "this crln need set it in next part to make sure that is a valid chunkHead (\r\n0xN\r\n)\n";
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
    std::cout << "pos: " << pos << std::endl;
	for (int i = 2; i < pos; i++)
		if (!std::isxdigit(buffer[i]))
		{
			std::cout << "throw invalid head chunk3\n" << std::endl;
			return 0;
		}
    long n = strtol(buffer.substr(2, pos).c_str(), NULL, 16);
	buffer.erase(0, pos + 2);
	return n;
}

int Post::handleChunked()
{
	std::string fileData;
	if (_chunkSize <= 0) // check is remending data
	{
		std::cout << "buffer head : "; printNonPrintableChars(_bufferBody.substr(0, 12));
		_chunkSize = getChunkSize(_bufferBody);
		if (_chunkSize == 0)
			return 1;
	}

	fileData = _bufferBody.substr(0, _chunkSize);
	_bufferBody.erase(0, _chunkSize);
	pasteInFile(_fileName, fileData);
	_chunkSize -= (long)fileData.size();
	if (_chunkSize > 0)
	{
		std::cout << "uncompleted request\n";
		return (long)fileData.size(); // uncompleted request
	}
	return handleChunked();
}


bool fileExists(std::string &filePath)
{
	struct stat buffer;
	return (stat(filePath.c_str(), &buffer) == 0);
}

void Post::setFileName(std::string &name)
{
	struct stat b;
	int n = 0;
	std::cout << "filename: " << _fileName << std::endl;
	while (stat(name.c_str(), &b) != -1)
	{
		name = "_" + name;
	std::cout << "filename: " << _fileName << std::endl;
	}
	_fileName = name;

	std::cout << "filename: " << _fileName << std::endl;
}

int Post::start( std::map<std::string, std::string> &headers, std::string &buffer)
{
	buffer = "\r\n" + buffer;
	_chunkSize = 0;
	setHeaders(headers);
	setBodyType();
	proseRequest(buffer);
	if (_bodyType == chunked)
	{
		std::cout << "extention : " << _mimeToExtension[headers["Content-Type"]] << std::endl;
		setFileName(_mimeToExtension[headers["Content-Type"]]);
	}
	return 1;
}

int Post::proseRequest(std::string &buffer)
{

	std::cout << "next Part ================== \n";
	size_t pos = buffer.rfind("\n");
	if (pos == std::string::npos)
	{
		_remainingBuffer += buffer;
		// std::cout << "no newline \\n \n";
		return 1;
	}
	_bufferBody = _remainingBuffer;
	_remainingBuffer = buffer.substr(pos + 1);
	_bufferBody += buffer.substr(0, pos + 1);
	std::cout << "buffer now : " ;
	// printNonPrintableChars(_bufferBody.substr(0, 100)); 

	if (this->_bodyType == chunked)
	{
		// check that chunk size is valid
		handleChunked();
	}
	if (this->_bodyType == contentLength)
	{
	}
	return 1;
}

Post::~Post()
{
}
