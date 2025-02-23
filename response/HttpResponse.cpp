#include "HttpResponse.hpp"


void	status_line(int client_socket) {
	std::string status;
	  status = "HTTP/1.1 200 OK\r\n";
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
	std::cout << header;
	send(client_socket, header.c_str(), header.size(), 0);
}
void    sendResponse(HttpResponse &response)
{
  int sig = response.request->sig;
  cout << "---- > "<<sig <<endl;
  // if (sig == 1)
  //   getResponse();
  // else if ()
  // postRseponse();
  // else if ()
  //   deleteResponse();
  	std::ifstream file("/Users/ataoufik/Desktop/webserver/doc/v/html/index.html");
    std::stringstream fileContent;
        
    if (file) {
        fileContent << file.rdbuf(); 
        file.close();
    } else {
        fileContent << "<html><body><h1>404 Not Found</h1></body></html>";
    }
    status_line(response.request->getfd());
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