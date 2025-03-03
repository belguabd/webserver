#include "HttpResponse.hpp"

string str = "./doc/html";
void	status_line(int client_socket,string status) {
	// std::string status;
	  // status = "HTTP/1.1 200 OK\r\n";
	send(client_socket, status.c_str(), status.size(), 0);
}

void	headersSending(int client_socket) {
	std::string header;
	time_t now = time(0);  
    struct tm tstruct = *localtime(&now);
    char buffer[80];
    strftime(buffer, sizeof(buffer), "%a %d %b %Y %H:%M:%S GMT\r\n", &tstruct);
	header += "Date: ";
	header += buffer;
	header += "Server: MyServer(verison beta)\r\n";
	// std::cout << header;
	send(client_socket, header.c_str(), header.size(), 0);
}
/*---------------------- Get method------------------------------------------*/
void HttpResponse::fileDataSend(string &data)
{
  size_t pos = data.find(".py"); // depanding config file
  if (pos!=string ::npos) {
    // cgi handle with GET method
    return ;
  }
  ifstream file(data);
  stringstream fileContent;
  fileContent << file.rdbuf();
  if (file) {
      fileContent << file.rdbuf(); 
      file.close();
  } else {
      this->notFound(this->request->getfd());
      return ;
  }
  status_line(this->request->getfd(),"HTTP/1.1 200 OK\r\n");
  headersSending(this->request->getfd());
  string body = fileContent.str();
  stringstream response1;
  response1 << "Content-Type: text/html\r\n"
            << "Content-Length: " << body.size() << "\r\n"
            << "Connection: close\r\n"
            << "\r\n"
             << body;
  string responseStr = response1.str();
  send(this->request->getfd(), responseStr.c_str(), responseStr.size(), 0);

}
void HttpResponse:: forbidden(int client_socket)
{
  ifstream file("./doc/error/4xx/403.html");
  stringstream fileContent;

  if (file) {
      fileContent << file.rdbuf(); 
      file.close();
  }
  status_line(client_socket,"HTTP/1.1 403 FORBIDDEN\r\n");
  headersSending(client_socket);
  string body = fileContent.str();
  stringstream response1;
  response1 << "Content-Type: text/html\r\n"
            << "Content-Length: " << body.size() << "\r\n"
            << "Connection: close\r\n"
            << "\r\n"
            << body;
  string responseStr = response1.str();
  send(client_socket, responseStr.c_str(), responseStr.size(), 0);
}

void  HttpResponse::dirDataSend(string &data)
{
  // index names depend on the configuration file. I use a simple index called 'dir.html.' and same autoindex i use off
  srand(static_cast<unsigned int>(time(0)));
  bool autoindex = rand() % 2;

  // bool autoindex =true;
  // cout <<"after data ==  > " <<data<<endl;
  string indexFile = "dir.html";
  string str = data;
  str +=indexFile;
  // cout <<"after str ==  > " <<data<<endl;
  if (ExistFile(str)==true) {
    this->fileDataSend(str);
  } else {
    if (autoindex ==false)
    {
      this->forbidden(this->request->getfd());
    } else {
      status_line(this->request->getfd(),"HTTP/1.1 200 OK\r\n");
      headersSending(this->request->getfd());
      string body = dirAutoindex(data);
      stringstream response1;
          response1 << "Content-Type: text/html\r\n"
            << "Content-Length: " << body.size() << "\r\n"
            << "Connection: close\r\n"
            << "\r\n"
            << body;
      string responseStr = response1.str();
      send(this->request->getfd(), responseStr.c_str(), responseStr.size(), 0);
    }
  }
}
void    HttpResponse::getResponse()
{
  string data = str;
  vector<std::string>  words = this->request->getDataFirstLine();
  cout<<"data = > " << words[1] <<endl;
  if (words[1]=="/")
  {
    this->defautlRoot();
    return;
  }
    data+=words[1];
  if (checkTypePath(data)==0) {
    this->notFound(this->request->getfd());
    return;
  } else if (checkTypePath(data)==1) {
    this->fileDataSend(data);
  } else if (checkTypePath(data)==2) {
    this->dirDataSend(data);
  }


}
void HttpResponse::defautlRoot()
{
  string str2 = "/index.html";
  string data = str ; 
  data +=str2;
  ifstream file(data);
  stringstream fileContent;
  fileContent << file.rdbuf();
  if (file) {
      fileContent << file.rdbuf(); 
      file.close();
  } else {
      this->notFound(this->request->getfd());
      return ;
  }
  status_line(this->request->getfd(),"HTTP/1.1 200 OK\r\n");
  headersSending(this->request->getfd());
  string body = fileContent.str();
  stringstream response1;
  response1 << "Content-Type: text/html\r\n"
            << "Content-Length: " << body.size() << "\r\n"
            << "Connection: close\r\n"
            << "\r\n"
             << body;
  string responseStr = response1.str();
  send(this->request->getfd(), responseStr.c_str(), responseStr.size(), 0);
}

void HttpResponse:: checkDataResev()
{
  
}
void    sendResponse(HttpResponse &response)
{
  int sig = response.request->sig;
  response.checkDataResev();
  if (sig == 1) {
    response.getResponse();
  } else if (sig == 2) {
    response.postResponse();
  }

}

HttpResponse::HttpResponse(HttpRequest *re) :request(re) {

}

HttpResponse:: ~HttpResponse() { }


int HttpResponse::writeData() {
    int bytes_send = 0;
    sendResponse(*this);
//   const char *msg = "Hi I am server";
//   ssize_t bytes_send = send(client_fd, msg, strlen(msg), 0);
//   if (bytes_send == -1)
//     std::cerr << "Error sending message to client" << std::endl;
  return bytes_send;
}


void HttpResponse::notFound(int client_socket) {
  	std::ifstream file("./doc/error/4xx/404.html");
    std::stringstream fileContent;
        
    if (file) {
        fileContent << file.rdbuf(); 
        file.close();
    }
    status_line(client_socket,"HTTP/1.1 404 NOT FOUND\r\n");
    headersSending(client_socket);
    std::string body = fileContent.str();
    std::stringstream response1;
    response1 << "Content-Type: text/html\r\n"
              << "Content-Length: " << body.size() << "\r\n"
              << "Connection: close\r\n"
              << "\r\n"
              << body;
    std::string responseStr = response1.str();
    send(client_socket, responseStr.c_str(), responseStr.size(), 0);
}

/*--------------------------------------------------------------------------------------------*/

/*---------------------- fanction extern class used------------------------------------------*/

bool ExistFile(string&filePath) {
  struct stat infoFile;
  if (stat(filePath.c_str(),&infoFile) != 0) {
        std::perror("stat");
        return false;
  }
  if (S_ISREG(infoFile.st_mode)) {
    return true;
  }
  return false;
}

string dirAutoindex(string &dirPath) {
  cout <<"dirPath ---------  ==  > " << dirPath<<endl;
    string html = "<!DOCTYPE html>\n"
                  "<html>\n"
                  "<head>\n"
                    "<meta charset=\"UTF-8\">\n"
                    "<title>Autoindex</title>\n"
                    "<style>\n"
                      "body { font-family: Arial, sans-serif; margin: 20px; }\n"
                      "h1 { font-size: 24px; }\n"
                      "ul { list-style-type: none; padding: 0; }\n"
                      "li { margin: 5px 0; }\n"
                      "a { text-decoration: none; color: #3498db; }\n"
                      "a:hover { text-decoration: underline; }\n"
                    "</style>\n"
                  "</head>\n"
                  "<body>\n"
                  "<h1>Index of /</h1>\n";

    DIR *dir = opendir(dirPath.c_str());
    if (!dir) {
        cerr << "Error: Unable to open directory " << dirPath << "\n";
        return "<h1>Directory not found</h1>";
    }
  string str = dirPath.substr(10);
    struct dirent* entry;
    while ((entry = readdir(dir)) != nullptr) {
        if (strcmp(entry->d_name, ".") != 0 && strcmp(entry->d_name, "..") != 0) {
            html += "<li><a href=\"" + str + "/" + entry->d_name + "\">";
            html += entry->d_name;
            html += "</a></li>\n";
        }
    }
    closedir(dir);

    html += "</ul>\n</body>\n</html>";
    return html;
}

int checkTypePath(string &path) {
  struct stat pathInfo;
    if (stat(path.c_str(), &pathInfo) != 0) {
        std::perror("stat");
        return 0;
    }

    if (S_ISREG(pathInfo.st_mode)) {
        // std::cout << path<< " is a file.\n";
        return 1;
    } else if (S_ISDIR(pathInfo.st_mode)) {
        // std::cout << path<< " is a directory.\n";
        return 2;
    } 
    return 0;
}

/*-------------------------------------------------------------------------------------------*/


/*--------------------------------------Post method------------------------------------------*/

void HttpResponse::postResponse()
{
  vector<string>  words = this->request->getDataFirstLine();
  cout <<"URL = "<<words[1]<<endl;
  //check if location upload is allowed
  //check type of resource (cgi , html,.. )
  // uploads success or other 
  // if ("/tmpbran/upload")// upload_store if path not support upload 
  // {
  //   this->forbidden(this->request->getfd());
  //   cout <<"hi"<<endl;
  //   return ;

  // }
  size_t pos = words[1].find(".py"); // depanding config file
  if (pos!=string ::npos) {
    // cgi handle with Post method
    return ;
  }
  ifstream file("./doc/html/Upload_/succ.html");
  stringstream fileContent;
        
  if (file) {
      fileContent << file.rdbuf(); 
      file.close();
  }
  status_line(this->request->getfd(),"HTTP/1.1 201 CREATED\r\n");
  headersSending(this->request->getfd());
  string body = fileContent.str();
  stringstream response1;
  response1 << "Content-Type: text/html\r\n"
            << "Content-Length: " << body.size() << "\r\n"
            << "Connection: close\r\n"
            << "\r\n"
            << body;
  string responseStr = response1.str();
  send(this->request->getfd(), responseStr.c_str(), responseStr.size(), 0);
}



/*--------------------------------------------------------------------------------------------*/