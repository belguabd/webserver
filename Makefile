NAME = webserver
CPP = c++ -std=c++98 #-fsanitize=address -g
CPPFLAGS =  -Wall -Wextra -Werror
RM = rm -f

SRC = \
	./request/Post/Post.cpp ./request/Post/Chunked.cpp ./request/Post/Boundary.cpp ./request/Post/BoundaryChunked.cpp  \
	main.cpp ./server/ServerSocket.cpp  ./server/WebServer.cpp ./request/HttpRequest.cpp \
	 ./conf/ServerConfig.cpp ./response/HttpResponse.cpp \

OBJ = $(SRC:.cpp=.o)
all:$(NAME)
$(NAME):$(OBJ)
	$(CPP) $(CPPFLAGS) -o $(NAME) $(OBJ) 

%.o:%.cpp ./server/ServerSocket.hpp  ./server/WebServer.hpp ./request/HttpRequest.hpp \
	./request/Post/Post.hpp ./request/Post/Chunked.hpp ./request/Post/Boundary.hpp ./request/Post/BoundaryChunked.hpp  \
	./response/HttpResponse.hpp ./conf/ServerConfig.hpp
	$(CPP) $(CPPFLAGS) -c $< -o $@ 

rm:
	$(RM) -f ./upload/*

clean:
	$(RM) $(OBJ)
	$(RM) ./upload/*
	$(RM) ./currentRequest
	$(RM) ./out.yaml

fclean:clean
	$(RM) $(NAME)

re:fclean all