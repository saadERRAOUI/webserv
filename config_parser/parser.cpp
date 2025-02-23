#include "parser.hpp"
#include <cstdio>
#include <memory>
#include <iostream>
ConfigParser::ConfigParser(std::string path)
    : file_path(path), file(path.c_str()), globalSection("global"),
      current_section(&globalSection) {
  state.g_state = INITIAL;
  state.string_state = OUT_QUOTES;
  state.value_substate = IN_STRING;
  state.substate = PARSING_INITIAL;
  if (!file.is_open())
    throw std::invalid_argument("could not open the file!");
}

std::string trim(std::string &line) {
    size_t start = line.find_first_not_of(" \t");
    if (start == std::string::npos)
        return "";
  return line.substr(line.find_first_not_of(" \t"),
                     line.find_last_not_of(" \t") + 1);
}

bool ConfigParser::isComment(char c) {
  if ((c == '#' && state.string_state != OUT_QUOTES))
    return true;
  return false;
}

void ConfigParser::determine_state(std::string &line) {
  if (state.substate == PARSING_INITIAL) {
    if (line.compare(0,2,"[[") == 0 && line.compare( line.length() - 2, 2, "]]"))
      state.substate = PARSING_HEADER;
    else
      state.substate = PARSING_KEY;
  }
}

void ConfigParser::throw_error(std::string error) {
  std::string message = "Error at line " + std::to_string(line_data.line_nb) +
                        " : " + error;
  throw std::invalid_argument(message);
}

void ConfigParser::process_line(std::string &line) {
    determine_state(line);
    tokenize(line);
    if (state.string_state == IN_QUOTES)
        this->throw_error("unterminated string");
    for (auto i : this->line_data.token_list)
        std::cout << i.type << " : >" << i.value << "<" <<std::endl;
    this->line_data.token_list.clear();
}

e_token ConfigParser::determine_token(std::string &line, size_t &i) {
    if (i >= (line.length()))
        return END_OF_LINE;
    if (line[i] == '[' && line[i+1] == '[')
        return  i+=2, DOUBLE_BRACKET_OPEN;
    if (line[i] == ']' && line[i+1] == ']')
        return i+=2,DOUBLE_BRACKET_CLOSE;
    if (line[i] == '=')
        return i++,EQUALS;
    if (line[i] == '[')
        return i++,BRACKET_OPEN;
    if (line[i] == ']')
        return i++,BRACKET_CLOSE;
    if (line[i] == '.')
        return i++, DOT;
    if (line[i] == '{')
        return i++, BRACE_OPEN;
    if (line[i] == '}')
        return i++, BRACE_CLOSE;
    if (line[i] == ',')
        return i++, COMMA;
    if (line[i] == '"')
        return i++,QUOTED_STRING;
    if (line[i] == '#')
        return i++, COMMENT;
    return BARE_KEY;
}


void ConfigParser::process_quoted_string(Token &token)
{
   this->state.string_state = IN_QUOTES;
   while (size_t &i = this->line_data.line_progress) {
       if (i >= this->line_data.line.length() || this->line_data.line[i] == '"') {
           if (this->line_data.line[i] == '"')
               this->state.string_state = OUT_QUOTES,i++;
           break;
       }
       token.value += this->line_data.line[i++];
   }
}

bool ConfigParser::is_token(char c)
{
   return c == '[' || c == ']' || c == '=' || c == '{' || c == '}' || c == ',' || c == '.' || c == '#' || c == ' ';
}
void ConfigParser::process_bare(Token &token)
{
    size_t &i =  this->line_data.line_progress;
    while (true) {
        if (i >= this->line_data.line.length() || is_token(this->line_data.line[i ]))
                break;
        token.value += this->line_data.line[i++];
    }
}
Token ConfigParser::get_next_token(std::string &line) {
    while (std::isspace(this->line_data.line[this->line_data.line_progress]))
        this->line_data.line_progress++;

    Token token(determine_token(line, this->line_data.line_progress));
    if (token.type == BARE_KEY)
        process_bare(token);
    if (token.type == QUOTED_STRING)
    {
        std::cout << line_data.line_nb << " line number : " << line_data.line_progress << std::endl;
        process_quoted_string(token);
    }
    return token;
}
void ConfigParser::tokenize(std::string &line) {
    Token token = get_next_token(line);

    while (token.type != END_OF_LINE)
     {
         this->line_data.token_list.push_back(token);
         token = get_next_token(line);
     }
}

void ConfigParser::parse() {
  std::string line;
  this->line_data.line_nb = 0;
  while (std::getline(file, line)) {
    this->line_data.line = line;
    this->line_data.line_nb++;
    this->line_data.line_progress = 0;
    std::string trimmed = trim(line);
    if (trimmed.empty())
        continue;
    process_line(trimmed);
  }
}

int main() {
  ConfigParser TOMLParser("config.toml");

  TOMLParser.parse();
}
