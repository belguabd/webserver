#include "HttpResponse.hpp"


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
void    sendResponse(HttpResponse &response)
{
  string str = "./doc/html/";
  int sig = response.request->sig;
  vector<std::string>  words = response.request->getDataFirstLine();
  cout << "--- >> " <<words[0]<<endl;
  cout <<"--- >> " <<words[1]<<endl;
  cout << "---- > "<<sig <<endl;
  if (words[1]!="/")
    str +=words[1];

  if (checkTypePath(str)==2) {
    string str1 =str;
    str1+="index.html";
    if (ExistFile(str1)==false) {
      status_line(response.request->getfd(),"HTTP/1.1 200 OK\r\n");
      headersSending(response.request->getfd());
      string body = autoindex(str);
      std::stringstream response1;
      response1 << "Content-Type: text/html\r\n"
              << "Content-Length: " << body.size() << "\r\n"
              << "Connection: close\r\n"
              << "\r\n"
              << body;
      std::string responseStr = response1.str();
      send(response.request->getfd(), responseStr.c_str(), responseStr.size(), 0);
      return ;
    }
  }
  // if ()
  // if (sig == 1)
  //   getResponse();
  // else if ()
  // postRseponse();
  // else if ()
  //   deleteResponse();
  string str2 = "index.html";
  str +=str2;
  std::ifstream file(str);
  std::stringstream fileContent;
        
    if (file) {
        fileContent << file.rdbuf(); 
        file.close();
    } else {
        response.notFound(response.request->getfd());
        return ;
    }
    status_line(response.request->getfd(),"HTTP/1.1 200 OK\r\n");
    headersSending(response.request->getfd());
    std::string body = fileContent.str();
    std::stringstream response1;
    response1 << "Content-Type: text/html\r\n"
              << "Content-Length: " << body.size() << "\r\n"
              << "Connection: close\r\n"
              << "\r\n"
              << body;
    std::string responseStr = response1.str();
    send(response.request->getfd(), responseStr.c_str(), responseStr.size(), 0);

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
  	std::ifstream file("/Users/ataoufik/Desktop/webserver/doc/error/404/404.html");
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



/*----------------------------------------------------------------*/

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

// string autoindex(string &dirPath) {
//   string str;
//   DIR *dir = opendir(dirPath.c_str());
//   struct dirent* entry;
//   while ((entry = readdir(dir))!=nullptr) {
//     if (strcmp(entry->d_name,".")!= 0 && strcmp(entry->d_name,"..")!= 0) {
//       str += entry->d_name;
//       str += "\n";
//       cout <<str;
//     }
//   }
//   closedir(dir);
//   return str;
// }

string autoindex(const string &dirPath) {
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

    struct dirent* entry;
    while ((entry = readdir(dir)) != nullptr) {
        if (strcmp(entry->d_name, ".") != 0 && strcmp(entry->d_name, "..") != 0) {
            html += "<li><a href=\"" + dirPath + "/" + entry->d_name + "\">";
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
    cout <<"paht =  "<<path<<endl;
    if (stat(path.c_str(), &pathInfo) != 0) {
        std::perror("statz");
        return 0;
    }

    if (S_ISREG(pathInfo.st_mode)) {
        std::cout << path<< " is a file.\n";
        return 1;
    } else if (S_ISDIR(pathInfo.st_mode)) {
        std::cout << path<< " is a directory.\n";
        return 2;
    } 
    return 0;
}