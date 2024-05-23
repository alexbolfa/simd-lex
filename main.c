#include <stdbool.h>
#include <stdio.h>
#include <string.h>
#include <time.h>

#include "lexer.h"

bool parse_flags(int argc, char **argv, bool *time_flag) {
    if (argc < 2 || argc > 3) {
        fprintf(stderr, "Usage: simd-lexer <file path> [-t/--time].\n");
        return false;
    }

    *time_flag = false;
    if (argc == 3 && (strcmp(argv[2], "-t") == 0 || strcmp(argv[2], "--time") == 0)) {
        *time_flag = true;
    }

    return true;
}

void print_results(TokenArray tokens, bool time_flag, clock_t start) {
    if (time_flag) {
        clock_t end = clock();
        double cpu_time_used = ((double) (end - start)) / CLOCKS_PER_SEC * 1000;
        printf("Time taken: %f ms\n", cpu_time_used);
    } else {
        print_tokens(tokens);
    };
}

int main(int argc, char **argv) {
    bool time_flag;

    if (!parse_flags(argc, argv, &time_flag)) {
        return -1;
    }

    // Start timer
    clock_t start = clock();

    // Run lexer
    char *file_content;
    TokenArray tokens = lex_file(argv[1], &file_content);

    // Results
    print_results(tokens, time_flag, start);

    // Clean up
    free(file_content);

    return 0;
}
