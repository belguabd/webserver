NAME = webserve
CPP = c++ -g -fsanitize=address
CPPFLAGS = #-Wall -Wextra -Werror -std=c++98
RM = rm -f

SRC = main.cpp ./server/ServerSocket.cpp  ./server/WebServer.cpp ./request/HttpRequest.cpp \
	./request/Post/Post.cpp ./request/Post/Chunked.cpp

OBJ = $(SRC:.cpp=.o)
all:$(NAME)
$(NAME):$(OBJ)
	$(CPP) $(CPPFLAGS) -o $(NAME) $(OBJ) 

%.o:%.cpp ./server/ServerSocket.hpp  ./server/WebServer.hpp ./request/HttpRequest.hpp ./request/Post/Post.hpp ./request/Post/Chunked.hpp 
	$(CPP) $(CPPFLAGS) -c $< -o $@ 

clean:
	$(RM) $(OBJ)

fclean:clean
	$(RM) $(NAME)
	$(RM) -f ./data/*

re:fclean all