#include "Connection.hpp"

Connection::Connection(int fd, Server server){
	fd_client = fd;
	s = &server;
	Request = NULL;
}
