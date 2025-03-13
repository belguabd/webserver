#include "HttpRequest.hpp"

LocationUplaods &getMatchedLocationUpload(const std::string &path, map<string, LocationUplaods> &configUploads)
{
	size_t pos = path.find("/", 1);
	string keyLocationUpload;

    if (pos == std::string::npos)
      pos = path.size() + 1;
	keyLocationUpload = path.substr(0, pos);
	if (configUploads.find(keyLocationUpload) == configUploads.end())
		configUploads[keyLocationUpload] = (LocationUplaods){.upload_store = UPLOAD_FOLDER}; // 
	return (configUploads.find(keyLocationUpload))->second;
}

void HttpRequest::handlePost()
{
  if (_post == NULL)
  {
    LocationUplaods &lc = getMatchedLocationUpload(dataFirstLine[1], server_config.getConfigUpload());
    
	  std::cout << "lc: " << lc.upload_store << std::endl;
    _post = new Post(mapheaders, queryParam, _buffer, lc);
  }
  else
  {
    _post->proseRequest(readBuffer);
    std::cout << "abcd\n";
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
  joinBuffer();
  str_parse = partRquest();
  if (getFirstTimeFlag() == 0)
  {
    size_t pos = str_parse.find("\r\n");
    if (pos != string::npos)
    {
      setFirstTimeFlag(1);
      _method = defineTypeMethod(str_parse.substr(0, pos + 2));
      requestLine();
      str_parse = str_parse.substr(pos + 2);
      if (_method == DELETE)
        std::cout << "handle request number: " << handleDeleteRequest(dataFirstLine[1]) << std::endl;
    }
  }
  parsePartRequest(str_parse);

  if ((_method == GET || _method == DELETE) && getendHeaders() == 1)
    setRequestStatus(1);
  else if (_method == POST && getendHeaders() == 1)
    handlePost();
}

HttpRequest::HttpRequest(int client_fd, ServerConfig &server_config)
    : client_fd(client_fd), firsttime(0), endHeaders(0), _method(NONE) , server_config(server_config){
  int flags = fcntl(client_fd, F_GETFL, 0);
  fcntl(client_fd, F_SETFL, flags | O_NONBLOCK);
  // server_config.
  _post = NULL;
}

int HttpRequest::readData()
{
  char buffer[5024];
  ssize_t bytes_received;
  std::memset(buffer, 0, sizeof(buffer));
  bytes_received = recv(client_fd, buffer, sizeof(buffer), 0);
  if (bytes_received > 0)
  {
    readBuffer.assign(buffer, bytes_received);
    // std::cout << readBuffer << "\n";
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

HttpRequest::~HttpRequest() 
{
  delete _post;
}
int HttpRequest::defineTypeMethod(string firstline)
{
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
    cout << "method error" << endl;
    this->requestStatus= 400;
    // exit(0);
  }
  this->dataFirstLine = words;
  puts("================================");
  cout << words[1] <<"\n";
  puts("================================");
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
  vector<string> words;
  if (pos == string::npos || (pos > 0 && str[pos - 1] == ' '))
  {
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

void HttpRequest ::parsePartRequest(string str_parse)
{
  if (this->endHeaders == 1)
    return;
  while (!str_parse.empty())
  {
    size_t pos = str_parse.find("\r\n");
    if (pos == string::npos)
      break;
    string str = str_parse.substr(0, pos + 2);
    if (str == "\r\n")
    {
      this->endHeaders = 1;
      break;
    }
    str_parse = str_parse.substr(pos + 2);
    checkHeaders(str);
    str.clear();
  }
}
void HttpRequest ::requestLine()
{
  string path;
  string querydata;
  this->dataFirstLine[1] = encodeUrl(this->dataFirstLine[1]);
  path = this->dataFirstLine[1];
  if (this->dataFirstLine[2].compare("HTTP/1.1") != 0)
  {
    cout << "Not Supported" << endl;
    return;
  }
  size_t pos = path.find("?");
  if (pos == string::npos)
  {
    pos = path.find("#");
    if (pos == string::npos)
    {
      return;
    }
    else
    {
      this->dataFirstLine[1] = this->dataFirstLine[1].substr(0, pos);
    //   cout << "-- > " << this->dataFirstLine[1] << endl;
      return;
    }
  }
  if (this->dataFirstLine[1][pos + 1] == '\0')
  {
    cout << "erro ??" << endl;
    this->dataFirstLine[1] = this->dataFirstLine[1].substr(0, pos);
    return;
  }
  querydata = this->dataFirstLine[1].substr(pos + 1);
  this->dataFirstLine[1] = this->dataFirstLine[1].substr(0, pos);
  size_t i = 0;
  while (i < querydata.length())
  {
    size_t endkey = querydata.find("=", i);
    size_t endval = querydata.find("&", i);
    if (endkey != string::npos)
    {
      string key = querydata.substr(i, endkey - i);
      string value;
      if (endval != string::npos)
        value = querydata.substr(endkey + 1, endval - endkey - 1);
      else
        value = querydata.substr(endkey + 1);
    //   cout << "Key: " << key << ", Value: " << value << endl;
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
  while ((pos = str.find("%", pos)) != string::npos)
  {
    tmp = str.substr(pos + 1, 2);
    char c = characterEncodeing(tmp);
    str.replace(pos, 3, 1, c);
    pos++;
  }
  return (str);
}

char characterEncodeing(string &tmp)
{
  // cout <<"tmp [0] = "<<tmp[0]<<endl;
  // cout <<"tmp +[1] = "<<endl;
  printNonPrintableChars(tmp);
  if (tmp[0] < '2' || tmp[0] > '7' || (!isdigit(tmp[1]) && (tmp[1] < 'A' || tmp[1] > 'F')))
  {
    cout << "character not allowed" << endl;
    exit(0);
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