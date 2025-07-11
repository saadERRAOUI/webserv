#include "HttpResponse.hpp"
#include <sstream>
/*
	Author: BOUZID Hicham
	Description: function get the lenght of file
	Date: 2025-05-05
*/

long GetLenght(std::string PathFile){
	std::ifstream file(PathFile.c_str(), std::ios::binary);

	if (!file.is_open()){
		std::cerr << "Error opening file: " << PathFile << " - " << strerror(errno) << '\n';
		return (0);
	}
	file.seekg(0, std::ios::end);
	long size = file.tellg();
	file.seekg(0, std::ios::beg);
	file.close();
	return (size);
}

/*
	Author: BOUZID Hicham
	Description: open file and return lenght
	Date: 2025-05-21
*/

std::string    OpenFile(std::string PathFile, bool status, Connection* Infos, const std::string prefix)
{
    char BUFFER[8001] = {0};
    std::string line, rt;

    const char* BUFFER1 = prefix.c_str();  // Use the passed prefix

    if (status == true)
    {
        std::ifstream *fd = new std::ifstream(PathFile.c_str(), std::ios::binary);

        if (!fd->is_open())
        {
            std::cerr << "Error opening file: " << strerror(errno) << '\n';
            return (std::string(""));
        }

        Infos->Setfile(*fd);
        
        // First, send the HTTP headers
        write(Infos->Getfd(), BUFFER1, strlen(BUFFER1));
        
        // Then read and send the file content in chunks
        long totalBytesRead = 0;
        long fileSize = GetLenght(PathFile);
        
        while (totalBytesRead < fileSize) {
            unsigned long remainingBytes = fileSize - totalBytesRead;
            long bytesToRead = (remainingBytes < sizeof(BUFFER)) ? remainingBytes : sizeof(BUFFER);
            
            Infos->GetFile()->read(BUFFER, bytesToRead);
            std::streamsize bytesRead = Infos->GetFile()->gcount();
            
            if (bytesRead <= 0) break;
            
            write(Infos->Getfd(), BUFFER, bytesRead);
            totalBytesRead += bytesRead;
        }
        
        Infos->GetFile()->close();
        delete fd;
        Infos->SetSize(0);
        // Don't call DefSize since we've already closed the file

        return std::string("");  // Return empty string since we've already sent everything
    }

    // For non-status case (shouldn't be used for file serving)
    Infos->GetFile()->read(BUFFER, sizeof(BUFFER));

    // Using std::vector<char> to concatenate BUFFER1 (prefix) and BUFFER (binary content)
    std::vector<char> combined;
    combined.insert(combined.end(), BUFFER1, BUFFER1 + strlen(BUFFER1));  // Insert prefix
    combined.insert(combined.end(), BUFFER, BUFFER + Infos->GetFile()->gcount());  // Insert file content

    // Write the combined buffer to the connection
    write(Infos->Getfd(), combined.data(), combined.size());

    Infos->SetSize(Infos->GetFile()->gcount());
    Infos->DefSize(0);

    return std::string(combined.begin(), combined.end());  // Return the combined result as a string
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
				or ours error pages.
	Date: 2025-04-21
*/

std::string ErrorBuilder(Connection *Infos, Server *tmpServer, int code)
{
    std::cout << "[DEBUG] ErrorBuilder: called with code=" << code << std::endl;
    std::string errorPagePath = chose_one(tmpServer->webServ.getErrorPages()[code], tmpServer->getErrorPages()[code]);
    std::cout << "[DEBUG] ErrorBuilder: error page path='" << errorPagePath << "'" << std::endl;
    std::string errorBody;
    
    // Try to read the error page file
    std::ifstream errorFile(errorPagePath.c_str());
    if (errorFile.is_open()) {
        std::cout << "[DEBUG] ErrorBuilder: successfully opened error page file" << std::endl;
        std::stringstream buffer;
        buffer << errorFile.rdbuf();
        errorBody = buffer.str();
        errorFile.close();
        std::cout << "[DEBUG] ErrorBuilder: read " << errorBody.length() << " bytes from error page" << std::endl;
    } else {
        std::cout << "[DEBUG] ErrorBuilder: failed to open error page file: " << strerror(errno) << std::endl;
        // Fallback to generic error message if file can't be read
        errorBody = "<html><body><h1>" + tostring(code) + " " + Infos->GetResponse().GetStatusCode(code) + "</h1><p>Error occurred while processing your request.</p></body></html>";
    }
    
    std::string response = Infos->GetRequest().getVersion();
    response += " " + tostring(code) + " ";
    response += Infos->GetResponse().GetStatusCode(code);
    response += "\r\n";
    response += "Content-Type: text/html\r\n";
    if (code == 301) {
        std::string location = Infos->GetRequest().getHeader("Location");
        if (!location.empty()) {
            response += "Location: " + location + "\r\n";
        }
    }
    response += "Content-Length: " + tostring(errorBody.length()) + "\r\n";
    response += "Connection: close\r\n";
    response += "\r\n";
    response += errorBody;
    
    write(Infos->Getfd(), response.c_str(), response.length());
    
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
