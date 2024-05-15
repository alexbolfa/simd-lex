#ifndef TOKENS_H
#define TOKENS_H

#include <stdint.h>

// TODO: TOKEN_TYPE: ADD 2 byte punctuators
typedef enum : uint8_t {
    // One byte punctuators
    TOK_L_PAREN = 80,   // (
    TOK_R_PAREN = 82,   // )
    TOK_L_SQUARE = 182, // [
    TOK_R_SQUARE = 186, // ]
    TOK_L_BRACE = 246,  // {
    TOK_R_BRACE = 250,  // }
    TOK_COMMA = 88,     // ,
    TOK_SEMI = 118,     // ;
    TOK_PLUS = 86,      // +
    TOK_MINUS = 90,     // -
    TOK_TILDE = 252,    // ~
    TOK_PERCENT = 74,   // %
    TOK_LESS = 120,     // <
    TOK_GREATER = 124,  // >
    TOK_QUESTION = 126, // ?
    TOK_EXCLAIM = 66,   // !
    TOK_STAR = 84,      // *
    TOK_CARET = 188,    // ^
    TOK_AMP = 76,       // &
    TOK_EQUAL = 122,    // =
    TOK_PERIDO = 92,    // .
    TOK_PIPE = 248,     // |
    TOK_SLASH = 94,     // /
    TOK_COLON = 116,    // :
    // TOK_HASH,       // # (Ignored: handled by preprocessing)

    TOK_EOF,         // End-of-file
} TokenType;
// TODO: two arrays for loc
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
