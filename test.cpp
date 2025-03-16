#include <string>
#include <map>
#include <iostream>

std::map<std::string, std::string> map;
int contentLength;

void addToMap(std::string filed)
{
    size_t pos = filed.find("="); 
    if (pos == std::string::npos)
    {
        std::cout << "invalid \n";
        return ;
    }
    std::string key = filed.substr(0, pos);
    std::string val = filed.substr(pos+ 1);
    map[key] =  val;
}

int handleKeyVal(std::string &buffer)
{
    size_t end = 0;
    size_t start = 0;
    end = buffer.find("&", end);
    while (end != std::string::npos)
    {
        std::string field = buffer.substr(start, end - start);
        addToMap(field);
        start = end + 1;
        end = buffer.find("&", start);
    }
    if (buffer.size() == contentLength)
    {
        std::cout << "this is the last\n";
        std::string field = buffer.substr(start, end - start);
        addToMap(field);
    }
    return 0;
}

int main()
{
    contentLength = 54;
    std::string buffer = "name=hassan&lastname=magueri&job=dev&language=c++;c;js";
    handleKeyVal(buffer);
    for (auto m : map)
    {
        std::cout << m.first << " : " << m.second << std::endl;
    }
}