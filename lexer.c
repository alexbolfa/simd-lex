#include "lexer.h"
#include "print_utils.c"

#include <stdio.h>
#include <stdlib.h>

TokenArray lex(char *input, long input_size) {
    TokenArray tokens = create_empty_token_array(input_size + 4);

    __m256i current_vec = load_vector(input);

    for (int i = 0; i < input_size; i += VECTOR_SIZE) {
        // Run sub lexers
        __m256i next_vec = load_vector(input + i + VECTOR_SIZE);
        __m256i tags = run_sublexers(current_vec, &next_vec);

        // Traverse tags
        int size;
        __m256i indices;
        find_token_indices(&tags, &indices, &size);

        // Add tokens
        append_tokens(&tokens, tags, indices, size);

        // Swap vectors
        current_vec = next_vec;
    }

    return tokens;
}

void mm256_pext(__m256i *vector, __m256i mask, int *size) {
    // Split vectors in 64bit segments that can be used by popcnt and pext
    const uint64_t *vector_64 = (uint64_t *) vector;
    const uint64_t *mask_64 = (uint64_t *) &mask;

    *size = 0;
    for (int i = 0; i < 4; ++i) {
        const uint64_t temp = _pext_u64(vector_64[i], mask_64[i]);

        *(uint64_t*)((uint8_t *) vector + *size) = temp; // Concatenate the next indices

        *size += _mm_popcnt_u64(mask_64[i]) >> 3;
    }
}

__m256i non_zero_mask(const __m256i vector) {
    __m256i mask = _mm256_cmpeq_epi8(vector, _mm256_setzero_si256());
    mask = _mm256_xor_si256(mask, _mm256_set1_epi32(-1));

    return mask;
}

void find_token_indices(__m256i *token_tags, __m256i *token_indices, int *size) {
    // Get mask of non-zero numbers
    __m256i mask = non_zero_mask(*token_tags);

    *token_indices = _mm256_setr_epi8(
        0, 1, 2, 3, 4, 5, 6, 7,
        8, 9, 10, 11, 12, 13, 14, 15,
        16, 17, 18, 19, 20, 21, 22, 23,
        24, 25, 26, 27, 28, 29, 30, 31
    );

    mm256_pext(token_indices, mask, size);
    mm256_pext(token_tags, mask, size);
}

__m256i run_sublexers(__m256i current_vec, __m256i *next_vec) {
    __m256i tags = _mm256_setzero_si256();

    // Run all sub lexers
    one_byte_punct_sub_lex(current_vec, &tags);
    two_byte_punct_sub_lex(current_vec, next_vec, &tags);

    return tags;
}

TokenArray lex_file(char *file_path, char **file_content) {
    long file_size;
    *file_content = read_file(file_path, &file_size, VECTOR_SIZE);

    TokenArray tokens = lex(*file_content, file_size);

    // Append end-of-file token
    append_token(
        &tokens,
        create_token(TOK_EOF, 0)
    );

    return tokens;
}

__m256i mm256_cmpistrm_any(__m128i match, __m256i vector) {
    // Split vector in two for `_mm_cmpistrm`
    __m128i low_vector = _mm256_extractf128_si256(vector, 0);
    __m128i high_vector = _mm256_extractf128_si256(vector, 1);

    __m128i low_outside_range_mask = _mm_cmpistrm(match, low_vector, (1 << 6));
    __m128i high_outside_range_mask = _mm_cmpistrm(match, high_vector, (1 << 6));

    return _mm256_set_m128i(
        high_outside_range_mask,
        low_outside_range_mask
    );
}

void one_byte_punct_sub_lex(__m256i vector, __m256i *tags) {
    // Punctuators with ASCII in range [40, 47] i.e. ()*+,-./
    __m256i mask = range_mask(vector, 40, 47);

    // Punctuators outside [40, 47] range i.e. !%&:;<=>?[]^{|}~
    // excluding preprocessing operator '#'
    __m128i match = _mm_loadu_si128((__m128i*)"!%&:;<=>?[]^{|}~");

    // Combines masks
    mask = _mm256_or_si256(
        mask,
        mm256_cmpistrm_any(match, vector)
    );

    // Overlay found one-byte punctators over tags
    *tags = _mm256_blendv_epi8(*tags, vector, mask);
}

__m256i look_ahead_one(__m256i current_vec, __m256i next_vec) {
    return _mm256_alignr_epi8(
        _mm256_permute2x128_si256(next_vec, current_vec, 3),
        current_vec,
        1   // Number of bytes that we shift
    );
}

__m256i get_mask(const uint32_t mask) {
    __m256i vmask = _mm256_set1_epi32(mask);

    const __m256i shuffle = _mm256_setr_epi64x(
        0x0000000000000000, 0x0101010101010101,
        0x0202020202020202, 0x0303030303030303
    );

    vmask = _mm256_shuffle_epi8(vmask, shuffle);

    // First diagonal is 0
    const __m256i bit_mask = _mm256_set1_epi64x(0x7fbfdfeff7fbfdfe);

    vmask = _mm256_or_si256(vmask, bit_mask);

    return _mm256_cmpeq_epi8(vmask, _mm256_set1_epi64x(-1));
}

void two_byte_punct_sub_lex(__m256i current_vec, __m256i *next_vec, __m256i *tags) {
    const __m256i shifted_1 = look_ahead_one(current_vec, *next_vec);

    // Search for first bytes: [-, %, *, <, ^, !, &, >, |, =, +, /]
    /* NOTE: I expect the compiler to optimize away these variables and
              run cmpeq in parallel to maximize throughput. */

    const char *first_bytes = ">+^!&*|%<-=/";
    uint32_t first_masks[12];

    for (int i = 0; i < 12; ++i) {
        first_masks[i] = _mm256_movemask_epi8(
            _mm256_cmpeq_epi8(
                current_vec,
                _mm256_set1_epi8(first_bytes[i])
            )
        );
    }

    // Search for second bytes: [+, &, |, <, -, =, >]
    const char *second_bytes = "+&|<-=>";
    uint32_t second_masks[7];

    for (int i = 0; i < 7; ++i) {
        second_masks[i] = _mm256_movemask_epi8(
                _mm256_cmpeq_epi8(
                shifted_1,
                _mm256_set1_epi8(second_bytes[i])
            )
        );
    }

    // Go through all two-byte punctuators
    const uint8_t punct_data[19][2] = {
        {4, 1},     // &&
        {9, 5},     // -=
        {0, 5},     // >=
        {4, 5},     // &=
        {9, 6},     // ->
        {0, 6},     // >>
        {5, 5},     // *=
        {11, 5},    // /=
        {2, 5},     // ^=
        {1, 0},     // ++
        {8, 3},     // <<
        {6, 5},     // |=
        {1, 5},     // +=
        {8, 5},     // <=
        {6, 2},     // ||
        {9, 4},     // --
        {10, 5},    // ==
        {3, 5},     // !=
        {7, 5},     // %=
    };

    // Store temporary found tags here to not delete from *tags
    uint32_t mask = 0;

    /* NOTE: I expect the compiler to optimize away loop local variables. */
    for (int i = 0; i < 19; ++i) {
        const uint8_t x = punct_data[i][0];
        const uint8_t y = punct_data[i][1];

        // Update temporary tags
        mask = mask | (first_masks[x] & second_masks[y]);
    }

    // Remove middle tag in series of three consecutive tags
    mask = mask ^ (mask & (mask << 1) & (mask >> 1));

    // Remove right tag in series of two consecutive tags
    mask = mask ^ (mask & (mask << 1));

    // Get token types
    __m256i tok_types = _mm256_sub_epi8(    // current_vec + shifted_1 - 2
        _mm256_adds_epu8(
            current_vec,
            shifted_1
        ),
        _mm256_set1_epi8(2)
    );

    // Update tags
    *tags = _mm256_blendv_epi8(
        *tags,
        tok_types,
        get_mask(mask)
    );

    // Remove second byte's tag
    *tags = _mm256_blendv_epi8(
        *tags,
        _mm256_setzero_si256(),
        get_mask(mask << 1)
    );

    // Remove first byte from next vector if it is a continuation of a current symbol
    uint8_t carry = ((mask & (1 << 31)) >> 31) * 0xFF;

    *next_vec = _mm256_blendv_epi8(
        _mm256_set1_epi8(32),   // Space ASCII value
        *next_vec,
        _mm256_setr_epi64x(
            0xffffffffffffffff - carry, 0xffffffffffffffff,
            0xffffffffffffffff,  0xffffffffffffffff
        )
    );
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
    char *file_content = (char *) malloc(size + padding + VECTOR_SIZE);

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
