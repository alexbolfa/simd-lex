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
            return "l_paren  (";
        case TOK_R_PAREN:
            return "r_paren  )";
        case TOK_L_SQUARE:
            return "l_square  [";
        case TOK_R_SQUARE:
            return "r_square  ]";
        case TOK_L_BRACE:
            return "l_brace  {";
        case TOK_R_BRACE:
            return "r_brace  }";
        case TOK_COMMA:
            return "comma  ,";
        case TOK_SEMI:
            return "semi  ;";
        case TOK_PLUS:
            return "plus  +";
        case TOK_MINUS:
            return "minus  -";
        case TOK_TILDE:
            return "tilde  ~";
        case TOK_PERCENT:
            return "percent  %";
        case TOK_LESS:
            return "less  <";
        case TOK_GREATER:
            return "greater  >";
        case TOK_QUESTION:
            return "question  ?";
        case TOK_EXCLAIM:
            return "exclaim  !";
        case TOK_STAR:
            return "star  *";
        case TOK_CARET:
            return "caret  ^";
        case TOK_AMP:
            return "amp  &";
        case TOK_EQUAL:
            return "equal  =";
        case TOK_PERIOD:
            return "period  .";
        case TOK_PIPE:
            return "pipe  |";
        case TOK_SLASH:
            return "slash  /";
        case TOK_COLON:
            return "colon  :";

        case TOK_EOF:
            return "eof  ";

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
    TokenType* tokens_types = (TokenType*) malloc(capacity * sizeof(TokenType) + VECTOR_SIZE);
    uint32_t* token_locs = (uint32_t*) malloc(capacity * sizeof(uint32_t) + VECTOR_SIZE);

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

void append_token(TokenArray *tok_array, Token token) {
    tok_array->token_types[tok_array->size] = token.type;
    tok_array->token_locs[tok_array->size] = token.loc;
    ++tok_array->size;
}

void append_tokens(TokenArray *tok_array, __m256i types, __m256i locs, int size) {
    _mm256_storeu_si256(
        (__m256i *) (tok_array->token_types + tok_array->size),
        types
    );

    _mm256_storeu_si256(
        (__m256i *) (tok_array->token_locs + tok_array->size),
        locs
    );

    tok_array->size += size;
}

void free_token_array(TokenArray tok_list) {
    free(tok_list.token_types);
    free(tok_list.token_locs);
}
