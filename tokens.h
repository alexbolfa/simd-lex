#ifndef TOKENS_H
#define TOKENS_H

#define VECTOR_SIZE 32

#include <immintrin.h>
#include <stdint.h>

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

    // Two byte punctuators
    TOK_AMP_AMP = 74,           // &&
    TOK_GREATER_EQUAL = 121,    // >=
    TOK_LESS_EQUAL = 119,       // <=
    TOK_EQUAL_EQUAL = 120,      // ==
    TOK_EXCLAIM_EQUAL = 92,     // !=
    TOK_PIPE_PIPE = 246,        // ||
    TOK_PLUS_EQUAL = 102,       // +=
    TOK_MINUS_EQUAL = 104,      // -=
    TOK_STAR_EQUAL = 101,       // *=
    TOK_SLASH_EQUAL = 106,      // /=
    TOK_CARET_EQUAL = 153,      // ^=
    TOK_PIPE_EQUAL = 183,       // |=
    TOK_PERCENT_EQUAL = 96,     // %=
    TOK_AMP_EQUAL = 97,         // &=
    TOK_PLUS_PLUS = 84,         // ++
    TOK_MINUS_MINUS = 88,       // --
    TOK_GREATER_GREATER = 122,  // >>
    TOK_LESS_LESS = 118,        // <<
    TOK_ARROW = 105,            // ->
    // TOK_HASH_HASH,       // ## (Ignored: handled by preprocessing)

    // Three byte punctuators
    TOK_ELLIPSIS = 138,                 // ...
    TOK_LESS_LESS_EQUAL = 181,          // <<=
    TOK_GREATER_GREATER_EQUAL = 185,    // >>=

    // Keywords
    TOK_AUTO = 3,               // auto
    TOK_BREAK = 4,              // break
    TOK_CASE = 5,               // case
    TOK_CHAR = 6,               // char
    TOK_CONST = 7,              // const
    TOK_CONTINUE = 8,           // continue
    TOK_DEFAULT = 9,            // default
    TOK_DO = 10,                // do
    TOK_DOUBLE = 11,            // double
    TOK_ELSE = 12,              // else
    TOK_ENUM = 13,              // enum
    TOK_EXTERN = 14,            // extern
    TOK_FLOAT = 15,             // float
    TOK_FOR = 16,               // for
    TOK_GOTO = 17,              // goto
    TOK_IF = 18,                // if
    TOK_INLINE = 19,            // inline
    TOK_INT = 20,               // int
    TOK_LONG = 21,              // long
    TOK_REGISTER = 22,          // register
    TOK_RESTRICT = 23,          // restrict
    TOK_RETURN = 24,            // return
    TOK_SHORT = 25,             // short
    TOK_SIGNED = 26,            // signed
    TOK_SIZEOF = 27,            // sizeof
    TOK_STATIC = 28,            // static
    TOK_STRUCT = 29,            // struct
    TOK_SWITCH = 30,            // switch
    TOK_TYPEDEF = 31,           // typedef
    TOK_UNION = 32,             // union
    TOK_UNSIGNED = 34,          // unsigned
    TOK_VOID = 35,              // void
    TOK_VOLATILE = 36,          // volatile
    TOK_WHILE = 39,             // while
    TOK__ALIGNAS = 48,          // _Alignas
    TOK__ALIGNOF = 49,          // _Alignof
    TOK__ATOMIC = 50,           // _Atomic
    TOK__BOOL = 51,             // _Bool
    TOK__COMPLEX = 52,          // _Complex
    TOK__GENERIC = 53,          // _Generic
    TOK__IMAGINARY = 54,        // _Imaginary
    TOK__NORETURN = 55,         // _Noreturn
    TOK__STATIC_ASSERT = 56,    // _Static_assert
    TOK__THREAD_LOCAL = 57,     // _Thread_local

    TOK_IDENT = 1,      // Identifiers
    TOK_NUM = 2,        // Numeric constants

    TOK_EOF = 0,        // End-of-file
} TokenType;

typedef struct Token Token;
struct Token {
    TokenType type;     // Token type (identifier, number ...etc)
    uint32_t loc;       // Token location in file
};

typedef struct TokenArray TokenArray;
struct TokenArray {
    uint64_t size;
    uint64_t capacity;
    uint32_t* token_locs;
    char* src;
    TokenType* token_types;
};

Token create_token(TokenType type, uint32_t loc);
void token_to_string(char *dst, const Token token, const char *src);
void print_tokens(const TokenArray tok_array);

TokenArray create_empty_token_array(uint64_t capacity);
void append_token(TokenArray *tok_array, Token token);

/**
 * Append list of tokens stored in __m256i vectors.
 *
 * @param tok_array The array to which we append.
 * @param types A left-packed __m256i vector with the tokens types.
 * @param locs A left-packed __m256i vector with the tokens location.
 * @param size Number of tokens to append.
 * @param start_idx Starting index of current vector of token.
 */
void append_tokens(TokenArray *tok_array, __m256i types, __m256i locs, int size, uint32_t start_idx);

void free_token_array(TokenArray tok_list);

#endif //TOKENS_H
