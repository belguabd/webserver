#include "HttpResponse.hpp"

void    sendResponse(HttpResponse &response)
{


}
HttpResponse::HttpResponse() {}

HttpResponse:: ~HttpResponse() {}


int HttpResponse::writeData() {
    int bytes_send = 0;
    sendResponse(*this);
//   const char *msg = "Hi I am server";
//   ssize_t bytes_send = send(client_fd, msg, strlen(msg), 0);
//   if (bytes_send == -1)
//     std::cerr << "Error sending message to client" << std::endl;
  return bytes_send;
}