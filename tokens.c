#include "tokens.h"

#include <stdio.h>
#include <stdlib.h>

Token create_token(TokenType type, uint32_t loc) {
    return (Token) { type, loc };
}

char* token_to_string(const Token token) {
    // TODO: token_to_string: ADD 2 byte punctuators
    switch (token.type) {
        // Punctuators
        case TOK_L_PAREN:
            return "(";
        case TOK_R_PAREN:
            return ")";
        case TOK_L_SQUARE:
            return "[";
        case TOK_R_SQUARE:
            return "]";
        case TOK_L_BRACE:
            return "{";
        case TOK_R_BRACE:
            return "}";
        case TOK_COMMA:
            return ",";
        case TOK_SEMI:
            return ";";
        case TOK_PLUS:
            return "+";
        case TOK_MINUS:
            return "-";
        case TOK_TILDE:
            return "~";
        case TOK_PERCENT:
            return "%";
        case TOK_LESS:
            return "<";
        case TOK_GREATER:
            return ">";
        case TOK_QUESTION:
            return "?";
        case TOK_EXCLAIM:
            return "!";
        case TOK_STAR:
            return "*";
        case TOK_CARET:
            return "^";
        case TOK_AMP:
            return "&";
        case TOK_EQUAL:
            return "=";
        case TOK_PERIDO:
            return ".";
        case TOK_PIPE:
            return "|";
        case TOK_SLASH:
            return "/";
        case TOK_COLON:
            return ":";

        case TOK_EOF:
            return "";

        default:
            fprintf(stderr, "Invalid token type.\n");
        return "";
    }
}

void print_tokens(const TokenArray tok_array) {
    for (int i = 0; i < tok_array.size; ++i) {
        Token token = create_token(tok_array.token_types[i], tok_array.token_locs[i]);

        printf("%s\n", token_to_string(token));
    }
}

TokenArray create_empty_token_array(uint64_t capacity) {
    TokenType* tokens_types = (TokenType*) malloc(capacity * sizeof(TokenType));
    uint32_t* token_locs = (uint32_t*) malloc(capacity * sizeof(uint32_t));

    if (tokens_types == NULL || token_locs == NULL) {
        fprintf(stderr, "Memory allocation failure.\n");
    }

    TokenArray tok_array;
    tok_array.token_types = tokens_types;
    tok_array.token_locs = token_locs;
    tok_array.capacity = capacity;
    tok_array.size = 0;

    return tok_array;
}

void append_token(TokenArray *tok_list, Token token) {
    tok_list->token_types[tok_list->size] = token.type;
    tok_list->token_locs[tok_list->size] = token.loc;
    ++tok_list->size;
}

void free_token_array(TokenArray tok_list) {
    free(tok_list.token_types);
    free(tok_list.token_locs);
}
