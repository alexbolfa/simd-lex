#ifndef TOKENS_H
#define TOKENS_H

#include <stdint.h>

// TODO: TOKEN_TYPE: ADD 2 byte punctuators
typedef enum : uint8_t {
    // One byte punctuators
    TOK_L_PAREN = 40,   // (
    TOK_R_PAREN = 41,   // )
    TOK_L_SQUARE = 91,  // [
    TOK_R_SQUARE = 93,  // ]
    TOK_L_BRACE = 123,  // {
    TOK_R_BRACE = 125,  // }
    TOK_COMMA = 44,     // ,
    TOK_SEMI = 59,      // ;
    TOK_PLUS = 43,      // +
    TOK_MINUS = 45,     // -
    TOK_TILDE = 126,    // ~
    TOK_PERCENT = 37,   // %
    TOK_LESS = 60,      // <
    TOK_GREATER = 62,   // >
    TOK_QUESTION = 63,  // ?
    TOK_EXCLAIM = 33,   // !
    TOK_STAR = 42,      // *
    TOK_CARET = 94,     // ^
    TOK_AMP = 38,       // &
    TOK_EQUAL = 61,     // =
    TOK_PERIOD = 46,    // .
    TOK_PIPE = 124,     // |
    TOK_SLASH = 47,     // /
    TOK_COLON = 58,     // :
    // TOK_HASH,        // # (Ignored: handled by preprocessing)

    TOK_EOF = 0,        // End-of-file
} TokenType;

typedef struct Token Token;
struct Token {
    TokenType type;     // Token type (identifier, number ...etc)
    uint32_t loc;          // Token location in file
};

typedef struct TokenArray TokenArray;
struct TokenArray {
    uint64_t size;
    uint64_t capacity;
    uint32_t* token_locs;
    TokenType* token_types;
};

Token create_token(TokenType type, uint32_t loc);
char* token_to_string(const Token token);
void print_tokens(const TokenArray tok_array);

TokenArray create_empty_token_array(uint64_t capacity);
void append_token(TokenArray *tok_list, Token token);
void free_token_array(TokenArray tok_list);

#endif //TOKENS_H
