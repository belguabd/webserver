#include <iostream>
#include <fstream>
#include "Post.hpp"

std::string readFile(const std::string &filename)
{
    std::ifstream file(filename);
    std::string buffer;
    while(file)
    {
        std::string line;
        std::getline(file, line);
        buffer += line + "\n";
    }
    return buffer;
}

int main(int argc, char const *argv[])
{
    map <string, string> headers;

    headers["Transfer-Encoding"] = "chunked";
    headers["Content-Type"] = "application/octet-stream";
    headers["Content-Length"] = "472";

    
    string bufferBody = readFile("./files/test");
    Post post = Post(headers, bufferBody);
    std::cout << post.getBodyType() << std::endl;
    post.proseRequest();
    cout << "chyata ---------------------------\n";
    bufferBody = readFile("./files/test2");
    post.proseRequest();
    return 0;
}


// # Direct chunked transfer using curl
// curl -X POST \
//     -H "Transfer-Encoding: chunked" \
//     --data-binary @- \
//     "localhost:8080" << EOF
// This is chunk 1
// This is chunk 2
// This is chunk 3
// EOF