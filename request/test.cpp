#include <iostream>
#include <string>
#include <fstream>
#include <cstdlib> // Required for strtol
#include <cerrno>  // Required for errno
#include <map>  // Required for errno

std::map<std::string, std::string> metadata;

void setBoundaryHeadMetadata(std::string subBuffer)
{
    size_t len = subBuffer.find_first_of(";\r");

    if (subBuffer[0] == '\n')
        return;
    size_t pos = subBuffer.find("=");
    if (pos > len)
    {
        std::cout << "throw1\n"; // throw
        return ;
    }
    std::string key = subBuffer.substr(1, pos - 1); // set 1 to skip `espace` " "
    size_t pos1 = subBuffer.find_first_of(";\r", pos);
	if (pos1 > len)
    {
        std::cout << "throw2\n"; // throw
        return ;
    }
    std::string value = subBuffer.substr(pos + 2, pos1 - pos - 3);
    metadata[key] = value;
	std::string param = subBuffer.substr(len+1);

	setBoundaryHeadMetadata(param);
}


int main()
{
	// std::string str = " name=\"username\"\r\n\"";
	std::string str = " name=\"file\"; filename=\"example.txt\"\r\n";
	setBoundaryHeadMetadata(str);

    std::cout << std::endl;
	for (auto it : metadata)
		std::cout << "|" << it.first << ":" << it.second << "|" << std::endl;


}
