#include "lexer.h"

#include "print_utils.c"

#include <stdio.h>
#include <stdlib.h>

TokenArray lex(char *input, long input_size) {
    TokenArray tokens = create_empty_token_array(input_size + 4);
    tokens.src = input;

    char last_char = 0;
    bool ch_continue = 0;
    bool escaped_continue = 0;
    __m256i current_vec = load_vector(input);
    __m256i src_current_vec = load_vector(input);

    for (int i = 0; i < input_size; i += VECTOR_SIZE) {
        // Run sub lexers
        __m256i next_vec = load_vector(input + i + VECTOR_SIZE);
        const __m256i src_next_vec = load_vector(input + i + VECTOR_SIZE);

        __m256i tags = run_sublexers(
            &current_vec, &next_vec,
            src_current_vec, last_char, &ch_continue, &escaped_continue);

        // Traverse tags
        int size;
        __m256i indices;
        find_token_indices(&tags, &indices, &size);

        // Handle results
        append_tokens(&tokens, tags, indices, size, i);
        _mm256_storeu_si256((__m256i *)(input + i), current_vec);
        last_char = input[i + 31];

        // Swap vectors
        current_vec = next_vec;
        src_current_vec = src_next_vec;
    }

    short lookup[256] = {0};
    populate_keyword_lookup_table(lookup);
    find_keywords(&tokens, lookup);

    return tokens;
}

uint8_t hash(uint64_t val) {
    return ((((val >> 32) ^ val) & 0xffffffff) * (uint64_t)3523216747) >> 32;
}

void populate_keyword_lookup_table(short *lookup) {
    const char *keywords[] = {
        "", "auto", "break", "case", "char", "const", "continue", "default", "do",
        "double", "else", "enum", "extern", "float", "for", "goto", "if", "inline",
        "int", "long", "register", "restrict", "return", "short", "signed", "sizeof",
        "static", "struct", "switch", "typedef", "union", "unsigned", "void", "volatile",
        "while", "_Alignas", "_Alignof", "_Atomic", "_Bool", "_Complex", "_Generic",
        "_Imaginary", "_Noreturn", "_Static_assert", "_Thread_local"
    };

    int num_keywords = (sizeof(keywords) / sizeof(keywords[0]));
    for (int i = 1; i < num_keywords; ++i) {
        char keyword_copy[9] = {0};
        strncpy(keyword_copy, keywords[i], 8);

        uint64_t val = *((uint64_t*) keyword_copy);
        uint8_t hash_val = hash(val);

        lookup[hash_val] = i;
    }
}

void find_keywords(TokenArray *tok_array, short *lookup) {
    const char *keywords[] = {
        "", "auto", "break", "case", "char", "const", "continue", "default", "do",
        "double", "else", "enum", "extern", "float", "for", "goto", "if", "inline",
        "int", "long", "register", "restrict", "return", "short", "signed", "sizeof",
        "static", "struct", "switch", "typedef", "union", "unsigned", "void", "volatile",
        "while", "_Alignas", "_Alignof", "_Atomic", "_Bool", "_Complex", "_Generic",
        "_Imaginary", "_Noreturn", "_Static_assert", "_Thread_local"
    };

    const TokenType keyword_types[] = {0, TOK_AUTO, TOK_BREAK, TOK_CASE, TOK_CHAR,
        TOK_CONST, TOK_CONTINUE, TOK_DEFAULT, TOK_DO, TOK_DOUBLE, TOK_ELSE,
        TOK_ENUM, TOK_EXTERN, TOK_FLOAT, TOK_FOR, TOK_GOTO, TOK_IF, TOK_INLINE,
        TOK_INT, TOK_LONG, TOK_REGISTER, TOK_RESTRICT, TOK_RETURN, TOK_SHORT,
        TOK_SIGNED, TOK_SIZEOF, TOK_STATIC, TOK_STRUCT, TOK_SWITCH, TOK_TYPEDEF,
        TOK_UNION, TOK_UNSIGNED, TOK_VOID, TOK_VOLATILE, TOK_WHILE, TOK__ALIGNAS,
        TOK__ALIGNOF, TOK__ATOMIC, TOK__BOOL, TOK__COMPLEX, TOK__GENERIC,
        TOK__IMAGINARY, TOK__NORETURN, TOK__STATIC_ASSERT, TOK__THREAD_LOCAL};

    for (int i = 0; i < tok_array->size; i += VECTOR_SIZE) {
        const __m256i current_vec = load_vector((const char *) tok_array->token_types + i);
        const __m256i mask = _mm256_cmpeq_epi8(
            current_vec,
            _mm256_set1_epi8(TOK_IDENT)
        );

        int size;
        uint8_t token_indices[] __attribute__((aligned(32))) = {
            0, 1, 2, 3, 4, 5, 6, 7,
            8, 9, 10, 11, 12, 13, 14, 15,
            16, 17, 18, 19, 20, 21, 22, 23,
            24, 25, 26, 27, 28, 29, 30, 31
        };
        mm256_pext((__m256i *) token_indices, mask, &size);

        for (int j = 0; j < size; ++j) {
            int pos = i + token_indices[j];
            char str[9] = {0};
            strncpy(str, tok_array->src + tok_array->token_locs[pos], 8);

            const uint64_t val = *((uint64_t*) str);
            const uint8_t hash_val = hash(val);
            const uint8_t keyword_pos = lookup[hash_val];

            bool are_equal = strcmp(
                    tok_array->src + tok_array->token_locs[pos],
                    keywords[keyword_pos]
                ) == 0;
            TokenType keyword_type = keyword_types[keyword_pos];

            // are_equal ? keyword_id : TOK_IDENT
            tok_array->token_types[pos] = TOK_IDENT ^ ((keyword_type ^ TOK_IDENT) & -!!are_equal);;
        }
    }
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

void replace_white_space(__m256i* vector) {
    __m256i white_spaces_mask = _mm256_cmpeq_epi8(
       *vector,
       _mm256_set1_epi8(' ')
   );

    white_spaces_mask = _mm256_or_si256(
        white_spaces_mask,
        _mm256_cmpeq_epi8(
            *vector,
            _mm256_set1_epi8('\n')
        )
    );

    white_spaces_mask = _mm256_or_si256(
        white_spaces_mask,
        _mm256_cmpeq_epi8(
            *vector,
            _mm256_set1_epi8('\t')
        )
    );

    white_spaces_mask = _mm256_or_si256(
        white_spaces_mask,
        _mm256_cmpeq_epi8(
            *vector,
            _mm256_set1_epi8('\r')
        )
    );

    *vector = _mm256_blendv_epi8(
        *vector,
        _mm256_setzero_si256(),
        white_spaces_mask
    );
}

__m256i run_sublexers(__m256i *current_vec, __m256i *next_vec, const __m256i src_current_vec, char last_char, bool *ch_continue, bool *
                      escaped_continue) {
    __m256i tags = _mm256_setzero_si256();

    three_byte_punct_sub_lex(current_vec, next_vec, &tags);
    two_byte_punct_sub_lex(current_vec, next_vec, &tags);
    one_byte_punct_sub_lex(current_vec, *next_vec, &tags, last_char);

    replace_white_space(current_vec);

    identifiers_sub_lex(*current_vec, &tags, last_char == 0);
    numeric_const_sub_lex(*current_vec, &tags, last_char == 0);

    text_lit_sub_lex(current_vec, &tags, '\'',
                     ch_continue, TOK_CHAR_LIT,
                     src_current_vec, escaped_continue);

    return tags;
}

TokenArray lex_file(char *file_path, char **file_content) {
    long file_size;
    *file_content = read_file(file_path, &file_size, VECTOR_SIZE);

    TokenArray tokens = lex(*file_content, file_size);

    // Append end-of-file token
    append_token(
        &tokens,
        create_token(TOK_EOF, file_size)
    );

    return tokens;
}

__m256i mm256_cmpistrm_any(__m128i match, __m256i vector) {
    // Split vector in two for `_mm_cmpistrm`
    __m128i low_vector = _mm256_extractf128_si256(vector, 0);
    __m128i high_vector = _mm256_extractf128_si256(vector, 1);

    __m128i low_outside_range_mask = _mm_cmpestrm(match, 16, low_vector, 16, (1 << 6));
    __m128i high_outside_range_mask = _mm_cmpestrm(match, 16, high_vector, 16, (1 << 6));

    return _mm256_set_m128i(
        high_outside_range_mask,
        low_outside_range_mask
    );
}

__m256i numeric_periods_mask(__m256i current_vec, __m256i next_vec, char last_char) {
    uint64_t is_period = _mm256_movemask_epi8(
        _mm256_cmpeq_epi8(
            current_vec,
            _mm256_set1_epi8('.')
        )
    );
    uint64_t has_num_after = _mm256_movemask_epi8(num_mask(look_ahead_one(current_vec, next_vec)));
    uint64_t has_num_before = _mm256_movemask_epi8(num_mask(current_vec));
    has_num_before = (has_num_before << 1) | (last_char >= 0 && last_char <= 9);

    return get_mask(is_period & (has_num_before | has_num_after));
}

void one_byte_punct_sub_lex(__m256i *current_vec, __m256i next_vec, __m256i *tags, char last_char) {
    // Punctuators with ASCII in range [40, 47] i.e. ()*+,-./
    __m256i mask = range_mask(*current_vec, 40, 47);

    // Punctuators outside [40, 47] range i.e. !%&:;<=>?[]^{|}~
    // excluding preprocessing operator '#'
    __m128i match = _mm_loadu_si128((__m128i*)"!%&:;<=>?[]^{|}~");

    // Combines masks
    mask = _mm256_or_si256(
        mask,
        mm256_cmpistrm_any(match, *current_vec)
    );

    // Ignore periods part of numeric constants
    mask = _mm256_xor_si256(
        mask,
        numeric_periods_mask(*current_vec, next_vec, last_char)
    );

    // Overlay found one-byte punctators over tags
    *tags = _mm256_blendv_epi8(*tags, *current_vec, mask);

    // Remove symbols found
    *current_vec = _mm256_blendv_epi8(
        *current_vec,
        _mm256_setzero_si256(),
        mask
    );
}

__m256i look_ahead_one(__m256i current_vec, __m256i next_vec) {
    return _mm256_alignr_epi8(
        _mm256_permute2x128_si256(next_vec, current_vec, 3),
        current_vec,
        1   // Number of bytes that we shift (compile constant)
    );
}

__m256i look_ahead_two(__m256i current_vec, __m256i next_vec) {
    return _mm256_alignr_epi8(
        _mm256_permute2x128_si256(next_vec, current_vec, 3),
        current_vec,
        2   // Number of bytes that we shift (compile constant)
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

void remove_prefix_64(__m256i *vector, uint64_t prefix) {
    *vector = _mm256_blendv_epi8(
        _mm256_set1_epi8(0),   // Space ASCII value
        *vector,
        _mm256_setr_epi64x(
            0xffffffffffffffff - prefix, 0xffffffffffffffff,
            0xffffffffffffffff,  0xffffffffffffffff
        )
    );
}

void two_byte_punct_sub_lex(__m256i *current_vec, __m256i *next_vec, __m256i *tags) {
    const __m256i shifted_1 = look_ahead_one(*current_vec, *next_vec);

    // Search for first bytes: [-, %, *, <, ^, !, &, >, |, =, +, /]
    /* NOTE: I expect the compiler to optimize away these variables and
              run cmpeq in parallel to maximize throughput. */

    const char *first_bytes = ">+^!&*|%<-=/";
    uint32_t first_masks[12];

    for (int i = 0; i < 12; ++i) {
        first_masks[i] = _mm256_movemask_epi8(
            _mm256_cmpeq_epi8(
                *current_vec,
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
            *current_vec,
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

    // Remove symbols found
    *current_vec = _mm256_blendv_epi8(
        *current_vec,
        _mm256_setzero_si256(),
        get_mask(mask | mask << 1)
    );

    // Remove first byte from next vector if it is a continuation of a current symbol
    uint8_t carry = ((mask & (1 << 31)) >> 31) * 0xFF;

    remove_prefix_64(next_vec, carry);
}

void three_byte_punct_sub_lex(__m256i *current_vec, __m256i *next_vec, __m256i *tags) {
    // Lex [..., <<=, >>=]
    __m256i shifted_one = look_ahead_one(*current_vec, *next_vec);
    __m256i shifted_two = look_ahead_two(*current_vec, *next_vec);

    /* NOTE: I expect the compiler to optimize away these variables and
              run cmpeq in parallel to maximize throughput. */
    const char *first_two_bytes = ".<>";

    uint32_t first_masks[3];
    uint32_t second_masks[3];
    uint32_t third_mask[2];

    for (int i = 0; i < 3; ++i) {
        first_masks[i] = _mm256_movemask_epi8(
            _mm256_cmpeq_epi8(
                *current_vec,
                _mm256_set1_epi8(first_two_bytes[i])
            )
        );

        second_masks[i] = _mm256_movemask_epi8(
            _mm256_cmpeq_epi8(
                shifted_one,
                _mm256_set1_epi8(first_two_bytes[i])
            )
        );
    }

    const char *third_bytes = ".=";

    for (int i = 0; i < 2; ++i) {
        third_mask[i] = _mm256_movemask_epi8(
            _mm256_cmpeq_epi8(
                shifted_two,
                _mm256_set1_epi8(third_bytes[i])
            )
        );
    }

    const uint8_t punct_data[3][3] = {
        {0, 0, 0},  // ...
        {1, 1, 1},  // <<=
        {2, 2, 1},  // >>=
    };

    uint32_t mask = 0;

    /* NOTE: I expect the compiler to optimize away loop local variables. */
    for (int i = 0; i < 3; ++i) {
        const uint8_t x = punct_data[i][0];
        const uint8_t y = punct_data[i][1];
        const uint8_t z = punct_data[i][2];

        mask = mask | (first_masks[x] & second_masks[y] & third_mask[z]);
    }

    // current_vec + shifted_one + shifted_two
    __m256i tok_types = _mm256_add_epi8(
        *current_vec,
        _mm256_add_epi8(
            shifted_one,
            shifted_two
        )
    );

    // Update tags
    *tags = _mm256_blendv_epi8(
        *tags,
        tok_types,
        get_mask(mask)
    );

    // Remove tail bytes tags
    *tags = _mm256_blendv_epi8(
        *tags,
        _mm256_setzero_si256(),
        get_mask(mask << 1 | mask << 2)
    );

    // Remove symbols found
    *current_vec = _mm256_blendv_epi8(
        *current_vec,
        _mm256_setzero_si256(),
        get_mask(mask | mask << 1 | mask << 2)
    );

    // Remove characters from next_vec if they are a continuation of a current symbol
    uint64_t carry = ((mask & (1 << 31)) >> 31) * 0xFFFF        // Remove first two bytes of next_vec
                        + ((mask & (1 << 30)) >> 30) * 0xFF;    // Remove first byte of next_vec

    remove_prefix_64(next_vec, carry);
}

__m256i alpha_mask(__m256i vector) {
    const __m256i A = _mm256_set1_epi8('A' - 1);
    const __m256i Z = _mm256_set1_epi8('Z' + 1);
    const __m256i a = _mm256_set1_epi8('a' - 1);
    const __m256i z = _mm256_set1_epi8('z' + 1);

    const __m256i is_upper = _mm256_and_si256(
        _mm256_cmpgt_epi8(vector, A),
        _mm256_cmpgt_epi8(Z, vector)
    );

    const __m256i is_lower = _mm256_and_si256(
        _mm256_cmpgt_epi8(vector, a),
        _mm256_cmpgt_epi8(z, vector)
    );

    return _mm256_or_si256(is_upper, is_lower);
}

void identifiers_sub_lex(__m256i current_vec, __m256i *tags, bool last_empty) {
    __m256i is_alpha = alpha_mask(current_vec);
    __m256i is_underscore = _mm256_cmpeq_epi8(
        current_vec,
        _mm256_set1_epi8('_')
    );

    __m256i ident_start_mask = _mm256_or_si256(
        is_alpha,
        is_underscore
    );

    uint32_t has_whitespace_before = _mm256_movemask_epi8(
        _mm256_cmpeq_epi8(
            current_vec,
            _mm256_setzero_si256()
        )
    );
    has_whitespace_before = (has_whitespace_before << 1) | last_empty;

    ident_start_mask = _mm256_and_si256(
        ident_start_mask,
        get_mask(has_whitespace_before)
    );

    *tags = _mm256_blendv_epi8(
        *tags,
        _mm256_set1_epi8(TOK_IDENT),
        ident_start_mask
    );
}

__m256i num_mask(__m256i vector) {
    const __m256i zero = _mm256_set1_epi8('0' - 1);
    const __m256i nine = _mm256_set1_epi8('9' + 1);

    __m256i is_ge_zero = _mm256_cmpgt_epi8(vector, zero);
    __m256i is_le_nine = _mm256_cmpgt_epi8(nine, vector);

    return _mm256_and_si256(is_ge_zero, is_le_nine);
}

void numeric_const_sub_lex(
    const __m256i current_vec,
    __m256i *tags,
    const bool last_empty
) {
    __m256i num_start_mask = num_mask(current_vec);

    uint32_t has_whitespace_before = _mm256_movemask_epi8(
        _mm256_cmpeq_epi8(
            current_vec,
            _mm256_setzero_si256()
        )
    );
    has_whitespace_before = (has_whitespace_before << 1) | last_empty;

    __m256i is_num_period = _mm256_cmpeq_epi8(
        current_vec,    // All periods in current_vec are for numbers
        _mm256_set1_epi8('.')
    );

    num_start_mask = _mm256_and_si256(
        num_start_mask,
        get_mask(has_whitespace_before)
    );

    num_start_mask = _mm256_or_si256(
        num_start_mask,
        _mm256_and_si256(
            is_num_period,
            get_mask(has_whitespace_before)
        )
    );

    *tags = _mm256_blendv_epi8(
        *tags,
        _mm256_set1_epi8(TOK_NUM),
        num_start_mask
    );
}

// TODO: remember if backslashes continue (if so remove or add)
void text_lit_sub_lex(
    __m256i *current_vec,
    __m256i *tags,
    const char delim,
    bool *does_continue,
    const TokenType type,
    const __m256i src_current_vec,
    bool *escaped_continue
) {
    uint32_t is_delim = _mm256_movemask_epi8(  // Delimiter
        _mm256_cmpeq_epi8(
            *current_vec,
            _mm256_set1_epi8(delim)
        )
    );

    uint32_t B = _mm256_movemask_epi8(  // Backslash
        _mm256_cmpeq_epi8(
            *current_vec,
            _mm256_set1_epi8('\\')
        )
    );

    const uint32_t O = 0xAAAAAAAA;  // 10101010101010101010101010101010 in binary
    const uint32_t E = 0x55555555;  // 01010101010101010101010101010101 in binary

    B = (B ^ (*escaped_continue)) & B;  // Remove escaped backslash

    uint64_t escaped_ch = (((B + (B & ~(B << 1)& E))& ~B)& ~E) | (((B+ ((B & ~(B << 1))& O))&  ~B)& E);

    escaped_ch ^= *escaped_continue;  // Add first character which might be escaped
    is_delim ^= (escaped_ch & is_delim);

    uint32_t region = _mm_cvtsi128_si32(    // Literal region
        _mm_clmulepi64_si128(
            _mm_set_epi32(0, 0, 0, is_delim ^ (*does_continue)),
            _mm_set1_epi8(-1),
            0
        )
    );

    // Remove tags inside literal
    *tags = _mm256_blendv_epi8(
        *tags,
        _mm256_setzero_si256(),
        get_mask(region)
    );

    *does_continue = (region >> 31) & 1;
    *escaped_continue = ((B >> 31) & 1) & (!((escaped_ch >> 31) & 1));

    // Add token literals
    is_delim &= region; // Keep only starting delimiters

    *tags = _mm256_blendv_epi8(
        *tags,
        _mm256_set1_epi8(type),
        get_mask(is_delim)
    );

    *current_vec = _mm256_blendv_epi8(
        *current_vec,
        src_current_vec,
        get_mask(region)
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
    size += padding;

    // Allocate aligned memory
    char *file_content;
    int result = posix_memalign((void **)&file_content, pad_multiple, size + VECTOR_SIZE);
    if (result != 0) {
        fprintf(stderr, "Memory allocation failed: %s.\n", strerror(result));
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
