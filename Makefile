NAME = webserver
CPP = c++ -std=c++98 #-fsanitize=address -g
CPPFLAGS = -Wall -Wextra -Werror
RM = rm -f

# Colors
GREEN = \033[1;32m
CYAN = \033[1;36m
YELLOW = \033[1;33m
RESET = \033[0m

SRC = \
	./request/Post/Post.cpp ./request/Post/Chunked.cpp ./request/Post/Boundary.cpp ./request/Post/BoundaryChunked.cpp \
	main.cpp ./server/ServerSocket.cpp ./server/WebServer.cpp ./request/HttpRequest.cpp \
	./serverConf/ServerConfig.cpp ./response/HttpResponse.cpp \

OBJ = $(SRC:.cpp=.o)

all: $(NAME)

$(NAME): $(OBJ)
	@echo "$(GREEN)◎ Linking:$(RESET) $(CYAN)$(NAME)$(RESET)"
	@$(CPP) $(CPPFLAGS) -o $(NAME) $(OBJ)
	@echo "$(GREEN)✔ Build complete$(RESET)"

%.o: %.cpp ./server/ServerSocket.hpp ./server/WebServer.hpp ./request/HttpRequest.hpp \
	./request/Post/Post.hpp ./request/Post/Chunked.hpp ./request/Post/Boundary.hpp ./request/Post/BoundaryChunked.hpp \
	./response/HttpResponse.hpp ./serverConf/ServerConfig.hpp
	@printf "$(YELLOW)→ Compiling:$(RESET) $(CYAN)$<$(RESET)\r"
	@$(CPP) $(CPPFLAGS) -c $< -o $@

clean:
	@echo "$(YELLOW)🧹 Cleaning object files...$(RESET)"
	@$(RM) $(OBJ)

fclean: clean
	@echo "$(YELLOW)🧹 Removing binary:$(RESET) $(CYAN)$(NAME)$(RESET)"
	@$(RM) $(NAME)

re: fclean all
