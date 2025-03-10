#include "HttpRequest.hpp"

void handleRequest(HttpRequest &request) {
  string str_parse;
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

  if (request.sig == 1 && request.getendHeaders() == 1) {
  if (request.sig == 1 && request.getendHeaders() == 1)
  {
    request.setRequestStatus(1);
  } else if (request.sig == 2 && request.getendHeaders() == 1) {
  }
  else if (request.sig == 2 && request.getendHeaders() == 1) {
    string tmp;
    if (i == 0) {
    if (request.firstPartBody == 0) {
      tmp = request.getbuffer();
      request._post.start(request.mapheaders, tmp);
      i = 1;
    } else {
      tmp = request.getreadbuffer();
      request._post.proseRequest(tmp);
    }
    request.setRequestStatus(request._post.getStatus());
      
    request.joinBuffer();
  }
}

HttpRequest::HttpRequest(int client_fd, ServerConfig &server_config)
    : client_fd(client_fd), firsttime(0), endHeaders(0), sig(0) , server_config(server_config),firstPartBody(0),checkCgi(0){
  int flags = fcntl(client_fd, F_GETFL, 0);
  fcntl(client_fd, F_SETFL, flags | O_NONBLOCK);
}

int HttpRequest::readData() {
  char buffer[5124];
  ssize_t bytes_received;
  std::memset(buffer, 0, sizeof(buffer));
  bytes_received = recv(client_fd, buffer, sizeof(buffer), 0);
  if (bytes_received > 0) {
    readBuffer.assign(buffer, bytes_received);
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
LocationCgi getValueMapcgi(map<string, LocationCgi> & configNormal,map<string, LocationCgi> ::const_iterator it) {
  LocationCgi config;
    if (it != configNormal.end()) {
        config= it->second;
    }
    return config;
}
HttpRequest::~HttpRequest() {}
void HttpRequest::checkPathIscgi(string &path)
{
  // cout <<"path  = > "<<path<<endl;
  ServerConfig config;
  string s;
  string method;
  vector <string >allowedMethod;
  vector <string >extension;
  LocationCgi log;
  config = this->getServerConfig();
  int i = 0 ;
  i = indexValidPath(path);
  string str = path.substr(0,i);
  string data = path.substr(i);
   if (config.configcgi.find(str) != config.configcgi.end()) {
    log = getValueMapcgi(config.configcgi,config.configcgi.find(str));
    allowedMethod = splitstring(log.allowed_methods);
    extension = splitstring(log.cgi_extension);
    this->checkCgi = 1;
  }
  else
    return ;
    this->rootcgi = log.root + data;
  for(size_t i = 0;i < extension.size();i++)
  {
      if (this->rootcgi.find(extension[i])!=string::npos) {
        s = extension[i];
        break;
      }
  }
  for(size_t i = 0;i < allowedMethod.size();i++)
  {
      if (this->dataFirstLine[0]==allowedMethod[i]) {
        method = allowedMethod[i];
        break;
      }
  }
    cout << "rootcgi  = "<<this->rootcgi<<endl;
    bool f = fileExists(this->rootcgi);
    cout << "fileExiists  = "<<f<<endl;
    cout << "extension  = "<<s<<endl;
    cout << "method  = "<<method<<endl;
    if (method.empty()) {
      cout <<"method not allowed"<<endl;
      exit(0);
    }
      if (s.empty()) {
      cout <<"CGI not supported type file"<<endl;
      exit(0);
    }

}
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
  checkPathIscgi(words[1]);
  if (words[0] == "GET")
    return (1);
  else if (words[0] == "POST")
    return (2);
  else if (words[0] == "DELETE")
    return (3);
  return (0);
}

vector<string> splitstring(const string &str) {
  vector<string> words;
  size_t i = 0, j;
  while (i < str.length()) {
    if ((j = str.find_first_of(" \t", i)) != string::npos) {
      if (j > i)
        words.push_back(str.substr(i, j - i));
      i = j + 1;
    } else {
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
       ++it) {
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
    return;
  this->_buffer += this->readBuffer;
  this->readBuffer.clear();
}

void HttpRequest ::parsePartRequest(string str_parse) {
  if (this->endHeaders == 1)
    return;
  while (!str_parse.empty()) {
    size_t pos = str_parse.find("\r\n");
    if (pos == string::npos)
      break;
    string str = str_parse.substr(0, pos + 2);
    if (str == "\r\n") {
      this->endHeaders = 1;
      break;
    }
    str_parse = str_parse.substr(pos + 2);
    checkHeaders(str);
    str.clear();
  }
}

void HttpRequest ::requestLine() {
  string path;
  string querydata;
  this->dataFirstLine[1] = encodeUrl(this->dataFirstLine[1]);
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

string encodeUrl(string &str)
{
  size_t pos = 0;
  string tmp;
  while ((pos=str.find("%",pos))!=string::npos)
  {
    tmp = str.substr(pos+1,2);
    char c = characterEncodeing(tmp);
    str.replace(pos,3,1,c);
    pos++;
  }
  return (str);
}

char characterEncodeing(string &tmp)
{
  // cout <<"tmp [0] = "<<tmp[0]<<endl; 
  // cout <<"tmp +[1] = "<<endl; 
  printNonPrintableChars(tmp);
  if (tmp[0]<'2' || tmp[0]>'7'||(!isdigit(tmp[1])&& (tmp[1]<'A' || tmp[1]>'F'))) {
    cout << "character not allowed"<<endl;
    exit (0);
  }
  return (static_cast<char>(stol(tmp, nullptr, 16)));
}
/*
find("%");
str.substr(pos ,pos+3);
%1
www.val.com/path/id?key=val&name=mohamed ayd

space         %20  
!             %21
"             %22
#             %23
$             %24
%             %25
&             %26
'             %27
(             %28
)             %29
*             %2A
+             %2B
,             %2C
-             %2D
.             %2E
/             %2F
0             %30
1             %31
2             %32
3             %33
4             %34
5             %35
6             %36
7             %37
8             %38
9             %39
:             %3A
;             %3B
<             %3C
=             %3D
>             %3E
?             %3F
@             %40
A             %41
B             %42
C             %43
D             %44
E             %45
F             %46
G             %47
H             %48
I             %49
J             %4A
K             %4B
L             %4C
M             %4D
N             %4E
O             %4F
P             %50
Q             %51
R             %52
S             %53
T             %54
U             %55
V             %56
W             %57
X             %58
Y             %59
Z             %5A
[             %5B
\             %5C
]             %5D
^             %5E
_             %5F
`             %60
a             %61
b             %62
c             %63
d             %64
e             %65
f             %66
g             %67
h             %68
i             %69
j             %6A
k             %6B
l             %6C
m             %6D
n             %6E
o             %6F
p             %70
q             %71
r             %72
s             %73
t             %74
u             %75
v             %76
w             %77
x             %78
y             %79
z             %7A
{             %7B
|             %7C
}             %7D
~             %7E


*/