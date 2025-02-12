#include "./server/WebServer.hpp"
#include "./request/HttpRequest.hpp"
#include <system_error>

int main()
{
    // std::string str = "hello";

    // std::cout << str.substr(0) << "\n";
    WebServer server;
    while (true)
    {
        server.run();
    }
}
