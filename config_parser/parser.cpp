#include "parser.hpp"
#include <cstdio>
#include <memory>
#include <iostream>

ConfigParser::ConfigParser(std::string path)
    : file_path(path), file(path.c_str()), globalSection("global"),
      current_section(&globalSection)
{
    state.g_state = INITIAL;
    state.string_state = OUT_QUOTES;
    state.value_substate = IN_STRING;
    state.substate = PARSING_INITIAL;

    valid_tokens_map[std::pair<e_substate, e_token>(PARSING_INITIAL, BEGIN_OF_LINE)] = BARE_KEY | QUOTED_STRING | DOUBLE_BRACKET_OPEN | COMMENT;
    valid_tokens_map[std::pair<e_substate, e_token>(PARSING_HEADER, BEGIN_OF_LINE)] = BARE_KEY | QUOTED_STRING | DOUBLE_BRACKET_OPEN | COMMENT;
    valid_tokens_map[std::pair<e_substate, e_token>(PARSING_KEY, BEGIN_OF_LINE)] = BARE_KEY | QUOTED_STRING | DOUBLE_BRACKET_OPEN | COMMENT;

    valid_tokens_map[std::pair<e_substate, e_token>(PARSING_HEADER, DOUBLE_BRACKET_OPEN)] = BARE_KEY | QUOTED_STRING;
    valid_tokens_map[std::pair<e_substate, e_token>(PARSING_HEADER, DOUBLE_BRACKET_CLOSE)] = END_OF_LINE | COMMENT;
    valid_tokens_map[std::pair<e_substate, e_token>(PARSING_HEADER, BARE_KEY)] = DOT | DOUBLE_BRACKET_CLOSE;
    valid_tokens_map[std::pair<e_substate, e_token>(PARSING_HEADER, QUOTED_STRING)] = DOT | DOUBLE_BRACKET_CLOSE;
    valid_tokens_map[std::pair<e_substate, e_token>(PARSING_HEADER, DOT)] = BARE_KEY | QUOTED_STRING;

    valid_tokens_map[std::pair<e_substate, e_token>(PARSING_KEY, BARE_KEY)] = EQUALS;
    valid_tokens_map[std::pair<e_substate, e_token>(PARSING_KEY, QUOTED_STRING)] = EQUALS;
    valid_tokens_map[std::pair<e_substate, e_token>(PARSING_KEY, EQUALS)] = BARE_KEY | QUOTED_STRING | BRACE_OPEN | BRACKET_OPEN;

    
    valid_context_map[std::pair<context, e_token>(IN_STRING, BARE_KEY)] = END_OF_LINE | COMMENT;
    valid_context_map[std::pair<context, e_token>(IN_STRING, QUOTED_STRING)] = END_OF_LINE | COMMENT;
    
    
    valid_context_map[std::pair<context, e_token>(IN_ARRAY, BARE_KEY)] = COMMA | BRACKET_CLOSE;
    valid_context_map[std::pair<context, e_token>(IN_ARRAY, QUOTED_STRING)] = COMMA | BRACKET_CLOSE;
    valid_context_map[std::pair<context, e_token>(IN_ARRAY, COMMA)] = BARE_KEY | QUOTED_STRING;
    valid_context_map[std::pair<context, e_token>(IN_ARRAY, BRACKET_CLOSE)] = END_OF_LINE | COMMENT;

    valid_context_map[std::pair<context, e_token>(IN_TABLE, BRACE_OPEN)] = BARE_KEY;
    valid_context_map[std::pair<context, e_token>(IN_TABLE, BARE_KEY)] = EQUALS;
    valid_context_map[std::pair<context, e_token>(IN_TABLE, EQUALS)] = QUOTED_STRING;
    valid_context_map[std::pair<context, e_token>(IN_TABLE, QUOTED_STRING)] = COMMA | BRACE_CLOSE;
      valid_context_map[std::pair<context, e_token>(IN_TABLE, EQUALS)] =  QUOTED_STRING;
    valid_context_map[std::pair<context, e_token>(IN_TABLE, COMMA)] = BARE_KEY;
    valid_context_map[std::pair<context, e_token>(IN_TABLE, BRACE_CLOSE)] = END_OF_LINE | COMMENT ;
    
    
    if (!file.is_open())
        throw std::invalid_argument("could not open the file!");
}

std::string trim(std::string &line)
{
    size_t start = line.find_first_not_of(" \t");
    if (start == std::string::npos)
        return "";
    return line.substr(start, line.find_last_not_of(" \t") + 1);
}

bool ConfigParser::isComment(char c)
{
    if ((c == '#' && state.string_state != OUT_QUOTES))
        return true;
    return false;
}

bool ConfigParser::is_token(char c)
{
    return isspace(c) || c == '[' || c == ']' || c == '=' || c == '{' || c == '}' || c == ',' || c == '.' || c == '#' || c == '"';
}

void ConfigParser::determine_state()
{
    if (this->line_data.token_list.empty())
        return;
    Token &token = this->line_data.token_list[1];
    if (token.type == DOUBLE_BRACKET_OPEN)
        state.substate = PARSING_HEADER;
    else
        state.substate = PARSING_KEY;
}

void ConfigParser::validate(std::deque<Token> &token_list)
{
    if (state.substate == PARSING_KEY || state.substate == PARSING_HEADER)
    {
        for (std::deque<Token>::iterator i = token_list.begin(); i != token_list.end(); i++)
        {
            if (i->type == END_OF_LINE || i->type == COMMENT)
                break;
            if (state.substate != PARSING_VALUE && (valid_tokens_map[std::pair<e_substate, e_token>(state.substate, i->type)] & (i + 1)->type) == 0)
                std::cout << "Error at line " << line_data.line_nb << " : unexpected token " << i->type << " before " << (i + 1)->type << std::endl;
            if (state.substate == PARSING_VALUE && (valid_context_map[std::pair<context, e_token>(state.value_substate, i->type)] & (i + 1)->type) == 0)
                std::cout << "value Error at line " << line_data.line_nb << " : unexpected token " << i->type << " before " << (i + 1)->type << std::endl;
            if (i->type == EQUALS && state.substate != PARSING_VALUE)
                {
                    state.substate = PARSING_VALUE;
                    if ((i + 1)->type == QUOTED_STRING)
                        state.value_substate = IN_STRING;
                    if ((i + 1)->type == BRACKET_OPEN)
                        state.value_substate = IN_ARRAY;
                    if ((i + 1)->type == BRACE_OPEN)
                        state.value_substate = IN_TABLE;
                    i++;
                }
            
            
        }
    }
}

void ConfigParser::throw_error(std::string error)
{
    std::string message = "Error at line " + std::to_string(line_data.line_nb) +
                          " : " + error;
    throw std::invalid_argument(message);
}

void ConfigParser::process_line(std::string &line)
{
    tokenize(line);
    determine_state();
    validate(this->line_data.token_list);
    if (state.string_state == IN_QUOTES)
        this->throw_error("unterminated string");
    this->line_data.token_list.clear();
}

e_token ConfigParser::determine_token(std::string &line, size_t &i)
{
    if (i >= (line.length()))
        return END_OF_LINE;
    if (line[i] == '[' && line[i + 1] == '[')
        return i += 2, DOUBLE_BRACKET_OPEN;
    if (line[i] == ']' && line[i + 1] == ']')
        return i += 2, DOUBLE_BRACKET_CLOSE;
    if (line[i] == '=')
        return i++, EQUALS;
    if (line[i] == '[')
        return i++, BRACKET_OPEN;
    if (line[i] == ']')
        return i++, BRACKET_CLOSE;
    if (line[i] == '.')
        return i++, DOT;
    if (line[i] == '{')
        return i++, BRACE_OPEN;
    if (line[i] == '}')
        return i++, BRACE_CLOSE;
    if (line[i] == ',')
        return i++, COMMA;
    if (line[i] == '"')
        return i++, QUOTED_STRING;
    if (line[i] == '#')
        return i++, COMMENT;
    return BARE_KEY;
}

void ConfigParser::process_quoted_string(Token &token)
{
    this->state.string_state = IN_QUOTES;
    while (size_t &i = this->line_data.line_progress)
    {
        if (i >= this->line_data.line.length() || this->line_data.line[i] == '"')
        {
            if (this->line_data.line[i] == '"')
                this->state.string_state = OUT_QUOTES, i++;
            break;
        }
        token.value += this->line_data.line[i++];
    }
}

void ConfigParser::process_bare(Token &token)
{
    size_t &i = this->line_data.line_progress;
    while (true)
    {
        if (i >= this->line_data.line.length() || is_token(this->line_data.line[i]))
            break;
        token.value += this->line_data.line[i++];
    }
}

Token ConfigParser::get_next_token(std::string &line)
{
    while (std::isspace(this->line_data.line[this->line_data.line_progress]))
        this->line_data.line_progress++;

    Token token(determine_token(line, this->line_data.line_progress));
    if (token.type == BARE_KEY)
        process_bare(token);
    if (token.type == QUOTED_STRING)
        process_quoted_string(token);
    return token;
}

void ConfigParser::tokenize(std::string &line)
{
    this->line_data.token_list.push_back(Token(BEGIN_OF_LINE));
    Token token = get_next_token(line);
    while (token.type != END_OF_LINE && token.type != COMMENT)
    {
        this->line_data.token_list.push_back(token);
        token = get_next_token(line);
    }
    this->line_data.token_list.push_back(token);
}

void ConfigParser::parse()
{
    std::string line;
    this->line_data.line_nb = 0;
    while (std::getline(file, line))
    {
        this->line_data.line = line;
        this->line_data.line_nb++;
        this->line_data.line_progress = 0;
        std::string trimmed = trim(line);
        if (trimmed.empty())
            continue;
        process_line(trimmed);
    }
}

int main()
{
    ConfigParser TOMLParser("config.toml");

    TOMLParser.parse();
}
