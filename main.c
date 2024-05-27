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

void print_results(TokenArray tokens, bool time_flag, double avg_time) {
    if (time_flag) {
        printf("Avg. time: %f ms\n", avg_time);
    } else {
        print_tokens(tokens);
    };
}

int main(int argc, char **argv) {
    const int repeat_bench = 10;
    double avg_time = 0;
    bool time_flag;

    if (!parse_flags(argc, argv, &time_flag)) {
        return -1;
    }

    char *file_content;
    TokenArray tokens;

    int cnt = repeat_bench;
    do {
        // Start timer
        clock_t start = clock();

        // Run lexer
        tokens = lex_file(argv[1], &file_content);

        // Stop timer
        clock_t end = clock();
        double cpu_time_used = ((double) (end - start)) / CLOCKS_PER_SEC * 1000;
        avg_time += cpu_time_used;

        --cnt;
    } while (time_flag && cnt);

    // Results
    print_results(tokens, time_flag, avg_time / repeat_bench);

    // Clean up
    free(file_content);

    return 0;
}
