CC = c++
CFLAGS = -std=c++98 -Wall -Werror -Wextra #-g -fsanitize=address,undefined,integer
NAME = webserv
HEADER = header.hpp\
	./server/server.hpp \
	./request/request.hpp \
	./response/response.hpp \
	./client/client.hpp \
	./parsing/headers/CheckMethods.hpp \
	./parsing/headers/ClientMaxBodySize.hpp \
	./parsing/headers/ConfigFileParser.hpp\
	./parsing/headers/ConfigValues.hpp \
	./parsing/headers/Index.hpp \
	./parsing/headers/Listen.hpp \
	./parsing/headers/LocationContext.hpp \
	./parsing/headers/LocationUri.hpp \
	./parsing/headers/ParseCGI.hpp \
	./parsing/headers/ReturnDir.hpp \
	./parsing/headers/Root.hpp \
	./parsing/headers/ServerContext.hpp \
	./parsing/headers/ServerName.hpp \
	./parsing/headers/TargetRequestParser.hpp \
	./parsing/headers/Uri.hpp \
	./parsing/headers/UriParser.hpp \
	./parsing/headers/utils.hpp \
	./cgi/headers/CGI.hpp

SRC = ./main.cpp \
	./server/server.cpp \
	./client/client.cpp \
	./request/request.cpp \
	./response/response.cpp \
	./parsing/CheckMethods.cpp \
	./parsing/ClientMaxBodySize.cpp \
	./parsing/ConfigFileParser.cpp\
	./parsing/ConfigValues.cpp \
	./parsing/Index.cpp \
	./parsing/Listen.cpp \
	./parsing/LocationContext.cpp \
	./parsing/LocationUri.cpp \
	./parsing/ParseCGI.cpp \
	./parsing/ReturnDir.cpp \
	./parsing/Root.cpp \
	./parsing/ServerContext.cpp \
	./parsing/ServerName.cpp \
	./parsing/TargetRequestParser.cpp \
	./parsing/Uri.cpp \
	./parsing/UriParser.cpp \
	./parsing/utils.cpp \
	./cgi/CGI.cpp


OBJ = $(SRC:.cpp=.o)

all: $(NAME)

$(NAME) : $(OBJ)
	$(CC) $(CFLAGS) $(OBJ) -o $(NAME)

%.o : %.cpp $(HEADER)
	$(CC) $(CFLAGS) -o $@ -c $<

clean:
	rm -rf $(OBJ)

fclean: clean
	rm -rf $(NAME)

re : fclean all


#
# ./server.hpp \
# ./request.hpp \
# ./response.hpp \
# ./client.hpp \
