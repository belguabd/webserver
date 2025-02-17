#include <iostream>
#include <fstream>
#include <sstream>
#include "Post.hpp"

using namespace std;

std::string readFile(const std::string &filename)
{
    std::ifstream file(filename);
    if (!file)
    {
        std::cerr << "Error: Unable to open file " << filename << std::endl;
        exit(1);
    }

    std::ostringstream buffer;
    buffer << file.rdbuf();  // Read entire file content including newlines

    return buffer.str();
}
int main(int argc, char const *argv[])
{
    map<string, string> headers;

    headers["Transfer-Encoding"] = "chunked";
    headers["Content-Type"] = "application/octet-stream";
    headers["Content-Length"] = "472";

    string bufferBody = readFile("/Users/emagueri/Desktop/download.jpg");
    Post post = Post(headers);
    post.proseRequest(bufferBody);

    // string bufferBody = readFile("./files/test1");
    // Post post = Post(headers);
    // post.proseRequest(bufferBody);
    // cout << "chyata ---------------------------\n";
    // bufferBody = readFile("./files/test2");
    // cout << "file:\n" << bufferBody << "\n";
    // post.proseRequest(bufferBody);

    // Post post = Post(headers);
    // std::string bufferBody = readFile("./files/newLine");
    // std::cout << "file:\n"
    //           << bufferBody << "\n=========\n"
    //           << std::endl;
    // post.proseRequest(bufferBody);
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