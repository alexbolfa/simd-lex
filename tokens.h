#ifndef TOKENS_H
#define TOKENS_H

#include <stdint.h>

// TODO: TOKEN_TYPE: ADD 2 byte punctuators
typedef enum : uint8_t {
    // One byte punctuators
    TOK_L_PAREN,    // (
    TOK_R_PAREN,    // )
    TOK_L_SQUARE,   // [
    TOK_R_SQUARE,   // ]
    TOK_L_BRACE,    // {
    TOK_R_BRACE,    // }
    TOK_COMMA,      // ,
    TOK_SEMI,       // ;
    TOK_PLUS,       // +
    TOK_MINUS,      // -
    TOK_TILDE,      // ~
    TOK_PERCENT,    // %
    TOK_LESS,       // <
    TOK_GREATER,    // >
    TOK_QUESTION,   // ?
    TOK_EXCLAIM,    // !
    TOK_STAR,       // *
    TOK_CARET,      // ^
    TOK_AMP,        // &
    TOK_EQUAL,      // =
    TOK_PERIDO,     // .
    TOK_PIPE,       // |
    TOK_SLASH,      // /
    TOK_COLON,      // :
    // TOK_HASH,       // # (Ignored: handled by preprocessing)

    TOK_EOF,         // End-of-file
} TokenType;

typedef struct Token Token;
struct Token {
    TokenType type;     // Token type (identifier, number ...etc)
    char *loc;          // Token location in file
    char *file_path;    // Path to file
};

typedef struct TokenNode TokenNode;
struct TokenNode {
    Token* tok;
    TokenNode* next;
};

typedef struct TokenList TokenList;
struct TokenList {
    TokenNode* head;
    TokenNode* tail;
};

TokenType look_up(uint8_t id);

Token* create_token(TokenType type, char* loc, char* file_path);
void free_token(Token* token);
char* token_to_string(const Token *token);
void print_tokens(const TokenList *tok_list);

TokenList* create_empty_token_list();
void append_token(TokenList *tok_list, Token *token);
void free_token_list(TokenList *tok_list);

#endif //TOKENS_H
