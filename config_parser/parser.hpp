#ifndef PARSER_HPP
#define PARSER_HPP
#include <cstddef>
#include <deque>
#include <fstream>
#include <map>
#include <string>
typedef enum { INITIAL, IN_SECTION, IN_SUBSECTION, ERROR } t_gstate;
typedef enum { IN_QUOTES, OUT_QUOTES } e_string_substate;
typedef enum { IN_STRING, IN_TABLE, IN_ARRAY } context;
typedef enum {
  PARSING_INITIAL,
  PARSING_HEADER,
  PARSING_KEY,
  PARSING_VALUE
} e_substate;
typedef enum {
  QUOTED_STRING = 1,
  BARE_KEY = 2,
  DOUBLE_BRACKET_OPEN = 4,
  DOUBLE_BRACKET_CLOSE = 8,
  EQUALS = 16,
  BRACE_OPEN = 32,
  BRACE_CLOSE = 64,
  BRACKET_OPEN = 128,
  BRACKET_CLOSE = 256,
  COMMA = 512,
  DOT = 1024,
    COMMENT = 2048,
    END_OF_LINE = 4096,
    BEGIN_OF_LINE = 8192
} e_token;

// Add a new state to track if we're expecting a value or separator in table context
typedef enum { 
    TABLE_KEY,     // Expecting a key
    TABLE_VALUE,   // Expecting a value
    TABLE_SEPARATOR // Expecting comma or closing brace
} t_table_state;

typedef struct Token {
  e_token type;
  std::string value;
  Token(e_token type) : type(type) {}
  Token(e_token type, std::string value) : type(type), value(value) {}
} Token;

typedef struct line_data {
  std::string line;
  size_t line_nb;
  size_t line_progress;
  std::deque<Token> token_list;
} t_line_data;

typedef std::deque<std::string> ArrayType;
typedef std::map<std::string, std::string> TableType;
typedef struct state_machine {
  t_gstate g_state;
  e_string_substate string_state;
  context value_substate;
  e_substate substate;
} state_machine;
struct TOMLValue {
  enum e_type { SINGLE, ARRAY, TABLE, BOOL };
  e_type type;
  std::string *single;
  ArrayType *array;
  TableType *table;
  bool true_false;

public:
  TOMLValue(enum e_type type)
      : type(type), single(NULL), array(NULL), table(NULL) {
    switch (type) {
    case SINGLE:
      single = new std::string();
      break;
    case ARRAY:
      array = new ArrayType();
      break;
    case TABLE:
      table = new TableType();
      break;
    case BOOL:
      break;
    }
  }
  
  // Add copy constructor
  TOMLValue(const TOMLValue& other)
      : type(other.type), single(NULL), array(NULL), table(NULL), true_false(other.true_false) {
    switch (type) {
    case SINGLE:
      if (other.single)
        single = new std::string(*other.single);
      break;
    case ARRAY:
      if (other.array)
        array = new ArrayType(*other.array);
      break;
    case TABLE:
      if (other.table)
        table = new TableType(*other.table);
      break;
    case BOOL:
      break;
    }
  }
  
  // Add assignment operator
  TOMLValue& operator=(const TOMLValue& other) {
    if (this != &other) {
      // Clean up existing resources
      delete single;
      delete array;
      delete table;
      
      // Copy from other
      type = other.type;
      single = NULL;
      array = NULL;
      table = NULL;
      true_false = other.true_false;
      
      switch (type) {
      case SINGLE:
        if (other.single)
          single = new std::string(*other.single);
        break;
      case ARRAY:
        if (other.array)
          array = new ArrayType(*other.array);
        break;
      case TABLE:
        if (other.table)
          table = new TableType(*other.table);
        break;
      case BOOL:
        break;
      }
    }
    return *this;
  }
  
  ~TOMLValue() {
    delete single;
    delete array;
    delete table;
  }
};

typedef std::pair<std::string, TOMLValue> key_pair;

typedef struct Section {
  std::string name;
  std::deque<key_pair> key_val;
  std::map<std::string, std::deque<Section> > /*section - values inside */
      raw_data;
  Section(std::string name) : name(name) {};
} Section;

class ConfigParser {
private:
  t_line_data line_data; /*the line to parse */
  std::string file_path; /*the file path*/
  std::ifstream file;
  Section *current_section; /*current section scope */
  state_machine state;
  std::map<std::pair<e_substate, e_token> , int> valid_tokens_map;
  std::map<std::pair<context, e_token> , int> valid_context_map;


  void determine_state();
  e_token determine_token(std::string &line, size_t &i);
  bool isComment(char c);
  void process_line(std::string &line);
  void tokenize(std::string &line);
  void process_bare(Token &token);
  void process_quoted_string(Token &token);
  Token get_next_token(std::string &line);
  bool is_token(char c);
  void validate(std::deque<Token> &token_list);
  void throw_error(std::string errr);
  void process_header();
  void process_keypair();

public:
  ConfigParser(std::string path);
  void parse();
  void debug_print_section(Section* section, int indent = 0);
  Section globalSection;  // Make this public so main can access it
};

#endif
