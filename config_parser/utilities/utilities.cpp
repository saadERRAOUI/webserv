
#include <string>
#include <sstream>
int parseInt(std::string string)
{
    std::stringstream ss(string);
    int x;
    ss >> x;
    if (ss.fail() || ss.rdbuf()->in_avail() != 0)
        throw std::invalid_argument("Invalid integer");
    return x;
}

std::string parseString(int integer)
{
    std::stringstream ss;
    ss << integer;
    return ss.str();
}