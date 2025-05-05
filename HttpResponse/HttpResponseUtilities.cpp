#include "HttpResponse.hpp"
/*
	Author: BOUZID Hicham
	Description: function get the lenght of file
	Date: 2025-05-05
*/

long GetLenght(std::string PathFile){
	std::ifstream file(PathFile.c_str(), std::ios::binary);

	if (!file.is_open()){
		std::cerr << "Error file: " << strerror(errno) << '\n';
		return (-1);
	}
	file.seekg(0, std::ios::end);
	long size = file.tellg();
	file.close();
	return (size);
}

/*
	Author: BOUZID Hicham
	Description: open file and return lenght
	Date: 2025-05-21
*/

std::string OpenFile(std::string PathFile, bool status, Connection *Infos)
{
	char BUFFER[8001] = {0};
	std::string line, rt;

	std::cout << "here.\n";
	if(status == true)
	{
		std::ifstream fd(PathFile.c_str(), std::ios::binary);

		if (!fd.is_open())
		{
			std::cerr << "Error file : " << strerror(errno) << '\n';
			return (std::string(""));
		}
		fd.read(BUFFER, 8000);
		Infos->Setfile(fd);
		Infos->DefSize(strlen(BUFFER));
		return (std::string(BUFFER));
	}
	Infos->GetFile()->read(BUFFER, 8000);
	Infos->DefSize(strlen(BUFFER));
	return (std::string(BUFFER));
}


/*
	Author: BOUZID Hicham
	Description: check if the server name that we recieved
				found on our config file or not
	Date: 2025-04-21
*/
bool HostName(Server *tmpServer, std::string name)
{
	// std::cout << "the name of server is : " << name << '\n';
	// std::cout << "the first server name is: " << *(tmpServer->getServerName().begin()) << '\n';
	std::vector<std::string>::iterator it = find(tmpServer->getServerName().begin(), tmpServer->getServerName().end(), name);
	if (it == tmpServer->getServerName().end())
		return (false);
	return (true);
}

/*
	Author: BOUZID Hicham
	Description: chose if the default error pages
				found on config file of we should serve ours
	Date: 2025-04-23
*/
std::string chose_one(std::string a, std::string b){
	if (!a.empty() && !access(a.c_str(), R_OK))
		return (a);
	return (std::string("./www/html/ErrorPages/") + b);
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
	std::string DefaultOrOurs;

	DefaultOrOurs = chose_one(tmpServer->webServ.getErrorPages()[code], tmpServer->getErrorPages()[code]);
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
	std::string rt = OpenFile(DefaultOrOurs, true, Infos);
	response += "Content-Length: " + tostring((int)rt.size());
	response += "\r\n\r\n";
	response += rt;
	if (code != 301)
		Infos->SetBool(true);
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
