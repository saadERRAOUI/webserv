NAME = webserv

CXX = c++

CPPFLAGS = -Wall -Wextra -Werror -std=c++98

CPPFILS = SetupServer/StartServerSetup.cpp SetupServer/MultipSockets.cpp

OBJS = $(CPPFILS:.cpp=.o)

all : $(NAME)

$(NAME) : $(OBJS)
	$(CXX) $(CPPFLAGS) $(OBJS) -o $(NAME)

clean:
	rm -rf $(OBJS)

fclean: clean
	rm -rf $(NAME)

re: fclean all
