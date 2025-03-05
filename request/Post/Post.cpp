#include "./Post.hpp"

Post::Post() 
: chunk(_bufferBody, _remainingBuffer, _headers, _status)
//, bound(_bufferBody, _remainingBuffer, _headers, _status)
{
	setBodyType();
	_status = 0;
	initializeMimeTypes();
}

Post &Post::operator=(const Post &other)
{
	if (this == &other)
		return *this;

	_bodyType = other._bodyType;
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

void printNonPrintableChars(const std::string &str)
{
	for (std::string::const_iterator it = str.begin(); it != str.end(); ++it)
	{
		if (!isprint(static_cast<unsigned char>(*it)))
		{
			std::cout << "(x" << std::hex << std::setw(2) << std::setfill('0') << (int)(unsigned char)(*it) << ")";
			if (static_cast<unsigned char>(*it) == '\n')
				std::cout << "\n";
		}
		else
		{
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

void Post::setFileName(std::string extention)
{
	struct stat b;
	std::string name = UPLOAD_FOLDER + std::string("filePost");
	int n = 0;
	while (stat((std::string(name + extention)).c_str(), &b) != -1)
		name.append("_");
	_fileName = (name + extention);
}

void Post::handleContentLength(std::string &buffer)
{
	printNonPrintableChars(_bufferBody);
	pasteInFile(_fileName, buffer);
}

int Post::start(std::map<std::string, std::string> &headers, std::map<std::string, std::string> &queryParam, std::string &buffer)
{
	buffer = "\r\n" + buffer;
	// std::cout <<"buffer "<<buffer<<std::endl;
	_queryParam = queryParam;
	_status = 0;
	setHeaders(headers);
	setBodyType();
	if (_bodyType == chunked)
		chunk.setFileName(chunk._mimeToExtension[chunk._headers["Content-Type"]]);
	else if (_bodyType == boundary)
	{
		std::cout << "Chunked\n";
		bound = new Boundary(_bufferBody, _remainingBuffer, _headers, _status);
	}
	else if (_bodyType == boundaryChunked)
	{
		buffer.insert(0, "\r\n2\r\n\r\n");
		std::cout << "boundaryChunked\n";
		boundChunk = new BoundaryChunked(_bufferBody, _remainingBuffer, _headers, _status);
	}
	else if (_bodyType == contentLength)
	{
		buffer.erase(0,2); // to remove \r\n
		setFileName(_mimeToExtension[headers["Content-Type"]]);
	}

	// for (const auto& header : _headers) {
	// 	std::cout << header.first << ":{" << header.second << "}" << std::endl;
	// }
	return proseRequest(buffer);
}

int Post::proseRequest(std::string &buffer)
{
	if (this->_bodyType == contentLength)
		handleContentLength(buffer);
	std::cout << "=========buffer befor: =========";  printNonPrintableChars(buffer);
	size_t pos = buffer.rfind("\n");
	if (pos == std::string::npos)
	{
		_remainingBuffer += buffer;
		return 1;
	}
	_bufferBody = _remainingBuffer;
	_remainingBuffer = buffer.substr(pos + 1);
	_bufferBody += buffer.substr(0, pos + 1);
	std::cout << "=========buffer after: =========";  printNonPrintableChars(buffer);
	// std::cout << "=<>=<>=<>=<>=<>=<>=<>=<>buffer body=<>=<>=<>=<>=<>=<>=<>=<>=<> \n"; printNonPrintableChars(_bufferBody);
	// std::cout << "==<>=<>=<>=<>=<>=<>=<>=<>=<>=<>=<>======<>=<>=<>=<>=<>=<>=<>=<>=<>=<>======<>=<>=<>=<>=<>=<>=<>\n";
	if (this->_bodyType == chunked)
	{
		chunk.handleChunked();
	}
	if (this->_bodyType == boundary)
	{
		bound->handleBoundary();
		// handleChunked();
	}
	if (this->_bodyType == boundaryChunked)
	{
		boundChunk->handleChunkedBoundary();
		// handleChunked();
	}
	return _status;
}

Post::~Post()
{
}

void Post::initializeMimeTypes() 
{
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
