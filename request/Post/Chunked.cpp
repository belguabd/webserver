#include "./Chunked.hpp"

// Chunked::Chunked():_bufferBody(), _remainingBuffer(), _headers();
// {}

Chunked &Chunked::operator=(const Chunked &other)
{
	if (this == &other)
		return *this;
	_chunkSize = other._chunkSize;
	_headers = other._headers;
	_bufferBody = other._bufferBody;
	_remainingBuffer = other._remainingBuffer;
	_status = other._status;
	return *this;
}

Chunked::Chunked(std::string &bufferBody, std::string &remainingBuffer, std::map<std::string, std::string> &headers, int &_status):
    _bufferBody(bufferBody), _remainingBuffer(remainingBuffer), _headers(headers), _status(_status)
{
	_chunkSize = 0;
	initializeMimeTypes();
}

void Chunked::setFileName(std::string extention)
{
	struct stat b;
	std::string name = UPLOAD_FOLDER + std::string("filePost");
	int n = 0;
	while (stat((std::string(name + extention)).c_str(), &b) != -1)
		name.append("_");
	_fileName = name + extention;
}

int Chunked::handleChunked()
{
	std::string fileData;
	if (_chunkSize <= 0) // check is remending data
	{
		// std::cout << "buffer head : "; printNonPrintableChars(_bufferBody.substr(0, 12));
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
		// std::cout << "uncompleted request\n";
		return (long)fileData.size(); // uncompleted request
	}
	return handleChunked();
}

int pasteInFile(std::string name, std::string &data)
{
	std::ofstream file(name, std::ios::app);
	std::string d = data;
	if (name == "currentRequest")
	{
		std::cout << "in current request\n";
		std::string::size_type pos = 0;
		const std::string from = "\r\n";
		const std::string to = "\\r\\n\n";

		while ((pos = d.find(from, pos)) != std::string::npos) {
			d.replace(pos, from.length(), to);
			pos += to.length(); // Move past the replacement
		}
	}
	if (!file.is_open())
	{
		std::cerr << "Failed to open file: " << name << " - " << std::strerror(errno) << std::endl;
		return 0;
	}
	file << d;
	file.close();
	return 1;
}


size_t Chunked::getChunkSize(std::string &buffer)
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



void Chunked::initializeMimeTypes() {
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

