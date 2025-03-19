#include "HttpRequest.hpp"
#define BUFFER_READ 5000

LocationUplaods &getMatchedLocationUpload(const std::string &path, map<string, LocationUplaods> &configUploads)
{
	size_t pos = path.find("/", 1);
	string keyLocationUpload;

    if (pos == std::string::npos)
      pos = path.size() + 1;
	keyLocationUpload = path.substr(0, pos);
	if (configUploads.find(keyLocationUpload) == configUploads.end())
		configUploads[keyLocationUpload] = (LocationUplaods){.upload_store = UPLOAD_FOLDER, .client_max_body_size= 1000000000, .allowed_methods="POST GET"}; // 
	return (configUploads.find(keyLocationUpload))->second;
}

void HttpRequest::handlePost()
{
  if (_post == NULL)
  {
    LocationUplaods &lc = getMatchedLocationUpload(dataFirstLine[1], server_config.getConfigUpload());
      mapheaders["isCgi"] = std::to_string(checkCgi);
      _post = new Post(mapheaders, queryParam, _buffer, lc);
  }
  else
  {
    _post->proseRequest(readBuffer);
    // std::cout << "abcd\n";
  }
  setRequestStatus(_post->getStatus());
	this->readBuffer.clear();
}

int HttpRequest::handleDeleteRequest(std::string filePath)
{
  struct stat meteData;
  filePath.insert(0, ".");
  // std::cout << "filePath: " << filePath << std::endl;

  size_t pos = filePath.find("/", 2);
  if (filePath.substr(0, pos + 1) != "./upload/")
  {
    return 405;
  }
  // Check if the file exists
  if (stat(filePath.c_str(), &meteData) != 0)
      return 404;

  // Try to delete the file
  if (unlink(filePath.c_str()) == 0)
      return 204;

  // Handle permission errors
  if (errno == EACCES || errno == EPERM)
      return 403;

  // Other filesystem errors
  return 500;
}

void HttpRequest::handleRequest()
{
  string str_parse;
  pasteInFile("currentRequest", readBuffer);
  joinBuffer();
  str_parse = partRquest();
  if (getFirstTimeFlag() == 0)
  {
    size_t pos = str_parse.find("\r\n");
    if (pos != string::npos) {
      setFirstTimeFlag(1);
      _method = defineTypeMethod(str_parse.substr(0, pos + 2));
      requestLine();
      str_parse = str_parse.substr(pos + 2);
      if (_method == DELETE)
        std::cout << "handle request number: " << handleDeleteRequest(dataFirstLine[1]) << std::endl;
    }
  }
  parsePartRequest(str_parse);
  if (getendHeaders() == 1 && this->requestStatus!=0) {
    return ;
  }
  else if ((_method == GET || _method == DELETE) && getendHeaders() == 1)
    setRequestStatus(200);
  else if (_method == POST && getendHeaders() == 1)
    handlePost();
}

HttpRequest::HttpRequest(int client_fd, ServerConfig &server_config)
    : client_fd(client_fd), firsttime(0), endHeaders(0), _method(0) , server_config(server_config) , isCGi(false) , checkCgi(0), cgi_for_test(0) , status_code(0){
  int flags = fcntl(client_fd, F_GETFL, 0);
  fcntl(client_fd, F_SETFL, flags | O_NONBLOCK);
  _post = NULL;
}

int HttpRequest::readData()
{
  char buffer[BUFFER_READ];
  ssize_t bytes_received;
  std::memset(buffer, 0, sizeof(buffer));
  bytes_received = recv(client_fd, buffer, sizeof(buffer), 0);
  if (bytes_received > 0)
  {
    readBuffer.assign(buffer, bytes_received);
    // puts("=========================");
    // std::cout << readBuffer << "\n";
    // puts("=========================");
    handleRequest();

  }
  return bytes_received;
}
std::string trimNewline(std::string str)
{
  while (!str.empty() && (str.back() == '\n' || str.back() == '\r' ||
                          str.back() == ' ' || str.back() == '\t'))
  {
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
    if (method.empty()) {
      exit(0);
    }
      if (s.empty()) {
      cout <<"CGI not supported type file"<<endl;
      exit(0);
    }
    size_t startPathInfo;
    size_t endPathInfo;
    startPathInfo =  this->rootcgi.find(s);

    endPathInfo = this->rootcgi.find("?");
    if (startPathInfo!=string::npos)
    {
      if (endPathInfo!=string::npos) {
        this->pathInfo = this->rootcgi.substr(startPathInfo + s.length(),endPathInfo - startPathInfo - s.length());
      }
      else
        this->pathInfo = this->rootcgi.substr(startPathInfo + s.length());
    }
    this->rootcgi = this->rootcgi.substr(0,startPathInfo+s.length());
    bool f = fileExists(this->rootcgi);
    if (f==false)
      this->rootcgi = "";
    if (s == ".php")
      this->cgiExtension = 1;
    else
      this->cgiExtension = 2;
}
int HttpRequest::defineTypeMethod(string firstline) {
  firstline = trimNewline(firstline);
  vector<string> words;
  size_t i = 0;
  while (i < firstline.length())
  {
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
  if (words.size() != 3 || words[1][0] != '/')
  {
    this->requestStatus = 400;
    this->endHeaders = 1;
    return 0;
  }
  this->dataFirstLine = words;
  checkPathIscgi(words[1]);
  if (this->checkCgi == 0 && (words[1].find(".php")!=string::npos||words[1].find(".py")!=string::npos)) {
    this->requestStatus = 500;
    this->endHeaders = 1;
    return 0;
  }
  if (words[0] == "GET")
    return (1);
  else if (words[0] == "POST")
    return (2);
  else if (words[0] == "DELETE")
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
void HttpRequest::checkHeaders(string &str)
{
  str = trimNewline(str);
  size_t pos = str.find(':');
  string result;
  string key;
  int i = 0;
  vector<string> words;
  vector<string> hostsize;
  if (pos == string::npos || (pos > 0 && str[pos - 1] == ' '))
  {
    this->requestStatus = 400;
    this->endHeaders = 1;
    return ;
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
  hostsize = splitstring(result);
  key = convertToUpper(headerName);
  if (key == "CONNECTION") {
    this->typeConnection = result;
  }
  // cout <<"key --->"<<key;
  // cout <<"     val --->"<<result<<endl;
  if (key == "HOST") {
    if (hostsize.size() != 1 || this->mapheaders.find(headerName) != this->mapheaders.end()) {
      cout <<"bad request "<<endl;
      this->requestStatus = 400;
      this->endHeaders = 1;
      return ;
    }
  }
  this->mapheaders[headerName] = result;
}

string HttpRequest ::partRquest()
{
  if (this->endHeaders == 1)
    return "";
  string line;
  string str;
  line += this->_buffer;
  this->_buffer.clear();
  size_t pos = line.find("\r\n\r\n");
  if (pos != string::npos)
  {
    str = line.substr(0, pos + 4);
    this->_buffer = line.substr(pos + 4);
  }
  else
  {
    pos = line.rfind("\r\n");
    if (pos != string::npos)
    {
      str = line.substr(0, pos + 2);
      this->_buffer = line.substr(pos + 2);
    }
    else
    {
      this->_buffer = line;
      return "";
    }
  }
  return (str);
}
void HttpRequest ::joinBuffer()
{

  if (this->endHeaders == 1)
    return;
  this->_buffer += this->readBuffer;
  this->readBuffer.clear();
}
/*-----------------*/
string convertToUpper(string str) {
  string result;
    for (size_t i = 0; i < str.size(); i++) {
        result += toupper(str[i]); 
    }
  return result;
}
/*-----------------*/

bool isNonASCII(char c) {
    return static_cast<unsigned char>(c) > 127;
}

void trimSpaces(std::string& str) {
    size_t start = 0;
    while (start < str.length() && (str[start] == ' ' || str[start] == '\t')) {
        ++start;
    }

    size_t end = str.length();
    while (end > start && (str[start] == ' '|| str[end - 1] == '\t')) 
        --end;

    str = str.substr(start, end - start);
}
bool isAllowedUriChar(char c) {
    
    const std::string allowedChars = "-._~:/?#[]@!$&'()*+,;=";
    if (std::isalnum(c) || allowedChars.find(c) != std::string::npos)
        return true;
    return false;
}

int HttpRequest::parseFiledLine(std::string &filedLine)
{
  size_t pos;
  if (filedLine == "\r\n")
    return 0;
  filedLine.pop_back();
  filedLine.pop_back();// to remove crlf
  
  if ( (pos = filedLine.find(":")) == std::string::npos || std::find_if(filedLine.begin(), filedLine.end(), isNonASCII) != filedLine.end())
    return 400;

  std::string filedLineName = filedLine.substr(0, pos);
  while (filedLineName[0] == ' ' || filedLineName[0]=='\t')
    filedLineName.erase(filedLineName.begin()); //remove spaces in the beginning

  if (filedLineName.find_first_of("()/\\<>@,;:\"{} \t\r\n") != std::string::npos)
    return 400;
  for (size_t i = 0; i < filedLineName.size(); i++) 
  { filedLineName[i] = std::toupper(filedLineName[i]); if (filedLineName[i] == '-') filedLineName[i] = '_';}

  // std::cout << "filedLineName : " << filedLineName << std::endl;
  if (mapheaders.find(filedLineName) != mapheaders.end())
  {
    std::vector<std::string> keys; keys.push_back("HOST"); keys.push_back("CONTENT_LENGTH"); keys.push_back("TRANSFER_ENCODING"); keys.push_back("AUTHORIZATION"); keys.push_back("CONNECTION"); keys.push_back("DATE"); keys.push_back("UPGRADE");
    if (std::find(keys.begin(),keys.end(),filedLineName) != keys.end())
      return 400; // std::cout << "I am duplicate\n";
  }
  std::string filedLineValue = filedLine.substr(pos + 1, filedLine.length());
  trimSpaces(filedLineValue);
  // std::cout << "filedLineValue->>" << filedLineValue << "---------\n";
  mapheaders[filedLineName] = filedLineValue;
  return 0;
}

void HttpRequest ::parsePartRequest(string str_parse)
{
  if (this->endHeaders == 1)
    return;
  // std::cout << "str_parse :"; printNonPrintableChars(str_parse);
  std::cout << "\\\\\\\n";

  size_t pos1 = 0;
  size_t pos2 = str_parse.find("\r\n");
  std::string filedLine;

  // 
  while (pos2 != std::string::npos && requestStatus == 0)
  {
    filedLine = str_parse.substr(pos1, pos2 - pos1 + 2);
    requestStatus =  parseFiledLine(filedLine);
    std::cout << "request status " << requestStatus << std::endl;
    pos1 = pos2 + 2;
    pos2 = str_parse.find("\r\n", pos1);
  }

  if (str_parse.find("\r\n\r\n") != std::string::npos)
  {
    this->endHeaders = 1;
    if (mapheaders.find("HOST") == mapheaders.end())
      requestStatus = 400; 
    else
      if (mapheaders["HOST"].empty() ||
        std::find_if(mapheaders["HOST"].begin(), mapheaders["HOST"].end(), isAllowedUriChar) == mapheaders["HOST"].end())
        requestStatus = 400;
  }
  // setMapHeaders();
  // while (!str_parse.empty())
  // {
  //   // ********
  //   size_t pos = str_parse.find("\r\n");
  //   if (pos == string::npos)
  //     break;
  //   string str = str_parse.substr(0, pos + 2);
  //   if (str == "\r\n")
  //   {
  //     this->endHeaders = 1;
  //     break;
  //   }
  //   str_parse = str_parse.substr(pos + 2);
  //   checkHeaders(str);
  //   str.clear();
  // }
}

void HttpRequest ::requestLine() {
  if (this->endHeaders == 1) {
    return ;
  }
  string path;
  this->dataFirstLine[1] = encodeUrl(this->dataFirstLine[1]);
  if (this->dataFirstLine[1].empty()) {
    this->requestStatus = 400;
    this->endHeaders = 1;
    return;
  }
  path = this->dataFirstLine[1];
  if (this->dataFirstLine[2].compare("HTTP/1.1") != 0)
  {
    this->requestStatus = 505;
    this->endHeaders = 1;
    return ;
  }
  size_t pos = path.find("?");
  size_t posHashtag = path.find("#");
  if (posHashtag !=string::npos) {
    this->dataFirstLine[1] = this->dataFirstLine[1].substr(0, posHashtag);
  }
  if (pos != string::npos) {
    this->queryString = this->dataFirstLine[1].substr(pos);
    this->dataFirstLine[1] = this->dataFirstLine[1].substr(0, pos);

  }
  // cout <<"querydata = "<<this->queryString<<endl;
  // cout <<"root cgi = "<<this->rootcgi<<endl;
  // cout <<"path_nfo = "<<this->pathInfo<<endl;
}

string encodeUrl(string &str)
{
  size_t pos = 0;
  string tmp;
  while ((pos = str.find("%", pos)) != string::npos)
  {
    tmp = str.substr(pos + 1, 2);
    char c = characterEncodeing(tmp);
    if (c == 0)
      return "";
    str.replace(pos, 3, 1, c);
    pos++;
  }
  return (str);
}

char characterEncodeing(string &tmp)
{
  printNonPrintableChars(tmp);
  if (tmp[0] < '2' || tmp[0] > '7' || (!isdigit(tmp[1]) && (tmp[1] < 'A' || tmp[1] > 'F'))) {
    return 0;
  }
  return (static_cast<char>(stol(tmp, nullptr, 16)));
}

/*
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