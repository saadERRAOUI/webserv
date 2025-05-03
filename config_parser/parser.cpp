#include "parser.hpp"
#include <cstdio>
#include <memory>
#include <iostream>
#include <stdlib.h>
#include "utilities.hpp"
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
        return  true;
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
                this->throw_error("Unexpected token");
            if (state.substate == PARSING_VALUE && (valid_context_map[std::pair<context, e_token>(state.value_substate, i->type)] & (i + 1)->type) == 0)
                this->throw_error(std::string("Unexpected token at value") + "on token " + i->type + " "));
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
    std::string message = "Error at line " + std::string(parseString(line_data.line_nb)) +
                          " : " + error;
    throw ConfigParser::SyntaxError(message);
}


void ConfigParser::process_header()
{   
    current_section = &globalSection;  // Start at the root
    
    // Process each section part (e.g., "server", "location")
    for (size_t i = 2; i < this->line_data.token_list.size(); i++)
    {
        if (this->line_data.token_list[i].type & (QUOTED_STRING | BARE_KEY))
        {
            std::string section_name = this->line_data.token_list[i].value;
            // Check if this section already exists
            bool found = false;
            
            if (current_section->raw_data.count(section_name) > 0) {
                // If we're at the last part of the path, add a new entry
                if (i == this->line_data.token_list.size() - 2 || 
                   (i + 1 < this->line_data.token_list.size() && 
                    this->line_data.token_list[i + 1].type == DOUBLE_BRACKET_CLOSE)) {
                    // This is the last segment, create a new section
                    current_section->raw_data[section_name].push_back(Section(section_name));
                    current_section = &current_section->raw_data[section_name].back();
                    found = true;
                } else {
                    // This is not the last segment, use the existing section
                    current_section = &current_section->raw_data[section_name].front();
                    found = true;
                }
            }
            
            // If not found, create a new section
            if (!found) {
                current_section->raw_data[section_name].push_back(Section(section_name));
                current_section = &current_section->raw_data[section_name].back();
            }
        }
    }
}

// // Fix the function definition - remove the default argument
// void ConfigParser::debug_print_section(Section* section, int indent)
// {
//     // Print section name
//     std::string indentation(indent * 2, ' ');
//     std::cout << indentation << "Section: " << section->name << std::endl;
    
//     // Print key-value pairs
//     for (size_t i = 0; i < section->key_val.size(); i++) {
//         std::cout << indentation << "  Key: " << section->key_val[i].first << ", ";
        
//         TOMLValue& val = section->key_val[i].second;
//         switch (val.type) {
//             case TOMLValue::SINGLE:
//                 if (val.single.get())
//                     std::cout << "Value: " << *val.single.get() << std::endl;
//                 break;
//             case TOMLValue::ARRAY:
//                 if (val.array.get()) {
//                     std::cout << "Array: [";
//                     for (size_t j = 0; j < val.array.get()->size(); j++) {
//                         std::cout << (j > 0 ? ", " : "") << (*val.array.get())[j];
//                     }
//                     std::cout << "]" << std::endl;
//                 }
//                 break;
//             case TOMLValue::TABLE:
//                 if (val.table.get()) {
//                     std::cout << "Table: {";
//                     for (TableType::iterator it = val.table.get()->begin(); it != val.table.get()->end(); ++it) {
//                         std::cout << (it != val.table.get()->begin() ? ", " : "") << it->first << " = " << it->second;
//                     }
//                     std::cout << "}" << std::endl;
//                 }
//                 break;
//         }
//     }
    
//     // Recursively print subsections
//     for (auto& section_pair : section->raw_data) {
//         for (auto& subsection : section_pair.second) {
//             debug_print_section(&subsection, indent + 1);
//         }
//     }
// }

void ConfigParser::process_keypair()
{
    std::string key;
    key = this->line_data.token_list[1].value;
    
    // Check for equals sign
    if (this->line_data.token_list[2].type != EQUALS || this->line_data.token_list.size() < 4)
    {
        this->throw_error("expected '=' after key");
        return;
    }
    
    // Process different value types based on the token after equals
    Token& value_token = this->line_data.token_list[3];
    
    if (value_token.type & (BARE_KEY | QUOTED_STRING))
    {
        // Simple string value
        TOMLValue value(TOMLValue::SINGLE);
        *value.single= value_token.value;
        if (value_token.type != QUOTED_STRING)
            if (value_token.value == "true" || value_token.value == "false")
            {
                value.true_false = value_token.value == "true";
                value.type = TOMLValue::BOOL;
            }
        current_section->key_val.push_back(key_pair(key, value));
    }
    else if (value_token.type == BRACKET_OPEN)
    {
        // Array value
        TOMLValue value(TOMLValue::ARRAY);
        
        // Process array elements (tokens between BRACKET_OPEN and BRACKET_CLOSE)
        for (size_t i = 4; i < this->line_data.token_list.size(); i++)
        {
            if (this->line_data.token_list[i].type & (BARE_KEY | QUOTED_STRING))
                value.array->push_back(this->line_data.token_list[i].value);
            
            if (this->line_data.token_list[i].type == BRACKET_CLOSE)
                break;
        }
        
        current_section->key_val.push_back(key_pair(key, value));
    }
    else if (value_token.type == BRACE_OPEN)
    {
        // Table value
        TOMLValue value(TOMLValue::TABLE);
        std::string table_key;
        
        // Process table key-value pairs (tokens between BRACE_OPEN and BRACE_CLOSE)
        for (size_t i = 4; i < this->line_data.token_list.size(); i++)
        {
            if (this->line_data.token_list[i].type & (BARE_KEY | QUOTED_STRING) && table_key.empty())
            {
                table_key = this->line_data.token_list[i].value;
            }
            else if (this->line_data.token_list[i].type == EQUALS)
            {
                // Skip the equals token
                continue;
            }
            else if (this->line_data.token_list[i].type & (BARE_KEY | QUOTED_STRING) && !table_key.empty())
            {
                // Add the key-value pair to the table
                (*value.table)[table_key] = this->line_data.token_list[i].value;
                table_key.clear();  // Reset for next key-value pair
            }
            
            if (this->line_data.token_list[i].type == BRACE_CLOSE)
                break;
        }
        
        current_section->key_val.push_back(key_pair(key, value));
    }
    else
    {
        this->throw_error("invalid value type");
    }
}

void ConfigParser::process_line(std::string &line)
{
    tokenize(line);
    determine_state();
    validate(this->line_data.token_list);
    if (this->line_data.token_list[1].type == COMMENT)
            return this->line_data.token_list.clear();;
    if (this->line_data.token_list[1].type == DOUBLE_BRACKET_OPEN)
        process_header();
    else
        process_keypair();
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
    size_t &i = this->line_data.line_progress;
    while (i < this->line_data.line.length())
    {
        if (this->line_data.line[i] == '"')
        {
            this->state.string_state = OUT_QUOTES;
            i++;  // Move past the closing quote
            break;
        }
        token.value += this->line_data.line[i++];
    }
    // If we got here without finding a closing quote, string is unterminated
    // (this will be caught later in process_line)
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


