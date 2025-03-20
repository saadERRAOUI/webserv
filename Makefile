CPP = c++


CPPFLAGS = -std=c++98 -Wall -Werror -Wextra


NAME = webserv

SRCS = main.cpp ./config_parser/*.cpp ./config_parser/utilities/*.cpp

$(NAME): $(SRCS)
	$(CPP) $(CPPFLAGS) -I ./Includes $(SRCS) -o $(NAME)

all : $(NAME)

clean :
	rm -rf $(NAME)

re : clean all
