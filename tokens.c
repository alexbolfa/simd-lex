#include "tokens.h"

#include <stdio.h>
#include <stdlib.h>

TokenType look_up(const uint8_t id) {
    // TODO: look_up: ADD 2 byte punctuators
    switch (id) {
        // One byte punctuators
        case 80: return TOK_L_PAREN;
        case 82: return TOK_R_PAREN;
        case 182: return TOK_L_SQUARE;
        case 186: return TOK_R_SQUARE;
        case 246: return TOK_L_BRACE;
        case 250: return TOK_R_BRACE;
        case 88: return TOK_COMMA;
        case 118: return TOK_SEMI;
        case 86: return TOK_PLUS;
        case 90: return TOK_MINUS;
        case 252: return TOK_TILDE;
        case 74: return TOK_PERCENT;
        case 120: return TOK_LESS;
        case 124: return TOK_GREATER;
        case 126: return TOK_QUESTION;
        case 66: return TOK_EXCLAIM;
        case 84: return TOK_STAR;
        case 188: return TOK_CARET;
        case 76: return TOK_AMP;
        case 122: return TOK_EQUAL;
        case 92: return TOK_PERIDO;
        case 248: return TOK_PIPE;
        case 94: return TOK_SLASH;
        case 116: return TOK_COLON;

        default:
            fprintf(stderr, "Invalid token type.\n");
        return -1;
    }
}

Token* create_token(TokenType type, char* loc, char* file_path) {
    Token* new_token = (Token*) malloc(sizeof(Token));
    if (new_token == NULL) {
        return NULL;    // Memory allocation failure
    }

    new_token->type = type;
    new_token->loc = loc;
    new_token->file_path = file_path;

    return new_token;
}

void free_token(Token* token) {
    free(token->loc);
    free(token);
}

char* token_to_string(const Token *token) {
    // TODO: token_to_string: ADD 2 byte punctuators
    switch (token->type) {
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

void print_tokens(const TokenList *tok_list) {
    TokenNode *current = tok_list->head;

    while (current) {
        Token *token = current->tok;

        printf("%s\n", token_to_string(token));

        current = current->next;
    }
}

TokenList* create_empty_token_list() {
    TokenList* new_list = (TokenList*) malloc(sizeof(TokenList));
    if (new_list == NULL) {
        return NULL;    // Memory allocation failure
    }

    new_list->head = NULL;
    new_list->tail = NULL;

    return new_list;
}

void append_token(TokenList *tok_list, Token *token) {
    // Create new token node
    TokenNode *node = (TokenNode*) malloc(sizeof(TokenNode));
    if (!node) {
        fprintf(stderr, "Memory allocation failed.\n");
        exit(EXIT_FAILURE);
    }

    node->tok = token;
    node->next = NULL;

    // Append the new token node
    if (!tok_list->head) {
        tok_list->head = tok_list->tail = node; // Initialise empty list
    } else {
        tok_list->tail->next = node;
        tok_list->tail = node;
    }
}

void free_token_list(TokenList *tok_list) {
    TokenNode *current = tok_list->head;

    while (current) {
        TokenNode *next = current->next;

        free_token(current->tok);   // Free token
        free(current);              // Free node

        current = next;
    }

    // Reset list
    tok_list->head = tok_list->tail = NULL;
}
