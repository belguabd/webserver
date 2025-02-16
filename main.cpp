#include "./server/WebServer.hpp"
#include "./request/HttpRequest.hpp"
#include <system_error>

int main()
{
    WebServer server;
    while (true)
    {
        server.run();
    }
}
