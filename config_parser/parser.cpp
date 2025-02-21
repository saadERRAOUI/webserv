#include "parser.hpp"
#include <cstdio>
#include <memory>

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
    if (line.substr(0, 2) == "[[" &&
        line.substr(line.length() - 2, line.length() - 1) == "]]")
      state.substate = PARSING_HEADER;
    else
      state.substate = PARSING_KEY;
  }
}

void ConfigParser::parse_quotes(std::string &line) {
  while (this->state.string_state == IN_QUOTES)
    for (size_t i = 0; i < line.length(); i++) {
    }
}
void ConfigParser::process_line(std::string &line) {
  for (size_t i = 0; i < line.length(); i++) {
    if (isComment(line[i]))
      break;
  }
}

e_token ConfigParser::determine_token(std::string &line, size_t &i) {
    if (line[i] == '[' && line[i+1] == '[')
        return  i++, DOUBLE_BRACKET_OPEN;
    if (line[i] == ']' && line[i+1] == ']')
        return i++,DOUBLE_BRACKET_CLOSE;
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
    return BARE_KEY;
}


void ConfigParser::process_quoted_string(Token &token)
{
   this->state.string_state = IN_QUOTES;
   while (size_t &i = this->line_data.line_progress) {
       if (i >= this->line_data.line.length() || this->line_data.line[i] == '"') {
           if (this->line_data.line[i] == '"')
               this->state.string_state = OUT_QUOTES;
           break;
       }
       token.value += this->line_data.line[i++];
   }
}

void ConfigParser::key(Token &token)
{
    
}
Token ConfigParser::get_next_token(std::string &line) {
    Token token(determine_token(line, this->line_data.line_progress));
    if (token.type == BARE_KEY)
        process_key(token);
    if (token.type == QUOTED_STRING)
        process_quoted_string(token);
    return token;
}
void ConfigParser::tokenize(std::string &line) {
  for (size_t &i = this->line_data.line_progress; i < line.length(); i++) {
      
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
    process_line(trimmed);
  }
}

int main() {
  ConfigParser TOMLParser("config.toml");

  TOMLParser.parse();
}
