#include "tokens.h"

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

Token create_token(TokenType type, uint32_t loc) {
    return (Token) { type, loc };
}

void token_to_string(char *dst, const Token token, const char *src) {
    switch (token.type) {
        // One byte punctuators
        case TOK_L_PAREN:
            strcpy(dst, "l_paren  (");
            break;
        case TOK_R_PAREN:
            strcpy(dst, "r_paren  )");
            break;
        case TOK_L_SQUARE:
            strcpy(dst, "l_square  [");
            break;
        case TOK_R_SQUARE:
            strcpy(dst, "r_square  ]");
            break;
        case TOK_L_BRACE:
            strcpy(dst, "l_brace  {");
            break;
        case TOK_R_BRACE:
            strcpy(dst, "r_brace  }");
            break;
        case TOK_COMMA:
            strcpy(dst, "comma  ,");
            break;
        case TOK_SEMI:
            strcpy(dst, "semi  ;");
            break;
        case TOK_PLUS:
            strcpy(dst, "plus  +");
            break;
        case TOK_MINUS:
            strcpy(dst, "minus  -");
            break;
        case TOK_TILDE:
            strcpy(dst, "tilde  ~");
            break;
        case TOK_PERCENT:
            strcpy(dst, "percent  %");
            break;
        case TOK_LESS:
            strcpy(dst, "less  <");
            break;
        case TOK_GREATER:
            strcpy(dst, "greater  >");
            break;
        case TOK_QUESTION:
            strcpy(dst, "question  ?");
            break;
        case TOK_EXCLAIM:
            strcpy(dst, "exclaim  !");
            break;
        case TOK_STAR:
            strcpy(dst, "star  *");
            break;
        case TOK_CARET:
            strcpy(dst, "caret  ^");
            break;
        case TOK_AMP:
            strcpy(dst, "amp  &");
            break;
        case TOK_EQUAL:
            strcpy(dst, "equal  =");
            break;
        case TOK_PERIOD:
            strcpy(dst, "period  .");
            break;
        case TOK_PIPE:
            strcpy(dst, "pipe  |");
            break;
        case TOK_SLASH:
            strcpy(dst, "slash  /");
            break;
        case TOK_COLON:
            strcpy(dst, "colon  :");
            break;

        // Two byte punctuators
        case TOK_AMP_AMP:
            strcpy(dst, "ampamp  &&");
            break;
        case TOK_GREATER_EQUAL:
            strcpy(dst, "greaterequal  >=");
            break;
        case TOK_LESS_EQUAL:
            strcpy(dst, "lessequal  <=");
            break;
        case TOK_EQUAL_EQUAL:
            strcpy(dst, "equalequal  ==");
            break;
        case TOK_EXCLAIM_EQUAL:
            strcpy(dst, "exclaimequal  !=");
            break;
        case TOK_PIPE_PIPE:
            strcpy(dst, "pipepipe  ||");
            break;
        case TOK_PLUS_EQUAL:
            strcpy(dst, "plusequal  +=");
            break;
        case TOK_MINUS_EQUAL:
            strcpy(dst, "minusequal  -=");
            break;
        case TOK_STAR_EQUAL:
            strcpy(dst, "starequal  *=");
            break;
        case TOK_SLASH_EQUAL:
            strcpy(dst, "slashequal  /=");
            break;
        case TOK_CARET_EQUAL:
            strcpy(dst, "caretequal  ^=");
            break;
        case TOK_PIPE_EQUAL:
            strcpy(dst, "pipeequal  |=");
            break;
        case TOK_PERCENT_EQUAL:
            strcpy(dst, "percentequal  %=");
            break;
        case TOK_AMP_EQUAL:
            strcpy(dst, "ampequal  &=");
            break;
        case TOK_PLUS_PLUS:
            strcpy(dst, "plusplus  ++");
            break;
        case TOK_MINUS_MINUS:
            strcpy(dst, "minusminus  --");
            break;
        case TOK_GREATER_GREATER:
            strcpy(dst, "greatergreater  >>");
            break;
        case TOK_LESS_LESS:
            strcpy(dst, "lessless  <<");
            break;
        case TOK_ARROW:
            strcpy(dst, "arrow  ->");
            break;

        // Three byte punctuators
        case TOK_ELLIPSIS:
            strcpy(dst, "ellipsis  ...");
            break;
        case TOK_LESS_LESS_EQUAL:
            strcpy(dst, "lesslessequal  <<=");
            break;
        case TOK_GREATER_GREATER_EQUAL:
            strcpy(dst, "greatergreaterequal  >>=");
            break;

        // Keywords
        case TOK_AUTO:
            strcpy(dst, "auto  auto");
            break;
        case TOK_BREAK:
            strcpy(dst, "break  break");
            break;
        case TOK_CASE:
            strcpy(dst, "case  case");
            break;
        case TOK_CHAR:
            strcpy(dst, "char  char");
            break;
        case TOK_CONST:
            strcpy(dst, "const  const");
            break;
        case TOK_CONTINUE:
            strcpy(dst, "continue  continue");
            break;
        case TOK_DEFAULT:
            strcpy(dst, "default  default");
            break;
        case TOK_DO:
            strcpy(dst, "do  do");
            break;
        case TOK_DOUBLE:
            strcpy(dst, "double  double");
            break;
        case TOK_ELSE:
            strcpy(dst, "else  else");
            break;
        case TOK_ENUM:
            strcpy(dst, "enum  enum");
            break;
        case TOK_EXTERN:
            strcpy(dst, "extern  extern");
            break;
        case TOK_FLOAT:
            strcpy(dst, "float  float");
            break;
        case TOK_FOR:
            strcpy(dst, "for  for");
            break;
        case TOK_GOTO:
            strcpy(dst, "goto  goto");
            break;
        case TOK_IF:
            strcpy(dst, "if  if");
            break;
        case TOK_INLINE:
            strcpy(dst, "inline  inline");
            break;
        case TOK_INT:
            strcpy(dst, "int  int");
            break;
        case TOK_LONG:
            strcpy(dst, "long  long");
            break;
        case TOK_REGISTER:
            strcpy(dst, "register  register");
            break;
        case TOK_RESTRICT:
            strcpy(dst, "restrict  restrict");
            break;
        case TOK_RETURN:
            strcpy(dst, "return  return");
            break;
        case TOK_SHORT:
            strcpy(dst, "short  short");
            break;
        case TOK_SIGNED:
            strcpy(dst, "signed  signed");
            break;
        case TOK_SIZEOF:
            strcpy(dst, "sizeof  sizeof");
            break;
        case TOK_STATIC:
            strcpy(dst, "static  static");
            break;
        case TOK_STRUCT:
            strcpy(dst, "struct  struct");
            break;
        case TOK_SWITCH:
            strcpy(dst, "switch  switch");
            break;
        case TOK_TYPEDEF:
            strcpy(dst, "typedef  typedef");
            break;
        case TOK_UNION:
            strcpy(dst, "union  union");
            break;
        case TOK_UNSIGNED:
            strcpy(dst, "unsigned  unsigned");
            break;
        case TOK_VOID:
            strcpy(dst, "void  void");
            break;
        case TOK_VOLATILE:
            strcpy(dst, "volatile  volatile");
            break;
        case TOK_WHILE:
            strcpy(dst, "while  while");
            break;
        case TOK__ALIGNAS:
            strcpy(dst, "_Alignas  _Alignas");
            break;
        case TOK__ALIGNOF:
            strcpy(dst, "_Alignof  _Alignof");
            break;
        case TOK__ATOMIC:
            strcpy(dst, "_Atomic  _Atomic");
            break;
        case TOK__BOOL:
            strcpy(dst, "_Bool  _Bool");
            break;
        case TOK__COMPLEX:
            strcpy(dst, "_Complex  _Complex");
            break;
        case TOK__GENERIC:
            strcpy(dst, "_Generic  _Generic");
            break;
        case TOK__IMAGINARY:
            strcpy(dst, "_Imaginary  _Imaginary");
            break;
        case TOK__NORETURN:
            strcpy(dst, "_Noreturn  _Noreturn");
            break;
        case TOK__STATIC_ASSERT:
            strcpy(dst, "_Static_assert  _Static_assert");
            break;
        case TOK__THREAD_LOCAL:
            strcpy(dst, "_Thread_local  _Thread_local");
            break;

        case TOK_CHAR_LIT:
            strcpy(dst, "char_constant  ");
            strcat(dst, (src + token.loc));
            break;
        case TOK_IDENT:
            strcpy(dst, "identifier  ");
            strcat(dst, (src + token.loc));
            break;
        case TOK_NUM:
            strcpy(dst, "numeric_constant  ");
            strcat(dst, (src + token.loc));
            break;

        case TOK_EOF:
            strcpy(dst, "eof  ");
            break;

        default:
            fprintf(stderr, "Invalid token type.\n");
            strcpy(dst, "");
    }
}

void print_tokens(const TokenArray tok_array) {
    for (int i = 0; i < tok_array.size; ++i) {
        Token token = create_token(tok_array.token_types[i], tok_array.token_locs[i]);

        char *str = malloc(100);
        token_to_string(str, token, tok_array.src);

        printf("<loc:%d> %s\n", token.loc, str);

        free(str);
    }
}

TokenArray create_empty_token_array(uint64_t capacity) {
    TokenType *tokens_types;
    uint32_t *token_locs;
    const size_t alignment = VECTOR_SIZE;

    int result = posix_memalign((void**)&tokens_types, alignment, capacity * sizeof(TokenType));
    result |= posix_memalign((void**)&token_locs, alignment, capacity * sizeof(uint32_t));

    if (result) {
        fprintf(stderr, "Memory allocation failure.\n");
    }

    TokenArray tok_array;
    tok_array.token_types = tokens_types;
    tok_array.token_locs = token_locs;
    tok_array.capacity = capacity;
    tok_array.src = NULL;
    tok_array.size = 0;

    return tok_array;
}

void append_token(TokenArray *tok_array, Token token) {
    tok_array->token_types[tok_array->size] = token.type;
    tok_array->token_locs[tok_array->size] = token.loc;
    ++tok_array->size;
}

void append_tokens(TokenArray *tok_array, __m256i types, __m256i locs, int size, uint32_t start_idx) {
    _mm256_storeu_si256(
        (__m256i *) (tok_array->token_types + tok_array->size),
        types
    );

    uint64_t *locs_64 = (uint64_t*) &locs;
    __m256i start_idx_vec = _mm256_set1_epi32(start_idx);

    for (uint8_t i = 0; i < 4 && size > 0; ++i) {
        __m256i locs_expanded = _mm256_cvtepu8_epi32(
            _mm_set_epi64x(0, *(locs_64 + i))   // Set lower 64 bits to current locations
        );

        locs_expanded = _mm256_add_epi32(
            locs_expanded,
            start_idx_vec
        );

        _mm256_storeu_si256(
            (__m256i *) (tok_array->token_locs + tok_array->size),
            locs_expanded
        );

        tok_array->size += 8;   // Assume that we always read 8 bytes. Adjust size later
        size -= 8;
    }

    tok_array->size += size;    // Adjust size
}

void free_token_array(TokenArray tok_list) {
    free(tok_list.token_types);
    free(tok_list.token_locs);
}
