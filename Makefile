CPP = c++

CPPFLAGS = -std=c++98 -Wall -Werror -Wextra -g3 #-fsanitize=address

NAME = webserv

SRCS = main.cpp ./config_parser/*.cpp ./config_parser/utilities/*.cpp ./Connection/Connection.cpp\
		./HttpResponse/HttpResponse.cpp ./HttpResponse/HttpResponseUtilities.cpp\
		./HttpRequest/HttpRequest.cpp SetupServer/MultipSockets.cpp HttpResponse/GetMethod.cpp\
		./HttpRequest/HttpRequestParser.cpp

$(NAME): $(SRCS)
	$(CPP) $(CPPFLAGS) -I ./Includes $(SRCS) -o $(NAME)

all : $(NAME)

clean :
	rm -rf $(NAME)

fclean : clean

re : clean all
