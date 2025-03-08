#include "./Post.hpp"

Post::Post(std::map<std::string, std::string> &headers, std::map<std::string, std::string> &queryParam, std::string &buffer)
:_headers(headers), _queryParam(queryParam)
{
	// setHeaders(headers);
	_status = 0;
	initializeMimeTypes();
	buffer = "\r\n" + buffer;
	_status = 0;
	setBodyType();
	setContentLengthSize();
	if (_bodyType == chunked)
		chunk = new Chunked(_bufferBody, _remainingBuffer, _headers, _status);
	else if (_bodyType == boundary)
		bound = new Boundary(_queryParam, _bufferBody, _remainingBuffer, _headers, _status);
	else if (_bodyType == boundaryChunked)
	{
		buffer.insert(0, "\r\n2\r\n\r\n");
		boundChunk = new BoundaryChunked(_queryParam, _bufferBody, _remainingBuffer, _headers, _status);
	}
	else if (_bodyType == contentLength)
	{
		buffer.erase(0, 2); // to remove \r\n
		setFileName(_mimeToExtension[headers["Content-Type"]]);
	}
	proseRequest(buffer);
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

void Post::setContentLengthSize()
{
	if (_headers.find("Content-Length") != _headers.end())
		_contentLengthSize = std::stoi(_headers["Content-Length"]);
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
	else if (_headers.find("Content-Type") != _headers.end() && _headers["Content-Type"].find("x-www-form-urlencoded") != std::string::npos)
		_bodyType = keyVal;
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

int Post::handleContentLength(std::string &buffer)
{
	// printNonPrintableChars(buffer);
	if (pasteInFile(_fileName, buffer) >= _contentLengthSize)
		_status = 1;
	return _status;
}

// int Post::handleKeyVal(std::string &buffer)
// {
// 	buffer
// }

int Post::start(std::map<std::string, std::string> &headers, std::map<std::string, std::string> &queryParam, std::string &buffer)
{
	// std::cout <<"buffer "<<buffer<<std::endl;
	// _queryParam = queryParam;
	buffer = "\r\n" + buffer;
	_status = 0;
	setHeaders(headers);
	setBodyType();
	setContentLengthSize();
	if (_bodyType == chunked)
		chunk = new Chunked(_bufferBody, _remainingBuffer, _headers, _status);
	else if (_bodyType == boundary)
		bound = new Boundary(_queryParam, _bufferBody, _remainingBuffer, _headers, _status);
	else if (_bodyType == boundaryChunked)
	{
		buffer.insert(0, "\r\n2\r\n\r\n");
		boundChunk = new BoundaryChunked(_queryParam, _bufferBody, _remainingBuffer, _headers, _status);
	}
	else if (_bodyType == contentLength)
	{
		buffer.erase(0, 2); // to remove \r\n
		setFileName(_mimeToExtension[headers["Content-Type"]]);
	}
	return proseRequest(buffer);
}

size_t Post::manipulateBuffer(std::string &buffer)
{
	size_t pos = buffer.rfind("\n");
	if (pos == std::string::npos)
	{
		_remainingBuffer += buffer;
		return pos;
	}
	_bufferBody = _remainingBuffer;
	_remainingBuffer = buffer.substr(pos + 1);
	_bufferBody += buffer.substr(0, pos + 1);
	return pos;
}

int Post::proseRequest(std::string &buffer)
{
	if (this->_bodyType == contentLength)
		return handleContentLength(buffer);
	if (this->_bodyType == keyVal)
		std::cout << "keyVal\n";
	// std::cout << "=========buffer befor: =========";  printNonPrintableChars(buffer);
	if (manipulateBuffer(buffer) == std::string::npos)
		return _status;
	if (this->_bodyType == chunked)
		chunk->handleChunked();
	if (this->_bodyType == boundary)
		bound->handleBoundary();
	if (this->_bodyType == boundaryChunked)
		boundChunk->handleChunkedBoundary();
	return _status;
}

Post::~Post()
{
	if (_bodyType == chunked)
		delete chunk;
	else if (_bodyType == boundary)
		delete bound;
	else if (_bodyType == boundaryChunked)
		delete boundChunk;
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
