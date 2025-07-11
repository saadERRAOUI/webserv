
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

int parse_positive_int(std::string string)
{
    int x = parseInt(string);
    if (x < 0)
        throw std::invalid_argument("Invalid positive integer");
    return x;
}

std::string parseString(int integer)
{
    std::stringstream ss;
    ss << integer;
    return ss.str();
}