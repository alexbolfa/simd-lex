#ifndef LEXER_H
#define LEXER_H

#include "tokens.h"

/**
 * Perform lexical analysis on the given file.
 *
 * @param input A pointer to the FILE structure representing
 *  the input file.
 * @param input_size Length of input.
 * @return A linked list of TokenNode structures.
 */
TokenArray lex(char *input, long input_size);

/**
 * Parallel Bits Extract (PEXT) on 256 bit vectors.
 *
 * @param vector A __m256i vector from which it extracts.
 * @param mask A __m256i mask for each bit to extract.
 * @param size A pointer to an integer where the number of elements
 *  extracted is stored.
 */
void mm256_pext(__m256i *vector, __m256i mask, int *size);

/**
 * Find the mask of non-zero elements of a given vector.
 *
 * @param vector A __m256i input vector.
 * @return Mask of non-zero elements of input vector.
 */
__m256i non_zero_mask(const __m256i vector);

/**
 * Finds indices of even values that mark the start of tokens.
 *
 * @param token_tags An __m256i vector to search for even values.
 * @param token_indices A pointer to an uint8_t array where indices
 *  are stored.
 * @param size A pointer to an int where the number of tokens is
 *  stored.
 */
void find_token_indices(__m256i *token_tags, __m256i *token_indices, int *size);

__m256i run_sublexers(char *input);

TokenArray lex_file(char *file_path, char **file_content);

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
