#include "HttpRequest.hpp"
int i= 0;

void printRequest(HttpRequest &request)
{
  std::string tmp = "currentRequest";
  std::string tmp2 = request.getreadbuffer();
  pasteInFile(tmp, tmp2);
}

void handleRequest(HttpRequest &request) {
  string str_parse;
  printRequest(request);
  request.joinBuffer();
  str_parse = request.partRquest();
  if (request.getFirstTimeFlag() == 0) {
    size_t pos = str_parse.find("\r\n");
    if (pos != string::npos) {
      request.setFirstTimeFlag(1);
      request.sig = request.defineTypeMethod(str_parse.substr(0, pos + 2));
      request.requestLine();
      str_parse = str_parse.substr(pos + 2);
    }
  }
  request.parsePartRequest(str_parse);

  if (request.sig == 1 && request.getendHeaders() == 1)
  {
    // cout <<"-------___------end headers----- get"<<endl;
    request.setRequestStatus(1);
  }

  else if (request.sig == 2 && request.getendHeaders() == 1)
  {
    string tmp;
    // cout <<"-------___------end headers----- post"<<endl;
    if (i == 0)
    {
      tmp = request.getbuffer();
      request._post.start(request.mapheaders, request.queryParam, tmp);
      i = 1;
    }
    else
    {
      tmp = request.getreadbuffer();
      request._post.proseRequest(tmp);
    }
    request.setRequestStatus(request._post.getStatus());
      
    request.joinBuffer();
  }
}

HttpRequest::HttpRequest(int client_fd)
    : client_fd(client_fd), firsttime(0), endHeaders(0),sig(0) {
  int flags = fcntl(client_fd, F_GETFL, 0);
  fcntl(client_fd, F_SETFL, flags | O_NONBLOCK);
}

int HttpRequest::readData() {
  char buffer[2];
  ssize_t bytes_received;
  std::memset(buffer, 0, sizeof(buffer));
  bytes_received = recv(client_fd, buffer, sizeof(buffer), 0);
  if (bytes_received > 0) {
    readBuffer.assign(buffer, bytes_received);
    // std::cout << readBuffer << "\n";
    handleRequest(*this);
  }
  return bytes_received;
}
std::string trimNewline(std::string str) {
  while (!str.empty() && (str.back() == '\n' || str.back() == '\r' ||
                          str.back() == ' ' || str.back() == '\t')) {
    str.pop_back();
  }
  return str;
}

HttpRequest::~HttpRequest() {}
int HttpRequest::defineTypeMethod(string firstline) {
  firstline = trimNewline(firstline);
  vector<string> words;
  size_t i = 0;
  while (i < firstline.length()) {
    while (i < firstline.length() &&
           (firstline[i] == ' ' || firstline[i] == '\t'))
      i++;
    if (i >= firstline.length())
      break;
    size_t pos = firstline.find_first_of(" \t", i);
    if (pos == std::string::npos)
      pos = firstline.length();
    words.push_back(firstline.substr(i, pos - i));
    i = pos + 1;
  }
  if (words.size() != 3 || words[1][0] != '/') {
    cout << "method error" << endl;
    exit(0);
  }
  this->dataFirstLine = words;
  if (words[0]=="GET")
    return (1);
  else if (words[0]=="POST")
    return (2);
  else if (words[0]=="DELETE")
    return (3);
  return (0);
}

vector<string> splitstring(const string &str)
{
  vector<string> words;
  size_t i = 0, j;
  while (i < str.length())
  {
    if ((j = str.find_first_of(" \t", i)) != string::npos)
    {
      if (j > i)
        words.push_back(str.substr(i, j - i));
      i = j + 1;
    }
    else
    {
      words.push_back(str.substr(i));
      break;
    }
  }
  return (words);
}
void HttpRequest::checkHeaders(string &str) {


  str = trimNewline(str);
  size_t pos = str.find(':');
  string result;
  vector<string> words;
  if (pos == string::npos || (pos > 0 && str[pos - 1] == ' ')) {
    cout << "bad request space : " << endl;
    exit(0);
  }
  words = splitstring(str.substr(pos + 1, str.length()));
  for (vector<string>::const_iterator it = words.begin(); it != words.end();
       ++it)
  {
    const string &words = *it;
    result += ' ';
    result += words;
  }
  string headerName = str.substr(0, pos);
  result.erase(0, result.find_first_not_of(" "));
  this->mapheaders[headerName] = result;
}

string HttpRequest ::partRquest() {
  if (this->endHeaders == 1)
    return "";
  string line;
  string str;
  line += this->_buffer;
  this->_buffer.clear();
  size_t pos = line.find("\r\n\r\n");
  if (pos != string::npos) {
    str = line.substr(0, pos + 4);
    this->_buffer = line.substr(pos + 4);
  } else {
    pos = line.rfind("\r\n");
    if (pos != string::npos) {
      str = line.substr(0, pos + 2);
      this->_buffer = line.substr(pos + 2);
    } else {
      this->_buffer = line;
      return "";
    }
  }
  return (str);
}
void HttpRequest ::joinBuffer() {

  if (this->endHeaders == 1)
    return ;
  this->_buffer += this->readBuffer;
  this->readBuffer.clear();
}

void HttpRequest ::parsePartRequest(string str_parse) {
  if (this->endHeaders==1)
    return;
  while (!str_parse.empty()) {
    size_t pos = str_parse.find("\r\n");
    if (pos == string::npos)
      break;
    string str = str_parse.substr(0, pos + 2);
    if (str == "\r\n") {
      this->endHeaders = 1;
      break ;
    }
    str_parse = str_parse.substr(pos + 2);
    checkHeaders(str);
    str.clear();
  }
}
void HttpRequest ::requestLine() {
  string path;
  string querydata;
  path = this->dataFirstLine[1];
  if (this->dataFirstLine[2].compare("HTTP/1.1") != 0) {
    cout << "Not Supported" << endl;
    return ;
  }
  size_t pos = path.find("?");
  if (pos == string::npos) {
    pos = path.find("#");
    if (pos == string::npos) {
      return;
    } else {
      this->dataFirstLine[1] = this->dataFirstLine[1].substr(0, pos);
      cout << "-- > " << this->dataFirstLine[1] << endl;
      return;
    }
  }
  if (this->dataFirstLine[1][pos + 1] == '\0') {
    cout << "erro ??" << endl;
    this->dataFirstLine[1] = this->dataFirstLine[1].substr(0, pos);
    return;
  }
  querydata = this->dataFirstLine[1].substr(pos + 1);
  this->dataFirstLine[1] = this->dataFirstLine[1].substr(0, pos);
  size_t i = 0;
  while (i < querydata.length()) {
    size_t endkey = querydata.find("=", i);
    size_t endval = querydata.find("&", i);
    if (endkey != string::npos) {
      string key = querydata.substr(i, endkey - i);
      string value;
      if (endval != string::npos)
        value = querydata.substr(endkey + 1, endval - endkey - 1);
      else
        value = querydata.substr(endkey + 1);
      cout << "Key: " << key << ", Value: " << value << endl;
      if (endval != string::npos)
        i = endval + 1;
      else
        i = querydata.length();
      this->queryParam[key] = value;
    }
  }
}
