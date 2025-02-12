#include "./server/WebServer.hpp"
#include "./request/httpRequest.hpp"

int main()
{
    WebServer server;
    while(true)
    {
        server.run();
        std::map<int, httpRequest *> &map = server.getClients();
        for (std::map<int, httpRequest *>::iterator it = map.begin(); it != map.end(); ++it)
        {
            std::cout << "->>>key: " << it->first << std::endl;
        }
    }
    
}