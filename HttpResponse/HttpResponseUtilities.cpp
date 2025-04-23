#include "HttpResponse.hpp"

/*
	Author: BOUZID Hicham
	Description: open file and return lenght
	Date: 2025-05-21
*/

std::string OpenFile(std::string PathFile, int status)
{
	std::ifstream fd(PathFile.c_str());
	std::string line, rt;
	(void)status;

	if (!fd.is_open())
	{
		std::cerr << "Error file : " << strerror(errno) << '\n';
		return (std::string(""));
	}
	std::getline(fd, line);
	rt = line + "\n";
	while (std::getline(fd, line))
	{
		rt += (line + "\n");
	}
	fd.close();
	return (rt);
}


/*
	Author: BOUZID Hicham
	Description: check if the server name that we recieved
				found on our config file or not
	Date: 2025-04-21
*/
bool HostName(Server *tmpServer, std::string name)
{
	std::cout << "the name of server is : " << name << '\n';
	std::cout << "the first server name is: " << *(tmpServer->getServerName().begin()) << '\n';
	std::vector<std::string>::iterator it = find(tmpServer->getServerName().begin(), tmpServer->getServerName().end(), name);
	if (it == tmpServer->getServerName().end())
		return (false);
	return (true);
}


/*
	Author: BOUZID Hicham
	Description: build a response for error stats
				and serve the default error pages
				of ours error pages.
	Date: 2025-04-21
*/
std::string ErrorBuilder(Connection *Infos, Server *tmpServer, int code)
{
	std::map<std::string, std::string> tmp_map = Infos->GetRequest().getHeaders();
	std::string response = Infos->GetRequest().getVersion();

	response += " " + tostring(code) + " ";
	response += Infos->GetResponse().GetStatusCode(code);
	response += "\r\n";
	for (std::map<std::string, std::string>::iterator it = tmp_map.begin(); it != tmp_map.end(); it++)
	{
		response += it->first;
		response += ": ";
		response += it->second;
		response += "\r\n";
	}
	std::string rt = OpenFile(std::string("./www/html/ErrorPages/") + tmpServer->getErrorPages()[code], code);
	response += "Content-Length: " + tostring((int)rt.size());
	response += "\r\n\r\n";
	response += rt;
	return (response);
}

/*
	Author: BOUZID Hicham
	Description: convert number to std::string
	Date: 2025-04-22
*/

std::string tostring(int number)
{
	std::ostringstream str;

	str << number;
	std::string converted = str.str();
	return (converted);
}
