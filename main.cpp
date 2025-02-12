#include "./server/WebServer.hpp"
#include "./request/httpRequest.hpp"

void    handleRequest(httpRequest &request)
{
    
}
int main()
{
    WebServer server;
    while(true)
    {
        server.run();
        for (std::map<int, httpRequest*>::const_iterator it = server.getConnectedClients() .begin();it != server.getConnectedClients().end(); ++it)
        {
            handleRequest(*it->second);
        //     // httpRequest *clientRequest = it->second;
        //     httpRequest clientRequest1 ;
        //     httpRequest *clientRequest = it->second;
        //     clientRequest1.setclient(*clientRequest);
        }
    }
}