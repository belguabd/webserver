#include "./server/WebServer.hpp"
#include "./request/HttpRequest.hpp"
#include "./request/Post/Post.hpp"
#include <system_error>

void makeFileEmpty(const std::string filename) {
    std::ofstream file(filename, std::ios::trunc);
    if (file.is_open()) {
        std::cout << "File " << filename << " has been emptied." << std::endl;
        file.close();
    } else {
        std::cerr << "Unable to open file: " << filename << std::endl;
    }
}

int main()
{
    makeFileEmpty(CURREQ);
    WebServer server;
    while (true)
    {
        server.run();
    }
}
