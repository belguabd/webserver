#include "HttpResponse.hpp"
#include <cstddef>
#include <sys/_types/_ssize_t.h>

string statusText(int status) {
  string text;
  if (status == 200) { // 200 400 403 404 405 413 414 500 501 505  201 CREATED
    text = "200 OK\r\n";
  } else if (status == 201) {
    text = "201 CREATED\r\n";
  }else if (status==204){
    text = "204 No Content\r\n";
  } else if(status==400) {
    text ="400 Bad Request\r\n"; 
  }  else if(status==403) {
    text = "403 Forbidden\r\n";
  } else if (status == 404) {
    text = "404 Not Found\r\n";
  } else if (status == 405) {
    text = "405 Method Not Allowed\r\n";
  } else if (status==413){
    text = "413 Request Entity Too Large\r\n";
  } else if(status==500) {
    text = "500 Internal Server Error\r\n";///
  } else if(status==504) {
    text = "504 Gateway Timeout\r\n";///
  } else if (status==505){
    text = "505 HTTP Version Not Supported\r\n";
  } else if (status == 301) {
    text = "301 Moved Permanently\r\n";
  }
  return text;
}
string status_line(int client_socket, int status) {
  string firstline = "HTTP/1.1 ";
  firstline += statusText(status);
  return firstline;
}

string headersSending(int client_socket, string serverName) {
  std::string header;
  time_t now = time(0);
  struct tm tstruct = *localtime(&now);
  char buffer[80];
  strftime(buffer, sizeof(buffer), "%a %d %b %Y %H:%M:%S GMT\r\n", &tstruct);
  header += "Date: ";
  header += buffer;
  return header;
}
/*---------------------- Get method------------------------------------------*/
void HttpResponse::fileDataSend(std::string &data, ServerConfig &config) {
    string ContentType;
    if (!this->file.is_open()) {
        this->file.open(data, ios::binary);
        if (!this->file.is_open()) {
            cerr << "Error opening file: " << data << endl;
            return;
        }

        if (firstTimeResponse == 0) {
            file_size = 0;
            this->file.seekg(0, ios::end);
            file_size = this->file.tellg();
            this->file.seekg(0, ios::beg);
            size_t pos = data.find(".");
            if (pos == string::npos) {
                ContentType = "text/plain";
            }
            else {
                string extension = data.substr(pos);
                ContentType = getMimeType(extension);
            }
            ostringstream response_headers;
            response_headers << status_line(this->request->getfd(), this->request->getRequestStatus());
            response_headers << headersSending(this->request->getfd(), config.getServerName());
            response_headers << "Content-Type: " << ContentType << "\r\n"
                             << "Content-Length: " << file_size << "\r\n"
                             << "Connection: " << this->request->typeConnection << "\r\n"
                             << "\r\n";
            cout <<"              path : " << strLocation<<"                                                 status code :"<<this->request->getRequestStatus()<<endl;
            this->bytesSend = send(this->request->getfd(), response_headers.str().c_str(), response_headers.str().size(), 0);
            firstTimeResponse = 1;
        }
    }

    this->file.seekg(this->file_offset, ios::beg);
    const size_t buffer_size = 1024;
    char buffer[buffer_size];
    this->file.read(buffer, buffer_size);
    streamsize bytes_read = this->file.gcount();
    if (bytes_read > 0) {
        this->bytesSend = send(this->request->getfd(), buffer, bytes_read, 0);
        this->file_offset += bytes_read;
    }

    if (bytes_read == 0) {
        this->file.close();
        complete = 1;
    }
}

int HttpResponse::checkFileAndSendData(string &data, ServerConfig &config, string &index) {
  string str;
  size_t i = 0;
  if (data[data.length() - 1] != '/') {
    data +="/";
  }
  vector<string> words;
  words = splitstring(index);
  while (data[data.length() - 1] == '/' && i < words.size()) {
    if (words[i].find(".php")!=string::npos || words[i].find(".py")!=string::npos) {
      i++;
      continue;
    }
    str = data;
    str += words[i];
    if (ExistFile(str) == true) {
      this->fileDataSend(str, config);
     return 0;
    }
    i++;
  }
  return 1;
}


void HttpResponse::dirDataSend(string &data, string &root, LocationConfig &normal, ServerConfig &config) {
    
  if (!normal._index.empty()) {
    if (this->checkFileAndSendData(data, config, normal._index)==1) {
      this->sendErrorPage(config, 404);
    }
  } else {
    if (normal._autoindex == false) {
      this->sendErrorPage(config, 403);
    } else {
        string body = dirAutoindex(this->strLocation,data,root);
        stringstream response1;
        response1 <<status_line(this->request->getfd(),200);
        response1 <<headersSending(this->request->getfd(),config.getServerName());
        response1 << "Content-Type: text/html\r\n"
              << "Content-Length: " << body.size() << "\r\n"
              << "Connection: close\r\n"
              << "\r\n"
              << body;
        string responseStr = response1.str();
        this->bytesSend = send(this->request->getfd(), responseStr.c_str(), responseStr.size(), 0);
        this->complete =1;
      }
  }
}
void HttpResponse::dirDataSend(string &data, ServerConfig &config) {
  int checkExist;
  string index;
  string root = config.getRoot();
  index = config.getIndex();
  checkExist = this->checkFileAndSendData(data, config, index);
  if (checkExist == 1) {
    this->sendErrorPage(config, 404);
    return;
  }
  if (config.getAutoindex() == false) {
    this->sendErrorPage(config, 403);
  } else {
    string body = dirAutoindex(this->strLocation,data, root);
    stringstream response1;
    response1 << status_line(this->request->getfd(), 200);
    response1 << headersSending(this->request->getfd(), config.getServerName());
    response1 << "Content-Type: text/html\r\n"
              << "Content-Length: " << body.size() << "\r\n"
              << "Connection: close\r\n"
              << "\r\n"
              << body;
    string responseStr = response1.str();
    this->bytesSend = send(this->request->getfd(), responseStr.c_str(), responseStr.size(), 0);
  }
}

void HttpResponse::getLocationResponse(LocationConfig &normal, string &str, ServerConfig &config) {
    string data;
    string root;
    int typePath;
    if (normal._root.empty()) {
        root = config.getRoot();
        data = root;
    } else {
        root = normal._root;
        data = root;
    }
    if (!root.empty() && root.back() != '/') {
        root += '/';
    }
    data = root;
    if (!str.empty()) {
        if (str[0] == '/') {
            str.erase(0, 1);
        }
        data += str;
    }
    // cout << "Final path being checked: " << data << endl;
    typePath = checkTypePath(data);
    if (typePath == 0) {
        this->sendErrorPage(config, 404);
    } 
    else if (typePath == 1) {
        if (data.find(".php") != string::npos || data.find(".py") != string::npos) {
            this->sendErrorPage(config, 403);
        } else {
            this->fileDataSend(data, config);
        }
    } 
    else if (typePath == 2) {
        if (data.back() != '/') {
            data += '/';
        }
        // if (!normal._index.empty()) {
        //     string index_path = data + normal._index;
        //     if (checkTypePath(index_path) == 1) {
        //         this->fileDataSend(index_path, config);
        //         return ;
        //     }
        // }
        this->dirDataSend(data, root, normal, config);
    }
}
void HttpResponse::redirectionResponse(string &str,ServerConfig &config) {
  string data;
  config = this->request->getServerConfig();
  if (config.typeUrl == 1) {
    if (this->request->mapheaders.find("Host") != this->request->mapheaders.end()) { // Host HOST
      string host = this->request->mapheaders["Host"];
      data = host;
      data += str;
    }
  } else {
    data = str;
  }
  
  stringstream response1;
  response1 << status_line(this->request->getfd(), 301);
  response1 << headersSending(this->request->getfd(), config.getServerName());
  response1 << "Location:" << data << "\r\n\r\n";
  string responseStr = response1.str();
  this->bytesSend = send(this->request->getfd(), responseStr.c_str(), responseStr.size(), 0);
  this->complete =1;
}
int indexValidPath(string str) {
    int i = 0;
    if (str[i] == '/') {
        i++;
    }
    if (str.find("/..")!=string::npos) {
      return -1;
    }
    if (str[i] == '\0') {
        return i;
    }
    while (str[i] && str[i] != '/') {
      if (str[i]=='.') {
        return 0;
      }
        i++;
    }
    return i;
}
bool HttpResponse::methodIsValid(ServerConfig &config, string method) {
  size_t pos = method.find("GET");
  if (pos == string::npos) {
    this->sendErrorPage(config, 405);
    return false;
  }
  return true;
}
string findMatchingLocation(const string& uri, const map<string, LocationConfig>& locations) {
    string matched = "";
    size_t max_len = 0;
    for (map<string, LocationConfig>::const_iterator it = locations.begin(); it != locations.end(); ++it) {
        const string& loc = it->first;
        if (uri.find(loc) == 0 && loc.length() > max_len) {
            matched = loc;
            max_len = loc.length();
        }
    }
    return matched;
}

void HttpResponse::getResponse() {
    string data;
    string path;
    ServerConfig config;
    config = this->request->getServerConf();
    vector<string> words = this->request->getDataFirstLine();
    this->strLocation = findMatchingLocation(words[1], config.location);
    if (!this->strLocation.empty())
        data = words[1].substr(this->strLocation.length());
    else
        this->strLocation = "/";

    if (data.empty() && words[1].back() == '/')
        data = "/";
  // cout <<"str ----------->"<<this->strLocation <<endl;
  if (config.location.find(strLocation) != config.location.end()) {
    LocationConfig log = getValueMap(config.location, config.location.find(this->strLocation));
    if (!log._return.empty()) {
      redirectionResponse(log._return,config);
      return;
    }
 
    getLocationResponse(log, data, config);
    return;
  }
    // if (!methodIsValid(config, log._allowed_methods)) {
    //   return;
    // }
  path = config.getRoot();
  path += words[1];
  if (checkTypePath(path) == 0) {
    this->sendErrorPage(config, 404);
  } else if (checkTypePath(path) == 1) {
    this->fileDataSend(path, config);
  } else if (checkTypePath(path) == 2) {
    this->dirDataSend(path, config);
  }
}


int HttpResponse::checkDataResev() {
  ServerConfig config = this->request->getServerConfig();
  int statuscode = this->request->getRequestStatus();
  if (statuscode == 400) {
    this->sendErrorPage(config, 400);
    return 1;
  } else if (statuscode == 505) {
    this->sendErrorPage(config, 505);
    return 1;
  } else if (statuscode ==405) {
    this->sendErrorPage(config,405);
    return 1;
  } else if (statuscode==404) {
    this->sendErrorPage(config,404);
    return 1;
  } else if (statuscode==403) {
    this->sendErrorPage(config,403);
    return 1;
  } else if (statuscode==500) {
    this->sendErrorPage(config,500);
    return 1;
  } else if (statuscode==504) {
    this->sendErrorPage(config,504);
    return 1;
  } else if (statuscode == 413) {
    this->sendErrorPage(config,413);
    return 1;
  }
  return 0;
}

HttpResponse::HttpResponse(HttpRequest *re)
    : request(re), firstTimeResponse(0), file_offset(0), complete(0),
      totalSent(0) ,bytesSend(0){

}

HttpResponse::~HttpResponse() {}

int HttpResponse::writeData() {

  ServerConfig config = this->request->getServerConfig();
  string data = "doc/html/Upload_/succ.html";
  int method = this->request->_method;
  if (this->checkDataResev() != 0) {
    return this->bytesSend;
  }
  if (this->request->checkCgi) {
     this->cgiResponse();
    return this->bytesSend;
  }
  if (method == GET) {
    this->getResponse();
  }
  else if (method == POST) {
    this->fileDataSend(data,config);
  }
  else if (method == DELETE)
  {
    this->fileDataSend(data,config);
  }
  return this->bytesSend;
}

/*--------------------------------------------------------------------------------------------*/

/*---------------------- fanction extern class
 * used------------------------------------------*/

bool ExistFile(string &filePath) {
  struct stat infoFile;
  if (stat(filePath.c_str(), &infoFile) != 0) {
    return false;
  }
  if (S_ISREG(infoFile.st_mode)) {
    return true;
  }
  return false;
}

string dirAutoindex(string &strlocation,string &dirPath, string &root) {
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
  string str = dirPath.substr(root.length());
  strlocation += str;
  while(strlocation[strlocation.length()-1]=='/')
    strlocation.pop_back();
  struct dirent *entry;
  while ((entry = readdir(dir)) != nullptr) {
    if (strcmp(entry->d_name, ".") != 0 && strcmp(entry->d_name, "..") != 0) {
      html += "<li><a href=\"" + strlocation + "/" + entry->d_name + "\">";
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
  // cout << "path"<<endl;
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
/*-------------------------------------- CGI
 * ------------------------------------------*/
void HttpResponse::cgiResponse() {

  bodycgi = request->getBodyCgi();
    if (firstTimeResponse == 0) {
            file_size = 0;
            std::ostringstream response_headers;
            response_headers << status_line(this->request->getfd(), this->request->getRequestStatus());
            response_headers << headersSending(this->request->getfd(), this->request->getServerConf().getServerName());
            response_headers <<"Access-Control-Allow-Headers: *\r\n"
                             << "Access-Control-Allow-Origin: *\r\n"
                             << "Content-Type: text/html\r\n"
                             << "Content-Length: " << bodycgi.length() << "\r\n"
                             << "Connection: " << this->request->typeConnection << "\r\n"
                             << "\r\n";

            this->bytesSend = send(this->request->getfd(), response_headers.str().c_str(), response_headers.str().size(), 0);
            firstTimeResponse = 1;
        }

    const size_t buffer_size = 1024;
    char buffer[buffer_size];
    int bytes_read;
    size_t remaining = bodycgi.size() - file_offset;
    size_t to_send = (remaining > buffer_size) ? buffer_size : remaining;
    memcpy(buffer, bodycgi.c_str() + file_offset, to_send);
    bytes_read = to_send;
    if (bytes_read > 0) {
        this->bytesSend = send(this->request->getfd(), buffer, bytes_read, 0);
        this->file_offset += bytes_read;
    }
    if (bytes_read == 0) {
        complete = 1;
    }

}

/*--------------------------------------Post
 * method------------------------------------------*/

void HttpResponse::postResponse() {
  string body;
  ServerConfig config;
  config = this->request->getServerConf();
  // cout <<"URL = "<<words[1]<<endl;
  ifstream file("./doc/html/Upload_/succ.html"); ///
  stringstream fileContent;

  if (file) {
    fileContent << file.rdbuf();
    body = fileContent.str();
    file.close();
  } else {
    body = "<!DOCTYPE html>\n"
           "<html lang=\"en\">\n"
           "<head>\n"
           "<meta charset=\"UTF-8\">\n"
           "<meta name=\"viewport\" content=\"width=device-width, "
           "initial-scale=1.0\">\n"
           "<title>Upload Successful</title>\n"
           "</head>\n"
           "<body>\n"
           "<h1>Upload Successful</h1>\n"
           "<p>Your file has been uploaded successfully.</p>\n"
           "<p><a href=\"/\">Return to Home</a></p>\n"
           "</body>\n"
           "</html>\n";
  }
  stringstream response1;
  response1 << status_line(this->request->getfd(), 201);
  response1 << headersSending(this->request->getfd(), config.getServerName());
  response1 << "Content-Type: text/html\r\n"
            << "Content-Length: " << body.size() << "\r\n"
            << "Connection: close\r\n"
            << "Set-Cookie: username=login\r\n"
            << "\r\n"
            << body;
  string responseStr = response1.str();

  ssize_t bytesSent =
      send(this->request->getfd(), responseStr.c_str(), responseStr.size(), 0);
  this->totalSent += bytesSent;
  if (responseStr.size() == totalSent) {
    complete = 1;
    this->totalSent = 0;
  }
}

/*--------------------------------------------------------------------------------------------*/

string HttpResponse::getMimeType(string &extension)
{
  mimeType[".txt"]= "text/plai";
  mimeType[".html"] = "text/html";
  mimeType[".css"] = "text/css";
  mimeType[".js"] = "text/javascript";
  mimeType[".md"] = "text/markdown";
  mimeType[".csv"] = "text/csv";
  mimeType[".cpp"] = "text/x-c";

  // Application types
  mimeType[".json"] = "application/json";
  mimeType[".xml"] = "application/xml";
  mimeType[".pdf"] = "application/pdf";
  mimeType[".zip"] = "application/zip";
  mimeType[".rar"] = "application/x-rar-compressed";
  mimeType[".tar"] = "application/x-tar";
  mimeType[".gz"] = "application/gzip";
  mimeType[".doc"] = "application/msword";
  mimeType[".xls"] = "application/vnd.ms-excel";
  mimeType[".docx"] =
      "application/vnd.openxmlformats-officedocument.wordprocessingml.document";
  mimeType[".xlsx"] =
      "application/vnd.openxmlformats-officedocument.spreadsheetml.sheet";
  mimeType[".bin"] = "application/octet-stream";

  // Image types
  mimeType[".jpg"] = "image/jpeg";
  mimeType[".png"] = "image/png";
  mimeType[".gif"] = "image/gif";
  mimeType[".svg"] = "image/svg+xml";
  mimeType[".webp"] = "image/webp";
  mimeType[".tiff"] = "image/tiff";
  mimeType[".bmp"] = "image/bmp";
  mimeType[".ico"] = "image/x-icon";

  // Audio types
  mimeType[".mp3"] = "audio/mpeg";
  mimeType[".wav"] = "audio/wav";
  mimeType[".weba"] = "audio/webm";

  // Video types
  mimeType[".mp4"] = "video/mp4";
  mimeType[".webm"] = "video/webm";
  mimeType[".ogv"] = "video/ogg";

  // Font types
  mimeType[".ttf"] = "font/ttf";
  mimeType[".woff"] = "font/woff";
  mimeType[".woff2"] = "font/woff2";
  mimeType[".otf"] = "font/otf";
  if (mimeType.find(extension) != mimeType.end())
    return mimeType[extension];
  return "application/octet-stream"; // Default binary type
}

/*----------------------------------error page ----------------*/

string errorPage(int statusCode) {
  string message = statusText(statusCode);
  return "<!DOCTYPE html>\n"
         "<html lang=\"en\">\n"
         "<head>\n"
         "<meta charset=\"UTF-8\">\n"
         "<meta name=\"viewport\" content=\"width=device-width, "
         "initial-scale=1.0\">\n"
         "<title>" +
         to_string(statusCode) +
         "</title>\n"
         "<style>\n"
         "body { font-family: Arial, sans-serif; text-align: center; padding: "
         "50px; }\n"
         "h1 { font-size: 50px; color: red; }\n"
         "p { font-size: 20px; color: #666; }\n"
         "</style>\n"
         "</head>\n"
         "<body>\n"
         "<h1>" +
         message +
         "</h1>\n"
         "<hr>\n"
         "</body>\n"
         "</html>";
}

void HttpResponse::sendErrorPage(ServerConfig &config, int status) {
  std::string ContentType;
  string val;
  string data;
  if (firstTimeResponse == 0) {
    string root = config.getRoot();
    std::ostringstream s;
    s << status;
    string statusStr = s.str();
    map<string, string>::const_iterator it;
    for (it = config.errorpage.begin(); it != config.errorpage.end(); ++it) {
      if (it->first.find(statusStr) != string::npos) {
        val = it->second;
        break;
      }
    }
    root += val;
    data = root;
  }
  if (!this->file.is_open()) {
    this->file.open(data, std::ios::binary);
    if (val.empty()||!this->file.is_open()) {
      string body = errorPage(status);
      stringstream response1;
      response1 << status_line(this->request->getfd(), status);
      response1 << headersSending(this->request->getfd(), config.getServerName());
      response1 << "Content-Type: text/html\r\n"
                << "Content-Length: " << body.size() << "\r\n"
                << "Connection: close\r\n"
                << "\r\n"
                << body;
      string responseStr = response1.str();
      cout <<"              path : " << strLocation<<"                                                 status code :"<<status<<endl;
      this->bytesSend = send(this->request->getfd(), responseStr.c_str(), responseStr.size(), 0);
      complete = 1;
      return ;
    }
  }
  if (firstTimeResponse == 0) {
    file_size = 0;
    this->file.seekg(0, std::ios::end);
    file_size = this->file.tellg();
    this->file.seekg(0, std::ios::beg);
    size_t pos = data.find(".");
    if (pos == string::npos) {
      ContentType = "text/plain";
    } else {
      string extension = data.substr(pos);
      ContentType = getMimeType(extension);
    }
    std::ostringstream response_headers;
    response_headers << status_line(this->request->getfd(), status);
    response_headers << headersSending(this->request->getfd(), config.getServerName());
    response_headers << "Content-Type: " << ContentType << "\r\n"
                     << "Content-Length: " << file_size << "\r\n"
                     << "Connection: close\r\n"
                     << "\r\n";
    // cout <<"              path : " << strLocation<<"                                                 status code :"<<status<<endl;
    this->bytesSend = send(this->request->getfd(), response_headers.str().c_str(),
         response_headers.str().size(), 0);
    firstTimeResponse = 1;
  }

  this->file.seekg(this->file_offset, std::ios::beg);
  const size_t buffer_size = 1024;
  char buffer[buffer_size];
  this->file.read(buffer, buffer_size);
  std::streamsize bytes_read = this->file.gcount();
  if (bytes_read > 0) {
    this->bytesSend = send(this->request->getfd(), buffer, bytes_read, 0);
    this->file_offset += bytes_read;
  }
  if (bytes_read == 0) {
    this->file.close();
    complete = 1;
  }
}