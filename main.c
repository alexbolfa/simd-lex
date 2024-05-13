#include <stdio.h>

#include "lexer.h"

int main(int argc, char **argv) {
    if (argc != 2) {
        fprintf(stderr, "Usage: simd-lexer <file path>.\n");
        return 1;
    }

    char *file_content;
    TokenList *tokens = lex_file(argv[1], &file_content);

    print_tokens(tokens);

    // Clean up
    free(file_content);

    return 0;
}
