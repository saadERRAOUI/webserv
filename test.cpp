#include <iostream>
#include <fstream>
#include <string.h>

long GetLenght(std::string PathFile)
{
	std::ifstream file(PathFile.c_str(), std::ios::binary);

	if (!file.is_open())
	{
		std::cerr << "Error file: " << strerror(errno) << '\n';
		return (-1);
	}
	file.seekg(0, std::ios::end);
	long size = file.tellg();
	file.close();
	return (size);
}

int main(){
	std::cout << "The size of this image: " << GetLenght("www/html/pepe/api/v1/to_test/tree.jpg") << '\n';
}
