#pragma once
#include <sys/stat.h>
#include <dirent.h>
#include <cstring>
#include <fcntl.h>
#include <fstream>
#include <sstream>
#include <iostream>
#include <map>
#include <netinet/in.h>
#include <string>
#include <sys/socket.h>
#include <sys/types.h>
#include <unistd.h>
#include <vector>
#include "../request/HttpRequest.hpp"
using namespace std;
class HttpResponse
{
	private:
	public:
		HttpRequest *request;
	HttpResponse(HttpRequest *re);
	~HttpResponse();
	int writeData();
	void	notFound(int client_socket);

};


int checkTypePath(string &path);
bool ExistFile(string&filePath);
string autoindex(const string &dirPath);