#include <iostream>
#include <cstring>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <unistd.h>
#include <fcntl.h>
#include <vector>

class ClientConnection
{
private:
    int client_fd;
    std::vector<char> readBuffer;
    std::vector<char> writeBuffer;

public:
    ClientConnection(int client_fd);
    void readData();
    void writeData();
};