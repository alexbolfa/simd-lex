#ifndef LEXER_H
#define LEXER_H

#include <stdbool.h>

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

void replace_white_space(__m256i* vector);

__m256i run_sublexers(__m256i *current_vec, __m256i *next_vec, bool last_empty);

TokenArray lex_file(char *file_path, char **file_content);

__m256i mm256_cmpistrm_any(__m128i match, __m256i vector);

/**
 * Lexes single byte punctuators and overlays their ASCII code to a
 *  given vector of tags, marking start of tokens.
 *
 * @param current_vec A __m256i vector to tokenize.
 * @param tags A __m256i holding token tags.
 * @return
 */
void one_byte_punct_sub_lex(__m256i *current_vec, __m256i *tags);

/**
 * Shift current vector by one to the left, adding the first element
 *  of the the next vector at the end.
 *
 * @param current_vec Left __m256i vector to concatenate.
 * @param next_vec Right __m256i vector to concatenate.
 * @return Left vector shifted to left.
 */
__m256i look_ahead_one(__m256i current_vec, __m256i next_vec);

/**
 * Shift current vector by two to the left, adding the first two
 *  elements of the the next vector at the end.
 *
 * @param current_vec Left __m256i vector to concatenate.
 * @param next_vec Right __m256i vector to concatenate.
 * @return Left vector shifted to left.
 */
__m256i look_ahead_two(__m256i current_vec, __m256i next_vec);

/**
 * Transform bit mask into byte mask.
 *
 * @author Evgeny Kluev & Satya Arjunan
 * @param mask An int bitmask.
 * @return A byte mask corresponding to input.
 */
__m256i get_mask(const uint32_t mask);

/**
 * Remove leading bytes of the first 64 bits according to a
 *  given mask.
 *
 * @param vector A __m256i from which to remove.
 * @param prefix A uint64_t mask telling us which bits to
 *  remove from the first 8 bytes of vector.
 */
void remove_prefix_64(__m256i *vector, uint64_t prefix);

/**
 * Lexes two byte punctuators and overlays special code to a
 *  given vector of tags, marking start of tokens.
 *
 * @param current_vec A __m256i vector to tokenize.
 * @param next_vec A __m256i vector to the next batch of characters.
 * @param tags A __m256i holding token tags.
 * @return
 */
void two_byte_punct_sub_lex(__m256i *current_vec, __m256i *next_vec, __m256i *tags);

/**
 * Lexes three byte punctuators and overlays special code to a
 *  given vector of tags, marking start of tokens.
 *
 * @param current_vec A __m256i vector to tokenize.
 * @param next_vec A __m256i vector to the next batch of characters.
 * @param tags A __m256i holding token tags.
 * @return
 */
void three_byte_punct_sub_lex(__m256i *current_vec, __m256i *next_vec, __m256i *tags);

__m256i alpha_mask(__m256i vector);

void identifiers_sub_lex(__m256i current_vec, __m256i *tags, bool last_empty);

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
