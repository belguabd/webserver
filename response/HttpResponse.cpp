#include "HttpResponse.hpp"
#include <cstddef>
#include <sys/_types/_ssize_t.h>

string statusText(int status) {
  string text;
  if (status == 200) { // 200 400 403 404 405 413 414 500 501 505  201 CREATED
    text = "200 OK\r\n";
  } else if (status == 201) {
    text = "201 CREATED\r\n";
  } else if (status == 400) {
    text = "400 Bad Request\r\n";
  } else if (status == 403) {
    text = "403 Forbidden\r\n";
  } else if (status == 404) {
    text = "404 Not Found\r\n";
  } else if (status == 405) {
    text = "405 Method Not Allowed\r\n";
  } else if (status == 413) {
    text = "413 Request Entity Too Large\r\n";
  } else if (status == 414) {
    text = "414 Request-URI Too Long\r\n";
  } else if (status == 500) {
    text = "500 Request-URI Too Long\r\n"; ///
  } else if (status == 501) {
    text = "501 Not Implemented\r\n"; ///
  } else if (status == 505) {
    text = "505 HTTP Version Not Supported\r\n";
  } else if (status == 301) {
    text = "301 Moved Permanently\r\n";
  } else if (status == 409) {
    text = "409 conflict\r\n"; ///
  } else if (status == 204) {
    text = "204 No Content\r\n";
  }
  return text;
}
void status_line(int client_socket, int status) {
  string firstline = "HTTP/1.1 ";
  firstline += statusText(status);
  send(client_socket, firstline.c_str(), firstline.size(), 0);
}

void headersSending(int client_socket, string serverName) {
  std::string header;
  time_t now = time(0);
  struct tm tstruct = *localtime(&now);
  char buffer[80];
  strftime(buffer, sizeof(buffer), "%a %d %b %Y %H:%M:%S GMT\r\n", &tstruct);
  header += "Date: ";
  header += buffer;
  if (!serverName.empty()) {
    header += "Server: ";
    header += serverName;
    header += "\r\n";
  }
  // std::cout << header;
  send(client_socket, header.c_str(), header.size(), 0);
}
/*---------------------- Get method------------------------------------------*/
void HttpResponse::fileDataSend(std::string &data, ServerConfig &config) {
  std::string ContentType;
  if (!this->file.is_open()) {
    this->file.open(data, std::ios::binary);
    if (!this->file.is_open()) {
      std::cerr << "Error opening file: " << data << std::endl;
      return;
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
    status_line(this->request->getfd(), 200);
    headersSending(this->request->getfd(), config.getServerName());
    response_headers << "Content-Type: " << ContentType << "\r\n"
                     << "Content-Length: " << file_size << "\r\n"
                     << "Connection: close\r\n"
                     << "\r\n";

    send(this->request->getfd(), response_headers.str().c_str(),
         response_headers.str().size(), 0);
    firstTimeResponse = 1;
  }

  this->file.seekg(this->file_offset, std::ios::beg);
  const size_t buffer_size = 1024;
  char buffer[buffer_size];
  this->file.read(buffer, buffer_size);
  std::streamsize bytes_read = this->file.gcount();
  if (bytes_read > 0) {
    send(this->request->getfd(), buffer, bytes_read, 0);
    this->file_offset += bytes_read;
    //  cout <<"fd client = "<<this->request->getfd()<<"   file_offset = " <<
    //  round((double)this->file_offset / (1024*1024) * 10) / 10 << " MB" <<
    //  endl;
  }
  if (bytes_read == 0) {
    this->file.close();
    complete = 1;
    cout << "----- END file --------" << endl;
  }
}

int HttpResponse::checkFileAndSendData(string &data, ServerConfig &config,
                                       string &index) {
  string str;
  size_t i = 0;
  vector<string> words;
  words = splitstring(index);
  bool exist = false;
  while (data[data.length() - 1] == '/' && i < words.size()) {
    str = data;
    str += words[i];
    if (ExistFile(str) == true) {
      exist = true;
      this->fileDataSend(str, config);
      break;
    }
    i++;
  }
  if (data[data.length() - 1] == '/' && exist == false) {
    return 1;
  }
  return 0;
}

void HttpResponse::dirDataSend(string &data, string &root,
                               LocationUplaods &upload, ServerConfig &config) {
  int checkExist;
  if (!upload.index.empty()) {
    checkExist = this->checkFileAndSendData(data, config, upload.index);
  } else {
    string index;
    index = config.getIndex();
    checkExist = this->checkFileAndSendData(data, config, index);
  }
  if (checkExist == 1) {
    this->sendErrorPage(config, 404);
    return;
  }
  this->sendErrorPage(config, 403);
}

void HttpResponse::dirDataSend(string &data, string &root,
                               LocationConfig &normal, ServerConfig &config) {
  int checkExist;
  if (!normal.index.empty()) {
    checkExist = this->checkFileAndSendData(data, config, normal.index);
  } else {
    string index;
    index = config.getIndex();
    checkExist = this->checkFileAndSendData(data, config, index);
  }
  if (checkExist == 1) {
    this->sendErrorPage(config, 404);
    return;
  }
  if (normal.autoindex == false) {
    this->sendErrorPage(config, 403);
  } else {
    status_line(this->request->getfd(), 200);
    headersSending(this->request->getfd(), config.getServerName());
    string body = dirAutoindex(data, root);
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
    status_line(this->request->getfd(), 200);
    headersSending(this->request->getfd(), config.getServerName());
    string body = dirAutoindex(data, root);
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
void HttpResponse::getLocationResponse(LocationConfig &normal, string &str,
                                       ServerConfig &config) {
  string data;
  string root;
  int typePath;
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
  typePath = checkTypePath(data);
  if (typePath == 0) {
    this->sendErrorPage(config, 404);
  } else if (typePath == 1) {
    this->fileDataSend(data, config);

  } else if (checkTypePath(data) == 2) {
    if (flag == 1)
      this->dirDataSend(data, root, normal, config);
    else
      this->dirDataSend(data, normal.root, normal, config);
  }
}
void HttpResponse::getLocationResponse(LocationUplaods &upload, string &str,
                                       ServerConfig &config) {
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
  if (checkTypePath(data) == 0) {
    this->sendErrorPage(config, 404);
  } else if (checkTypePath(data) == 1) {
    this->fileDataSend(data, config);

  } else if (checkTypePath(data) == 2) {
    if (flag == 1)
      this->dirDataSend(data, root, upload, config);
    else
      this->dirDataSend(data, upload.root, upload, config);
  }
}
void HttpResponse::redirectionResponse(string &str) {
  ServerConfig config;
  string data;
  config = this->request->getServerConfig();
  if (config.typeUrl == 1) {
    if (this->request->mapheaders.find("Host") !=
        this->request->mapheaders.end()) {
      string host = getValueFromMap(config.configRedirection,
                                    config.configRedirection.find("Host"));
      data = host;
      data += str;
    }
  } else {
    data = str;
  }
  status_line(this->request->getfd(), 301);
  headersSending(this->request->getfd(), config.getServerName());
  stringstream response1;
  response1 << "Location:" << data << "\r\n\r\n";
  string responseStr = response1.str();
  send(this->request->getfd(), responseStr.c_str(), responseStr.size(), 0);
}
int indexValidPath(string str) {
  int i = 0;
  if (str.length() == 1)
    return 0;
  i++;
  while (str[i] && str[i] != '/')
    i++;
  return (i);
}
bool HttpResponse::methodIsValid(ServerConfig &config, string method) {
  size_t pos = method.find("GET");
  if (pos == string::npos) {
    this->sendErrorPage(config, 405);
    return false;
  }
  return true;
}
void HttpResponse::getResponse() {
  string data;
  string path;
  string str;
  ServerConfig config;
  config = this->request->getServerConf();
  vector<string> words = this->request->getDataFirstLine();
  int i = indexValidPath(words[1]);
  str = words[1].substr(0, i);
  if (words[1][i] != 0)
    data = words[1].substr(i);
  else
    data += "/";
  if (config.configRedirection.find(str) != config.configRedirection.end()) {
    string urlRedirection = getValueFromMap(config.configRedirection,
                                            config.configRedirection.find(str));
    redirectionResponse(urlRedirection);
    return;
  }
  if (config.configNormal.find(str) != config.configNormal.end()) {
    LocationConfig log =
        getValueFromMap(config.configNormal, config.configNormal.find(str));
    if (!methodIsValid(config, log.allowed_methods)) {
      return;
    }
    getLocationResponse(log, data, config);
    return;
  }
  if (config.configUpload.find(str) != config.configUpload.end()) {
    LocationUplaods log =
        getValueFromMap(config.configUpload, config.configUpload.find(str));
    if (!methodIsValid(config, log.allowed_methods)) {
      return;
    }
    getLocationResponse(log, data, config);
    return;
  }
  path = config.getRoot();
  if (words[1].length() == 1) {
    defautlRoot(config);
    return;
  }
  path += words[1];
  // cout <<"fd  =   "<<this->request->getfd()<<", path = "<<path<<endl;
  if (checkTypePath(path) == 0) {
    this->sendErrorPage(config, 404);
  } else if (checkTypePath(path) == 1) {
    this->fileDataSend(path, config);
  } else if (checkTypePath(path) == 2) {
    this->dirDataSend(path, config);
  }
}
void HttpResponse::defautlRoot(ServerConfig &config) {
  vector<string> words;
  string root = config.getRoot();
  size_t i = 0;
  words = splitstring(config.getIndex());
  bool exist = false;
  while (i < words.size()) {
    root = config.getRoot();
    root += "/";
    root += words[i];
    if (ExistFile(root) == true) {
      exist = true;
      break;
    }
    i++;
  }
  if (exist == true) {
    this->fileDataSend(root, config);
  } else {
    this->sendErrorPage(config, 404);
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
  }
  return 0;
}
void sendResponse(HttpResponse &response) {
  int method = response.request->_method;
  if (response.checkDataResev() != 0) {
    return;
  }
  if (response.request->checkCgi) {
    puts("==================================>");
    response.cgiResponse();
    return;
  }
  if (method == GET) {
    response.getResponse();
  } else if (method == POST) {
    response.postResponse();
  } else if (method == DELETE) {
    // response.deleteResponse();
  }
}

HttpResponse::HttpResponse(HttpRequest *re)
    : request(re), firstTimeResponse(0), file_offset(0), complete(0) {

  // static int i =0;
  // if (i ==1)
  //   exit(102);
  // cout <<"here ----->"<<endl;
  // i++;
}

HttpResponse::~HttpResponse() {}

int HttpResponse::writeData() {
  
  int bytes_send = 0;
  sendResponse(*this);
  cout << "complete ------>"<< this->complete << "\n";
  //   const char *msg = "Hi I am server";
  //   ssize_t bytes_send = send(client_fd, msg, strlen(msg), 0);
  //   if (bytes_send == -1)
  //     std::cerr << "Error sending message to client" << std::endl;
  return bytes_send;
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

string dirAutoindex(string &dirPath, string &root) {

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
  size_t pos = dirPath.rfind("/");
  string str = dirPath.substr(pos + 1);
  struct dirent *entry;
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

  // cout << "--->"<<this->request->filename << "\n";
  ServerConfig config;
  // ifstream file(this->request->filename);
  // stringstream fileContent;
  // fileContent << request->getBodyCgi();

  // fileContent << file.rdbuf();
  // if (file) {
  //     fileContent << file.rdbuf();
  //     file.close();
  // }
  status_line(this->request->getfd(), 200);
  headersSending(this->request->getfd(), config.getServerName());
  // string body = fileContent.str();
  // cout << "request--------->"<< request->getBodyCgi() << endl;
  string body = request->getBodyCgi();
  stringstream response1;
  response1 << "Content-Type: text/html\r\n"
            // response1 << "Content-Type: image/jpeg\r\n"
            << "Content-Length: " << body.size() << "\r\n"
            << "Connection: close\r\n"
            << "\r\n"
            << body;
  string responseStr = response1.str();
  send(this->request->getfd(), responseStr.c_str(), responseStr.size(), 0);
  this->complete = 1;
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
  status_line(this->request->getfd(), 201);
  headersSending(this->request->getfd(), config.getServerName());
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

string HttpResponse::getMimeType(string &extension) {
  mimeType[".txt"] = "text/plai";
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
  string data;
  if (firstTimeResponse == 0) {
    string val;
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
    if (!this->file.is_open()) {
      string body = errorPage(status);
      status_line(this->request->getfd(), status);
      headersSending(this->request->getfd(), config.getServerName());
      stringstream response1;
      response1 << "Content-Type: text/html\r\n"
                << "Content-Length: " << body.size() << "\r\n"
                << "Connection: close\r\n"
                << "\r\n"
                << body;
      string responseStr = response1.str();
      send(this->request->getfd(), responseStr.c_str(), responseStr.size(), 0);
      complete = 1;
      return;
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
    status_line(this->request->getfd(), status);
    headersSending(this->request->getfd(), config.getServerName());
    response_headers << "Content-Type: " << ContentType << "\r\n"
                     << "Content-Length: " << file_size << "\r\n"
                     << "Connection: close\r\n"
                     << "\r\n";

    send(this->request->getfd(), response_headers.str().c_str(),
         response_headers.str().size(), 0);
    firstTimeResponse = 1;
  }

  this->file.seekg(this->file_offset, std::ios::beg);
  const size_t buffer_size = 1024;
  char buffer[buffer_size];
  this->file.read(buffer, buffer_size);
  std::streamsize bytes_read = this->file.gcount();
  if (bytes_read > 0) {
    send(this->request->getfd(), buffer, bytes_read, 0);
    // cout <<"fd client = "<<this->request->getfd()<<"   file_offset = " <<
    // round((double)this->file_offset / (1024*1024) * 10) / 10 << " MB" <<
    // endl;
    this->file_offset += bytes_read;
  }
  if (bytes_read == 0) {
    this->file.close();
    complete = 1;
    cout << "----- END file --------" << endl;
  }
}