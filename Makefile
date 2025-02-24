NAME = webserve
CPP = c++ -g -fsanitize=address 
CPPFLAGS = #-Wall -Wextra -Werror -std=c++98
RM = rm -f

SRC = main.cpp ./server/ServerSocket.cpp  ./server/WebServer.cpp ./request/HttpRequest.cpp \
	./request/Post/Post.cpp ./request/Post/Chunked.cpp ./request/Post/Boundary.cpp

OBJ = $(SRC:.cpp=.o)
all:$(NAME) rm
$(NAME):$(OBJ)
	$(CPP) $(CPPFLAGS) -o $(NAME) $(OBJ) 

%.o:%.cpp ./server/ServerSocket.hpp  ./server/WebServer.hpp ./request/HttpRequest.hpp \
	./request/Post/Post.hpp ./request/Post/Chunked.hpp ./request/Post/Boundary.hpp 
	$(CPP) $(CPPFLAGS) -c $< -o $@ 

rm:
	$(RM) -f ./data/*

clean:
	$(RM) $(OBJ)
	$(RM) -f ./data/*

fclean:clean
	$(RM) $(NAME)

re:fclean all