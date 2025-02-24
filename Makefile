NAME = webserver
CPP = c++ -fsanitize=address -g -std=c++98
CPPFLAGS = #-Wall -Wextra -Werror -std=c++98
RM = rm -f

SRC = main.cpp ./server/ServerSocket.cpp  ./server/WebServer.cpp ./request/HttpRequest.cpp ./response/HttpResponse.cpp ./conf/ServerConfig.cpp

OBJ = $(SRC:.cpp=.o)
all:$(NAME)
$(NAME):$(OBJ)
	$(CPP) $(CPPFLAGS) -o $(NAME) $(OBJ) 

%.o:%.cpp ./server/ServerSocket.hpp  ./server/WebServer.hpp ./request/HttpRequest.hpp ./response/HttpResponse.hpp ./conf/ServerConfig.hpp
	$(CPP) $(CPPFLAGS) -c $< -o $@ 

clean:
	$(RM) $(OBJ)

fclean:clean
	$(RM) $(NAME)

re:fclean all