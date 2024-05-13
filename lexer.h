#ifndef LEXER_H
#define LEXER_H

#include <immintrin.h>

#include "tokens.h"

/**
 * Perform lexical analysis on the given file.
 *
 * @param input A pointer to the FILE structure representing
 *  the input file.
 * @param input_size Length of input.
 * @param file_path Path to the file of input.
 * @return A linked list of TokenNode structures.
 */
TokenList* lex(char *input, long input_size, char* file_path);

/**
 * Finds indices of even values that mark the start of tokens.
 *
 * @param vector An __m256i vector to search for even values.
 * @param token_indices A pointer to an uint8_t array where indices
 *  are stored.
 * @param size A pointer to a short where the number of even numbers
 *  is stored.
 */
void find_token_indices(__m256i vector, uint8_t *token_indices, int *size);

__m256i lex_vector(__m256i vector);

TokenList* lex_file(char* file_path, char **file_content);

/**
 * Lexes single byte punctuators and overlays their ASCII code to a
 *  given vector of tags.
 *
 * @param vector
 * @param tags
 * @return
 */
void one_byte_punct_sub_lex(__m256i vector, __m256i *tags);

__m256i load_vector(const char* pos);

/**
 * Generates a mask indicating elements within the specified range.
 *
 * @param vector An __m256i vector containing the elements to be
 *  compared.
 * @param lo The lower bound of the range (inclusive).
 * @param up The upper bound of the range (inclusive).
 * @return An __m256i mask with bytes set to 0xFF for elements within
 *  the range.
 */
__m256i range_mask(__m256i vector, uint8_t lo, uint8_t up);

char* read_file(const char *filename, long *file_size, long pad_multiple);

#endif //LEXER_H
