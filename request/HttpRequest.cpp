/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   HttpRequest.cpp                                    :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: ataoufik <ataoufik@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/02/10 10:27:32 by ataoufik          #+#    #+#             */
/*   Updated: 2025/02/14 18:12:13 by ataoufik         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "HttpRequest.hpp"
void    handleRequest(HttpRequest &request)
{
    string str_parse;
    // if (this->flaaaa = 1)
    // {
    //   request._post.fun(readBuffer);
    //   // fun(request._post,readBuffer);
    // }
    request.joinBuffer();
    str_parse = request.partRquest();
    if (request.getFirstTimeFlag() == 0)
    {
        size_t pos = str_parse.find("\r\n");
        if (pos != string::npos)
        {
          request.setFirstTimeFlag(1);
          int f = request.defineTypeMethod(str_parse.substr(0,pos + 2));
          if (f == 1)
            std::cout<< "Method --->> GET  <---"<<std::endl;
          else if (f == 2)
            std::cout<< "Method --->> POST  <---"<<std::endl;
          else if (f == 3)
            std::cout<< "Method --->> DELETE  <---"<<std::endl;
          // size_t pos = str_parse.find("\r\n");
          str_parse = str_parse.substr(pos + 2);

        }
        // cout <<str_parse<<endl;
        // exit(0);
    }
    request.parsePartRequest(str_parse);
      
    if (request.getbuffer().empty())
    {
      for (auto it = request.mapheaders.begin(); it != request.mapheaders.end(); ++it) {
        std::cout << "key =  " << it->first << "-->  "<< "value = " << it->second << std::endl;
      }
    }
    }

HttpRequest::HttpRequest(int client_fd) : client_fd(client_fd) ,firsttime(0) {
  int flags = fcntl(client_fd, F_GETFL, 0);
  fcntl(client_fd, F_SETFL, flags | O_NONBLOCK);
  
}


int HttpRequest::readData()
{
  char buffer[40];
  ssize_t bytes_received;
  
  while((bytes_received = recv(client_fd, buffer, sizeof(buffer) - 1, 0)) > 0)
  {
    if (bytes_received > 0)
    {
      buffer[bytes_received] = '\0';
      readBuffer = buffer;
      handleRequest(*this);
      
    }
    else if (bytes_received == 0)
    {
      std::cout << "Client " << client_fd << " disconnected\n";
      return -1;
    }
    else
    {
      std::cerr << "Error receiving data from client " << client_fd << ": " << strerror(errno) << "\n";
      return -1;
    }
  }
  return bytes_received;
}

int HttpRequest::writeData() {
    int bytes_send = 0;
//   const char *msg = "Hi I am server";
//   ssize_t bytes_send = send(client_fd, msg, strlen(msg), 0);
//   if (bytes_send == -1)
//     std::cerr << "Error sending message to client" << std::endl;
  return bytes_send;
}
HttpRequest::~HttpRequest() {}
int HttpRequest::defineTypeMethod(const string firstline) {
  vector<string> words;
  size_t start;
  size_t i = 0;
  while (i < firstline.length()) {
    start = i;
    while (i < firstline.length() && firstline[i] != ' ') {
      if (firstline[i] == '\t') {
        cout << "Bad Request" << endl;
        exit(1);
      }
      i++;
    }
    if (firstline[i] == ' ') {
      if (firstline[i + 1] == ' ' || firstline[i + 1] == '\t') {
        cout << "Bad Request" << endl;
        exit(1);
      }
    }
    // std::cout << " i  = "<< i <<std::endl;
    // std::cout << " start  = "<< start <<std::endl;
    words.push_back(firstline.substr(start, i - start));
    i++;
  }
  if (words[0] == "GET")
    return 1;
  else if (words[0] == "POST")
    return 2;
  else if (words[0] == "DELETE")
    return 3;
  cout << "Bad Request" << endl;
  // words[2] ??? check
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
void  checkHeaders(string& str, map<string, string>& headersMap) {
  // if (str.length() == 0)
  //     this->sig++;
  // if (this->sig >1)
  // {
  //     cout<<"Forbidden"<<endl;
  //     exit(1);
  // }
  size_t pos = str.find(':');
  string result;
  vector<string> words;
  // if ((str.length() != 0 && (pos == string::npos) || str[pos - 1] == ' ' ||
  //      str[pos - 1] == '\t')) {
  //   cout << "Bad Request" << endl;
  //   exit(1);
  // }
  // result += str.substr(0, pos + 1);
  words = splitstring(str.substr(pos + 1, str.length()));
  for (vector<string>::const_iterator it = words.begin(); it != words.end();
       ++it) {
    const string &words = *it;
    result += ' ';
    result += words;
  }
  headersMap[str.substr(0, pos + 1)] = result;
}

string HttpRequest :: partRquest()
{
  string line;
  string str;
  line += this->_buffer;
  this->_buffer.clear();
  size_t pos = line.rfind("\r\n");
  if (pos != string::npos) {
    str = line.substr(0,pos + 2);
    this->_buffer = line.substr(pos + 2);
  }
  else {
      this->_buffer = line;
      return "";
    }
    return (str);
}

void HttpRequest :: joinBuffer()
{
    this->_buffer += this->readBuffer;
    this->readBuffer.clear();
}

void HttpRequest :: parsePartRequest(string str_parse)
{
  // cout<<" -      ->  "<<str_parse <<endl;
  while(!str_parse.empty())
  {
    size_t pos = str_parse.find("\r\n");
    string str = str_parse.substr(0,pos + 2);
    // cout << " str =   "<<str<<endl;
    str_parse = str_parse.substr(pos + 2);
    // cout << " str_parse =   "<<str_parse<<endl;
    checkHeaders(str,this->mapheaders);
    str.clear();
    // checkHeaders()
    // if (str =="\r\n")
    //   exit(0);
  }
  
}
// GET / HTTP/1.1"\r\n"
// Host: localhost:8080"\r\n"
// U ser-Agent'\0'