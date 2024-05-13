#include <immintrin.h>
#include <stdalign.h>
#include <stdio.h>
#include <string.h>

static void print_array_epi8(int8_t* array, int size) {
    for (int i = 0; i < size; ++i) {
        if (array[i] == -1) {
            printf("_ ");
            continue;
        }

        char str[5]; // Assuming maximum integer length is 4 digits (+ sign)
        sprintf(str, "%d", (unsigned char) array[i]); // Convert integer to string

        int len = strlen(str); // Get string length

        printf("%s%*s", str, 4 - len, ""); // Print the string with adjusted spacing
    }

    printf("\n");
}

static void print_m128_epi8(__m128i input) {
    alignas(16) int8_t values[16];
    _mm_store_si128((__m128i*)values, input);

    print_array_epi8(values, 16);
}

static void print_m256_epi8(__m256i input) {
    alignas(32) int8_t values[32];
    _mm256_store_si256((__m256i*)values, input);

    print_array_epi8(values, 32);
}

static void print_char(char *input) {
    for (int i = 0; i < 32; ++i) {
        if (input[i] == '\n') {
            printf("\\n ");
            continue;
        }
        printf("%c  ", input[i]); // Print the string with adjusted spacing
    }
    printf("\n");
}
