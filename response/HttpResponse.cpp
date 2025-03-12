#include "HttpResponse.hpp"

void	status_line(int client_socket,string status) {
  // cout <<status;
	send(client_socket, status.c_str(), status.size(), 0);
}

void	headersSending(int client_socket,string serverName) {
	std::string header;
	time_t now = time(0);  
    struct tm tstruct = *localtime(&now);
    char buffer[80];
    strftime(buffer, sizeof(buffer), "%a %d %b %Y %H:%M:%S GMT\r\n", &tstruct);
	header += "Date: ";
	header += buffer;
  if (!serverName.empty()) {
    header += "Server: ";
    header +=serverName;
    header +="\r\n";
  }
	// std::cout << header;
	send(client_socket, header.c_str(), header.size(), 0);
}
/*---------------------- Get method------------------------------------------*/
void HttpResponse::fileDataSend(string &data,ServerConfig &config)
{

  ifstream file(data);
  stringstream fileContent;
  fileContent << file.rdbuf();
  if (file) {
      fileContent << file.rdbuf(); 
      file.close();
  }
  status_line(this->request->getfd(),"HTTP/1.1 200 OK\r\n");
  headersSending(this->request->getfd(),config.getServerName());
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
void HttpResponse:: forbidden(int client_socket,ServerConfig &config)
{
  string body;
  string val;
  string root = config.getRoot();
  stringstream fileContent;
    map<string, string>::const_iterator it;
    for (it = config.errorpage.begin(); it != config.errorpage.end(); ++it) {
        if (it->first.find("403")!=string::npos) {
          val = it->second;
          break;
        }
    }
    root += val;
  	ifstream file(root);
  if (file) {
      fileContent << file.rdbuf();
      body = fileContent.str();
      file.close();
  } else {
      body =  "<!DOCTYPE html>\n"
              "<html lang=\"en\">\n"
              "<head>\n"
              "<meta charset=\"UTF-8\">\n"
              " <meta name=\"viewport\" content=\"width=device-width, initial-scale=1.0\">\n"
              "<title>403 - Forbidden</title>\n"
              "</head> \n"
              "<body>\n"
              "<h1>403 Forbidden</h1>\n"
              " <p>You do not have permission to access this page.</p>\n"
              "</body>\n</html>";
  }
  status_line(client_socket,"HTTP/1.1 403 FORBIDDEN\r\n");
  headersSending(client_socket,config.getServerName());
  stringstream response1;
  response1 << "Content-Type: text/html\r\n"
            << "Content-Length: " << body.size() << "\r\n"
            << "Connection: close\r\n"
            << "\r\n"
            << body;
  string responseStr = response1.str();
  send(client_socket, responseStr.c_str(), responseStr.size(), 0);
}
int  HttpResponse::checkFileAndSendData(string &data ,ServerConfig &config,string &index)
{
  string str;
  size_t i = 0;
  vector<string> words;
  words = splitstring(index);
  bool exist = false;
  while (data[data.length()-1]== '/' && i < words.size())
  {
    str = data;
    str += words[i];
    if (ExistFile(str) == true) {
        exist = true;
      this->fileDataSend(str,config);
      break;
    }
    i++;
  }
  if (data[data.length()-1] == '/' && exist == false) {
    return 1;
  }
  return 0;
}

void  HttpResponse::dirDataSend(string &data, string &root,LocationUplaods &upload, ServerConfig &config)
{
  int checkExist;
  if (!upload.index.empty()) {
    checkExist = this->checkFileAndSendData(data, config, upload.index);
  } else {
    string index;
    index = config.getIndex();
    checkExist = this->checkFileAndSendData(data, config, index);
  }
  if (checkExist == 1) {
    this->notFound(this->request->getfd(),config);
    return;
  }
  this->forbidden(this->request->getfd(),config);
}

void  HttpResponse::dirDataSend(string &data, string &root,LocationConfig &normal, ServerConfig &config)
{
  int checkExist;
  if (!normal.index.empty()) {
    checkExist = this->checkFileAndSendData(data, config, normal.index);
  } else {
    string index;
    index = config.getIndex();
    checkExist = this->checkFileAndSendData(data, config, index);
  }
  if (checkExist == 1) {
    this->notFound(this->request->getfd(),config);
    return;
  }
  if (normal.autoindex == false)
  {
      this->forbidden(this->request->getfd(),config);
  } else {
      status_line(this->request->getfd(),"HTTP/1.1 200 OK\r\n");
      headersSending(this->request->getfd(),config.getServerName());
      string body = dirAutoindex(data,root);
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

void    HttpResponse:: getLocationResponse(LocationConfig &normal,string &str,ServerConfig &config)
{
  string data;
  string root;
  int flag;
  if (normal.root.empty()) {
    root = config.getRoot();
    data = root;
    flag = 1;
  } else {
    data = normal.root;
    flag = 2;
  }
  data += str;
  if (checkTypePath(data)==0) {
    this->notFound(this->request->getfd(),config);
  } else if (checkTypePath(data)==1) {
    this->fileDataSend(data,config);

  } else if (checkTypePath(data)==2) {
    if (flag==1)
      this->dirDataSend(data,root,normal,config);
    else
      this->dirDataSend(data,normal.root,normal,config);
  }
}
void    HttpResponse:: getLocationResponse(LocationUplaods &upload,string &str,ServerConfig &config)
{
  string data;
  string root;
  int flag;
  if (upload.root.empty()) {
    root = config.getRoot();
    data = root;
    flag = 1;
  } else {
    data = upload.root;
    flag = 2;
  }
  data += str;
  if (checkTypePath(data)==0) {
    this->notFound(this->request->getfd(),config);
  } else if (checkTypePath(data)==1) {
    this->fileDataSend(data,config);

  } else if (checkTypePath(data)==2) {
    if (flag==1)
      this->dirDataSend(data,root,upload,config);
    else
      this->dirDataSend(data,upload.root,upload,config);
  }
}
void HttpResponse::redirectionResponse(string &str)
{
  ServerConfig config;
  string data;
  config = this->request->getServerConfig();
  if (config.typeUrl == 1) {
    if(this->request->mapheaders.find("Host") != this->request->mapheaders.end()) {
      string host = getValueFromMap(config.configRedirection,config.configRedirection.find("Host"));
      data = host;
      data += str;
  }
  } else {
    data = str;
  }
  status_line(this->request->getfd(),"HTTP/1.1 301 Moved Permanently\r\n");
  headersSending(this->request->getfd(),config.getServerName());
  stringstream response1;
  response1 << "Location:"<<data<< "\r\n\r\n";
  string responseStr = response1.str();
  send(this->request->getfd(), responseStr.c_str(), responseStr.size(), 0);
}
int indexValidPath(string str)
{
  int i = 0;
  if (str.length()==1)
    return 0;
  i++;
  while (str[i] && str[i]!='/')
    i++;
  return (i);
}
void    HttpResponse::getResponse()
{
  string data;
  string str;
  ServerConfig config;
  config = this->request->getServerConf();
  vector<string>  words = this->request->getDataFirstLine();
  int i = indexValidPath(words[1]);
  if (i != 0 && words[1][i]==0) {
    words[1]+="/";
  }
  str = words[1].substr(0,i);
  data = words[1].substr(i);
  if(config.configRedirection.find(str) != config.configRedirection.end()) {
    string urlRedirection = getValueFromMap(config.configRedirection,config.configRedirection.find(str));
    redirectionResponse(urlRedirection);
    // cout <<"str --> "<<str1<<endl;
  }
  if (config.configNormal.find(str) != config.configNormal.end()) {
    LocationConfig log = getValueFromMap(config.configNormal,config.configNormal.find(str));
    getLocationResponse(log,data,config);
  }
  if (config.configUpload.find(str) != config.configUpload.end()) {
    LocationUplaods log = getValueFromMap(config.configUpload,config.configUpload.find(str));
    getLocationResponse(log,data,config);
  }

  // cout<<"data = > " << words[1] <<endl;
  if (words[1]=="/")
  {
    this->defautlRoot(config);
    return;
  }
  // if (checkTypePath(data)==0) {
  //   this->notFound(this->request->getfd());
  //   return;
  // } else if (checkTypePath(data)==1) {
  //   this->fileDataSend(data);
  // } else if (checkTypePath(data)==2) {
  //   this->dirDataSend(data);


}
void HttpResponse::defautlRoot(ServerConfig &config)
{
  vector<string> words;
  string root  = config.getRoot();
  stringstream fileContent;
  size_t i = 0;
  words = splitstring(config.getIndex());
  bool exist = false;
  while (i < words.size())
  {
    root  = config.getRoot();
    root += "/";
    root += words[i];
    if (ExistFile(root)==true) {
        exist = true;
      break;
    }
    i++;
  }
  if (exist == true) {
    ifstream file(root);
    fileContent << file.rdbuf();
    file.close();
  }
  else {
    this->notFound(this->request->getfd(),config);
    return ;
  }
  status_line(this->request->getfd(),"HTTP/1.1 200 OK\r\n");
  headersSending(this->request->getfd(),config.getServerName());
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

int HttpResponse:: checkDataResev()
{
  ServerConfig config = this->request->getServerConfig();
  int statuscode = this->request->getRequestStatus();
  if (statuscode == 400) {
    this->badRequest(this->request->getfd(),config);
    return 1;
  } else if (statuscode ==505) {
    this->HttpVersionNotSupported(this->request->getfd(),config);
    return 1;
  }
  return 0;
}
void    sendResponse(HttpResponse &response)
{
  int method = response.request->_method;
  if (response.checkDataResev()!=0) {
    return ;
  }
  // cout << response.request->filename << "\n";
  if (method == GET) {
    response.getResponse();
  }
  else if (method == POST) {
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
void HttpResponse::HttpVersionNotSupported(int client_socket,ServerConfig &config) {
    string body;
    string val;
    string root = config.getRoot();
    stringstream fileContent;
    map<string, string>::const_iterator it;
    for (it = config.errorpage.begin(); it != config.errorpage.end(); ++it) {
        if (it->first.find("505")!=string::npos) {
          val = it->second;
          break;
        }
    }
    root += val;
  	ifstream file(root);
    if (file) {
        fileContent << file.rdbuf(); 
        body = fileContent.str();
        file.close();
    } else {
      body =  "<!DOCTYPE html>\n"
              "<html lang=\"en\">\n"
              "<head>\n"
              "<meta charset=\"UTF-8\">\n"
              " <meta name=\"viewport\" content=\"width=device-width, initial-scale=1.0\">\n"
              "<title>505 - HTTP Version not supported</title>\n"
              "</head> \n"
              "<body>\n"
              "<h1>505 HTTP Version Not Supported</h1>\n"
              "<p>The page you are looking for does not exist.</p>\n"
              "</body>\n</html>";
    }
    status_line(client_socket,"HTTP/1.1 505 HTTP Version Not Supported\r\n");
    headersSending(client_socket,config.getServerName());
    stringstream response1;
    response1 << "Content-Type: text/html\r\n"
              << "Content-Length: " << body.size() << "\r\n"
              << "Connection: close\r\n"
              << "\r\n"
              << body;
    string responseStr = response1.str();
    send(client_socket, responseStr.c_str(), responseStr.size(), 0);
}
void HttpResponse::badRequest(int client_socket,ServerConfig &config) {
    string body;
    string val;
    string root = config.getRoot();
    stringstream fileContent;
    map<string, string>::const_iterator it;
    for (it = config.errorpage.begin(); it != config.errorpage.end(); ++it) {
        if (it->first.find("400")!=string::npos) {
          val = it->second;
          break;
        }
    }
    root += val;
  	ifstream file(root);
    if (file) {
        fileContent << file.rdbuf(); 
        body = fileContent.str();
        file.close();
    } else {
      body =  "<!DOCTYPE html>\n"
              "<html lang=\"en\">\n"
              "<head>\n"
              "<meta charset=\"UTF-8\">\n"
              " <meta name=\"viewport\" content=\"width=device-width, initial-scale=1.0\">\n"
              "<title>400 - Bad Request</title>\n"
              "</head> \n"
              "<body>\n"
              "<h1>404 Bad Request</h1>\n"
              "</body>\n</html>";
    }
    status_line(client_socket,"HTTP/1.1 400 Bad Request\r\n");
    headersSending(client_socket,config.getServerName());
    stringstream response1;
    response1 << "Content-Type: text/html\r\n"
              << "Content-Length: " << body.size() << "\r\n"
              << "Connection: close\r\n"
              << "\r\n"
              << body;
    string responseStr = response1.str();
    send(client_socket, responseStr.c_str(), responseStr.size(), 0);
}

void HttpResponse::notFound(int client_socket,ServerConfig &config) {
    string body;
    string val;
    string root = config.getRoot();
    stringstream fileContent;
    map<string, string>::const_iterator it;
    for (it = config.errorpage.begin(); it != config.errorpage.end(); ++it) {
        if (it->first.find("404")!=string::npos) {
          val = it->second;
          break;
        }
    }
    root += val;
  	ifstream file(root);
    if (file) {
        fileContent << file.rdbuf(); 
        body = fileContent.str();
        file.close();
    } else {
      body =  "<!DOCTYPE html>\n"
              "<html lang=\"en\">\n"
              "<head>\n"
              "<meta charset=\"UTF-8\">\n"
              " <meta name=\"viewport\" content=\"width=device-width, initial-scale=1.0\">\n"
              "<title>404 - Page Not Found</title>\n"
              "</head> \n"
              "<body>\n"
              "<h1>404 Not Found</h1>\n"
              "<p>The page you are looking for does not exist.</p>\n"
              "</body>\n</html>";
    }
    status_line(client_socket,"HTTP/1.1 404 Not Found\r\n");
    headersSending(client_socket,config.getServerName());
    stringstream response1;
    response1 << "Content-Type: text/html\r\n"
              << "Content-Length: " << body.size() << "\r\n"
              << "Connection: close\r\n"
              << "\r\n"
              << body;
    string responseStr = response1.str();
    send(client_socket, responseStr.c_str(), responseStr.size(), 0);
}

/*--------------------------------------------------------------------------------------------*/

/*---------------------- fanction extern class used------------------------------------------*/

bool ExistFile(string&filePath) {
  struct stat infoFile;
  if (stat(filePath.c_str(),&infoFile) != 0) {
        return false;
  }
  if (S_ISREG(infoFile.st_mode)) {
    return true;
  }
  return false;
}

string dirAutoindex(string &dirPath,string &root) {

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
  string str = dirPath.substr(root.length() + 1);
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
  ServerConfig config;
  string body;
  config = this->request->getServerConf();
  vector<string>  words = this->request->getDataFirstLine();
  cout <<"URL = "<<words[1]<<endl;
  ifstream file("./doc/html/Upload_/succ.html"); ///
  stringstream fileContent;
        
  if (file) {
      fileContent << file.rdbuf();
      body = fileContent.str();
      file.close();
  } else {
      body =  "<!DOCTYPE html>\n"
              "<html lang=\"en\">\n"
              "<head>\n"
              "<meta charset=\"UTF-8\">\n"
              "<meta name=\"viewport\" content=\"width=device-width, initial-scale=1.0\">\n"
              "<title>Upload Successful</title>\n"
              "</head>\n"
              "<body>\n"
              "<h1>Upload Successful</h1>\n"
              "<p>Your file has been uploaded successfully.</p>\n"
              "<p><a href=\"/\">Return to Home</a></p>\n"
              "</body>\n"
              "</html>\n";

  }
  status_line(this->request->getfd(),"HTTP/1.1 201 CREATED\r\n");
  headersSending(this->request->getfd(),config.getServerName());
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






// string generateErrorPage(int statusCode, const string& message) {
//     return "<!DOCTYPE html>\n"
//            "<html lang=\"en\">\n"
//            "<head>\n"
//            "<meta charset=\"UTF-8\">\n"
//            "<meta name=\"viewport\" content=\"width=device-width, initial-scale=1.0\">\n"
//            "<title>" + to_string(statusCode) + " - " + message + "</title>\n"
//            "<style>\n"
//            "body { font-family: Arial, sans-serif; text-align: center; padding: 50px; }\n"
//            "h1 { font-size: 50px; color: red; }\n"
//            "p { font-size: 20px; color: #666; }\n"
//            "</style>\n"
//            "</head>\n"
//            "<body>\n"
//            "<h1>" + to_string(statusCode) + " " + message + "</h1>\n"
//            "<p>The request could not be processed.</p>\n"
//            "</body>\n"
//            "</html>";
// }