/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   httpRequest.cpp                                    :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: ataoufik <ataoufik@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/02/10 10:27:32 by ataoufik          #+#    #+#             */
/*   Updated: 2025/02/12 12:25:12 by ataoufik         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include"httpRequest.hpp"
httpRequest::httpRequest(int client_fd) : client_fd(client_fd) ,flags(0) {
  int flags = fcntl(client_fd, F_GETFL, 0);
  fcntl(client_fd, F_SETFL, flags | O_NONBLOCK);
}

int httpRequest::readData() {
  char buffer[4024];
  ssize_t bytes_received = recv(client_fd, buffer, sizeof(buffer), 0);
  if (bytes_received > 0) {
    buffer[bytes_received] = '\0';
    readBuffer.append(buffer, bytes_received);
  }
  std::cout << readBuffer << "\n";
  if (bytes_received == 0)
    std::cout << "Client disconnected" << std::endl;
  return bytes_received;
}

int httpRequest::writeData() {
  const char *msg = "Hi I am server";
  ssize_t bytes_send = send(client_fd, msg, strlen(msg), 0);
  if (bytes_send == -1)
    std::cerr << "Error sending message to client" << std::endl;
  return bytes_send;
}
httpRequest::~httpRequest() {

}

int httpRequest:: defineTypeMethod(const string firstline)
{
    vector<string> words;
    size_t start;
    size_t i = 0, j;
    while (i < firstline.length())
    {
        start = i;
        while (i < firstline.length() && firstline[i] != ' ')
        {
            if(firstline[i] == '\t')
            {
                cout <<"Bad Request"<<endl;
                exit(1);
            }
            i++;
        }
        if(firstline[i] == ' ')
        {
            if(firstline[i + 1] == ' '||firstline[i + 1] == '\t')
            {
                cout <<"Bad Request"<<endl;
                exit(1);
            }
        }

        words.push_back(firstline.substr(start, i - start));
        i++;
    }
    if (words[0]=="GET")
        return 1;
    else if (words[0]=="POST")
        return 2;
    else if (words[0]=="DELETE")
        return 3;
    cout <<"Bad Request"<<endl;
    // words[2] ??? check
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

string httpRequest :: checkHeaders(const string& str)
{
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
    if((str.length() != 0 &&(pos == string::npos) || str[pos - 1]==' '||str[pos - 1]=='\t'))
    {
        cout<<"Bad Request"<<endl;
        exit(1);
    }
    result += str.substr(0,pos + 1);
    words = splitstring(str.substr(pos + 1,str.length()));
    for (vector<string>::const_iterator it = words.begin(); it != words.end(); ++it) {
        const string &words = *it;
        result += ' ';
        result += words;
    }
    result +="\r\n";
    return result;
}

string httpRequest :: get_line(string line)
{
    line +=this->buffer;
    string str;
    this->buffer.clear();
    size_t pos = line.find('\r');
    if (pos != string::npos)
        str = line.substr(0,pos +1);
    else{
        this->buffer = line.substr(pos +1); 
        return "";
    }
    this->buffer = line.substr(pos +1); 
    return (str);
}

void httpRequest :: joinbuffer()
{
    this->buffer += this->readBuffer;
    std::cout<<"------ > "<< this->readBuffer <<std::endl;
    this->readBuffer.clear();
}

