#include "./server/WebServer.hpp"
#include "./request/httpRequest.hpp"

int main()
{
    WebServer server;
    while(true)
    {
        server.run();
        // for (std::map<int, httpRequest*>::const_iterator it = server.getConnectedClients().begin();it != server.getConnectedClients().end(); ++it)
        // {
        //     int client_id = it->first;
        //     // httpRequest *clientRequest = it->second;
        //     httpRequest clientRequest1 ;
        //     httpRequest *clientRequest = it->second;
        // //    clientRequest1.setclient(*clientRequest);

            // std::cout<<"--->"<<client_id <<std::endl;

        // }
        // std::cout <<"  ----> "<<httpRequest.client.getfd();
    }
    
}