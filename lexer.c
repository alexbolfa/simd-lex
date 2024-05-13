#define VECTOR_SIZE 32

#include "lexer.h"
#include "print_utils.c"

#include <stdio.h>
#include <stdlib.h>

TokenList* lex(char *input, long input_size, char* file_path) {
    TokenList* tokens = create_empty_token_list();
    char *curr_input = input;

    for (int i = 0; i < input_size; i += VECTOR_SIZE) {
        const __m256i vector = load_vector(curr_input);
        const __m256i tags = lex_vector(vector);  // Even-valued tags mark start of token
        const uint8_t* tags_array = (uint8_t*) &tags;

        // Traverse tags
        int size;
        uint8_t indices[VECTOR_SIZE];
        find_token_indices(tags, indices, &size);

        for (uint8_t j = 0; j < size; ++j) {
            const uint8_t loc = indices[j];

            append_token(
                tokens,
                create_token(
                    look_up(tags_array[loc]),
                    curr_input + loc,
                    file_path
                )
            );
        }

        curr_input += VECTOR_SIZE;
    }

    return tokens;
}

void find_token_indices(__m256i vector, uint8_t *token_indices, int *size) {
    // Get mask of even numbers
    __m256i even_mask = _mm256_set1_epi8(1);
    even_mask = _mm256_and_si256(vector, even_mask);
    even_mask = _mm256_xor_si256(even_mask, _mm256_set1_epi8(1));
    even_mask *= 0xFF;

    __m256i indices = _mm256_setr_epi8(
        0, 1, 2, 3, 4, 5, 6, 7,
        8, 9, 10, 11, 12, 13, 14, 15,
        16, 17, 18, 19, 20, 21, 22, 23,
        24, 25, 26, 27, 28, 29, 30, 31
    );

    // Split vectors in 64bit segments that can be used by popcnt and pext
    uint64_t *indices_64 = (uint64_t *) &indices;
    uint64_t *mask_64 = (uint64_t *) &even_mask;

    *size = 0;
    for (int i = 0; i < 4; ++i) {
        uint64_t temp = _pext_u64(indices_64[i], mask_64[i]);

        *(uint64_t*)(token_indices + *size) = temp; // Concatenate the next indices

        *size += _mm_popcnt_u64(mask_64[i]) >> 3;
    }
}

__m256i lex_vector(__m256i vector) {
    __m256i tags = _mm256_set1_epi8(-1);

    // Run all sub lexers
    one_byte_punct_sub_lex(vector, &tags);

    return tags;
}

TokenList* lex_file(char* file_path, char **file_content) {
    long file_size;
    *file_content = read_file(file_path, &file_size, VECTOR_SIZE);

    TokenList *tokens = lex(*file_content, file_size, file_path);

    // Append end-of-file token
    append_token(
        tokens,
        create_token(TOK_EOF, NULL, file_path)
        );

    return tokens;
}

void one_byte_punct_sub_lex(__m256i vector, __m256i *tags) {
    // Punctuators with ASCII in range [40, 47] i.e. ()*+,-./
    __m256i mask = range_mask(vector, 40, 47);

    // Punctuators outside [40, 47] range i.e. !%&:;<=>?[]^{|}~
    // excluding preprocessing operator '#'
    __m128i match = _mm_loadu_si128((__m128i*)"!%&:;<=>?[]^{|}~");

    // Split vector in two for `_mm_cmpistrm`
    __m128i low_vector = _mm256_extractf128_si256(vector, 0);
    __m128i high_vector = _mm256_extractf128_si256(vector, 1);

    __m128i low_outside_range_mask = _mm_cmpistrm(match, low_vector, (1 << 6));
    __m128i high_outside_range_mask = _mm_cmpistrm(match, high_vector, (1 << 6));

    __m256i outside_range_mask = _mm256_set_m128i(
        high_outside_range_mask,
        low_outside_range_mask
    );

    // Combines masks
    mask = _mm256_or_si256(mask, outside_range_mask);

    // Overlay found one-byte punctators over tags
    vector = _mm256_adds_epu8(vector, vector);  // Multiply by two
    *tags = _mm256_blendv_epi8(*tags, vector, mask);
}

void two_byte_punct_sub_lex(__m256i vector, __m256i *tags) {

}

__m256i load_vector(const char* pos) {
    return _mm256_loadu_si256((__m256i*) pos);
}

__m256i range_mask(__m256i vector, uint8_t lo, uint8_t up) {
    __m256i lo_bound = _mm256_set1_epi8(lo - 1);
    __m256i up_bound = _mm256_set1_epi8(up + 1);

    __m256i mask_gt = _mm256_cmpgt_epi8(vector, lo_bound);
    __m256i mask_lt = _mm256_cmpgt_epi8(up_bound, vector);

    return _mm256_and_si256(mask_lt, mask_gt);
}

char* read_file(const char *filename, long *file_size, long pad_multiple) {
    // Open file
    FILE *file = fopen(filename, "r");
    if (!file) {
        fprintf(stderr, "Error opening file.\n");
        return NULL;
    }

    // Get file size
    fseek(file, 0, SEEK_END);
    *file_size = ftell(file);
    rewind(file);

    // Allocate memory multiple of pad_multiple
    long size = *file_size + 1;
    long padding = (pad_multiple - (size % pad_multiple)) % pad_multiple;
    char *file_content = (char *) malloc(size + padding);

    if (!file_content) {
        fprintf(stderr, "Memory allocation failed.\n");
        fclose(file);
        return NULL;
    }

    // Read file content
    size_t bytes_read = fread(file_content, 1, *file_size, file);
    if (bytes_read != *file_size) {
        fprintf(stderr, "Error reading file.\n");
        free(file_content);
        fclose(file);
        return NULL;
    }

    file_content[*file_size] = '\0';    // Null-terminate the file content

    // Clean up
    fclose(file);

    return file_content;
}
