#include "./Post.hpp"

void Post::initializeMimeTypes() {
    // Text types
    _mimeToExtension["text/plain"] = ".txt";
    _mimeToExtension["text/html"] = ".html";
    _mimeToExtension["text/css"] = ".css";
    _mimeToExtension["text/javascript"] = ".js";
    _mimeToExtension["text/markdown"] = ".md";
    _mimeToExtension["text/csv"] = ".csv";
    _mimeToExtension["text/x-c"] = ".cpp";
    
    // Application types
    _mimeToExtension["application/json"] = ".json";
    _mimeToExtension["application/xml"] = ".xml";
    _mimeToExtension["application/pdf"] = ".pdf";
    _mimeToExtension["application/zip"] = ".zip";
    _mimeToExtension["application/x-rar-compressed"] = ".rar";
    _mimeToExtension["application/x-tar"] = ".tar";
    _mimeToExtension["application/gzip"] = ".gz";
    _mimeToExtension["application/msword"] = ".doc";
    _mimeToExtension["application/vnd.ms-excel"] = ".xls";
    _mimeToExtension["application/vnd.openxmlformats-officedocument.wordprocessingml.document"] = ".docx";
    _mimeToExtension["application/vnd.openxmlformats-officedocument.spreadsheetml.sheet"] = ".xlsx";
    _mimeToExtension["application/octet-stream"] = ".bin";
    
    // Image types
    _mimeToExtension["image/jpeg"] = ".jpg";
    _mimeToExtension["image/png"] = ".png";
    _mimeToExtension["image/gif"] = ".gif";
    _mimeToExtension["image/svg+xml"] = ".svg";
    _mimeToExtension["image/webp"] = ".webp";
    _mimeToExtension["image/tiff"] = ".tiff";
    _mimeToExtension["image/bmp"] = ".bmp";
    _mimeToExtension["image/x-icon"] = ".ico";
    
    // Audio types
    _mimeToExtension["audio/mpeg"] = ".mp3";
    _mimeToExtension["audio/wav"] = ".wav";
    _mimeToExtension["audio/webm"] = ".weba";
    
    // Video types
    _mimeToExtension["video/mp4"] = ".mp4";
    _mimeToExtension["video/webm"] = ".webm";
    _mimeToExtension["video/ogg"] = ".ogv";
    
    // Font types
    _mimeToExtension["font/ttf"] = ".ttf";
    _mimeToExtension["font/woff"] = ".woff";
    _mimeToExtension["font/woff2"] = ".woff2";
    _mimeToExtension["font/otf"] = ".otf";
}


Post::Post()
{
	setBodyType();
	_chunkSize = 0;
	initializeMimeTypes();
}

Post::Post(std::map<std::string, std::string> &headers, std::string &bufferBody) : _headers(headers), _bufferBody(bufferBody)
{
	setBodyType();
	_chunkSize = 0;
	initializeMimeTypes();
}

Post::Post(std::map<std::string, std::string> &headers) : _headers(headers)
{
	setBodyType();
	_chunkSize = -1;
	initializeMimeTypes();
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
	pasteInFile(FILENAME, fileData);
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
	if (_bodyType == chunked)
	{
		std::cout << "extention : " << _mimeToExtension[headers["Content-Type"]] << std::endl;
		setFileName(_mimeToExtension[headers["Content-Type"]]);
	}
	proseRequest(buffer);
	return 1;
}

int Post::proseRequest(std::string &buffer)
{

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
