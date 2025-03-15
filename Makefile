CPP = c++

CPPFLAGS = -std=c++98 -Wall -Werror -Wextra

NAME = webserv

SRCS = main.cpp HttpRequest/HttpRequest.cpp

$(NAME): $(SRCS)
	$(CPP) $(CPPFLAGS) -o $(NAME) $(SRCS)

all : $(NAME)

clean :
	rm -rf $(NAME)

re : clean all
