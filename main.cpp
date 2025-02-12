#include "./server/WebServer.hpp"
#include "./request/HttpRequest.hpp"
#include <system_error>

int main()
{
    WebServer server;
    while(true)
    {
        server.run();
        // server.displayAllClients();
        for (HttpRequest *client : server.connected_clients)
        {
            client->display();
        }
    }
    
}