#include <iostream>
#include <string>
#include <fstream>
#include <cstdlib> // Required for strtol
#include <cerrno>  // Required for errno


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

int proseRequest(std::string &buffer)
{
	// size_t ;
    std::string _remainingBuffer;
    std::string _bufferBody;
	// _bufferBody = buffer.substr(buffer.rfind("\n"));
	// _remainingBuffer = bu
	std::cout << "next chunk ================== \n";
	size_t pos = buffer.rfind("\n");
	if (pos == std::string::npos)
	{
		_remainingBuffer += buffer;
		std::cout << "no newline \\n \n";
		return 1;
	}
	_bufferBody = _remainingBuffer;
	// if (buffer.size() > pos + 1 && buffer.at(pos - 1) == '\r')
	_remainingBuffer = buffer.substr(pos + 1);
	_bufferBody += buffer.substr(0, pos + 1);
	std::cout << "buffer now : " ;
	printNonPrintableChars(_bufferBody.substr(0, 100));
	return 1;
}


int main()
{
    std::string buffer = "\r\naa\r\nabcd\r\n10\r\n";
    std::cout << getChunkSize(buffer) << std::endl;
    printNonPrintableChars(buffer);
}
