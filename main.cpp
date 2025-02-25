#include "./server/WebServer.hpp"
#include "./conf/ServerConfig.hpp"
#include "./request/HttpRequest.hpp"
#include <system_error>

int main(int arc,char *arv[])
{
    if (!arv[1])
        return 0;
    string str = arv[1];
    WebServer server (str);
    // ServerConfig config(str);
    // config.dataConfigFile();
    while (true)
    {
        server.run();
        
        // server.displayAllClients();
        
        // std::map<int, httpRequest *> &map = server.getClients();
        // for (std::map<int, httpRequest *>::iterator it = map.begin(); it != map.end(); ++it)
        // {
        //     std::cout << "->>>key: " << it->first << std::endl;
        // }
    }
}
