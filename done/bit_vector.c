/**
 * @file bit_vector.c
 * @author Joseph Abboud & Zad Abi Fadel
 * @brief Basic functions used for bit operations and manipualtions on large sizes of vectors
 * @date 2020
 *
 */

#include "bit.h"

#include <stddef.h> // for size_t
#include <stdint.h>
#include <stdlib.h> // for allocs
#include <string.h> // for memset
#include <stdio.h>

#include "bit_vector.h"

#define BYTES_IN_WORD 4

#define ONE_BYTE_SIZE 8
#define TWO_BYTES_SIZE 16
#define THREE_BYTES_SIZE 24
#define FOUR_BYTES_SIZE 32

#define FULL_BYTE  0xff

/**
* @brief Set the num_bits LSBs of a given byte to 1 and the rest to 0
*  example: create_byte(0, 5) == 0b00011111
*
* @param byte The byte to change
* @param num_bits The number of LSBs that are set to 1
*/
void create_byte(uint8_t *byte, size_t num_bits)
{
    for (size_t i = 0; i < num_bits; ++i) {
        bit_set(byte, i);
    }
}

/**
 * @brief Compute the bit-by-bit inverse of a byte, then keeps the until_bit LSBs
 *  example: invert_part_byte(0b01001010, 5) ==  00010101
 *
 * @param byte The byte to change
 * @param until_bit The number of LSBs to keep after inverting
 * @return uint8_t The new byte value
 */
uint8_t invert_part_byte(uint8_t byte, size_t until_bit)
{
    uint8_t mask = FULL_BYTE >> (ONE_BYTE_SIZE - until_bit);
    return (~byte) & mask;
}

/**
 * @brief Create a uint32_t out of an array of 4 uint8_t's
 *
 * @param bytes The array containing the four bytes
 * @return uint32_t The new word
 */
uint32_t create_word32_arr(uint8_t bytes[BYTES_IN_WORD])
{
    return bytes[0] + (bytes[1]  << ONE_BYTE_SIZE) + (bytes[2]  << TWO_BYTES_SIZE) + (bytes[3]  << THREE_BYTES_SIZE);
}

/**
 * @brief Create a uint8_t array from a given uint32_t
 *
 * NB: This method uses a malloc call, so pay attention to using a
 *      free after calling it if necessary
 *
 * @param word The 32-bit word to divide
 * @return uint8_t* Pointer to the first byte (least significant)
 */
uint8_t * create_bytes_from_word(uint32_t word)
{
    uint8_t *bytes = malloc(BYTES_IN_WORD);

    if (bytes != NULL) {
        for (int i = 0; i < BYTES_IN_WORD; ++i) {
            bytes[i] = (uint8_t) (word >> (i * ONE_BYTE_SIZE));
        }
    }
    return bytes;
}

/**
 * @brief Set the value of a given bit in a bit vector to a given value
 *
 * @param pbv The pointer to the bit vector
 * @param index The index of the bit
 * @param value The bit value we want to assign at index (1 or 0)
 * @return bit_vector_t* The new bit_vector after change
 */
bit_vector_t* bit_vector_set(bit_vector_t* pbv, size_t index, bit_t value)
{
    if (pbv == NULL || index >= pbv->size) {
        return NULL;
    }

    size_t index_in_word = index % FOUR_BYTES_SIZE;
    size_t index_of_word = index / FOUR_BYTES_SIZE;

    uint32_t word = pbv->content[index_of_word];
    uint8_t *bytes = create_bytes_from_word(word);

    for (int i = 0; i < BYTES_IN_WORD; ++i) {
        if (index_in_word < (i + 1) * ONE_BYTE_SIZE) {
            bit_edit(&bytes[i], index_in_word % ONE_BYTE_SIZE, value);
            break;
        }
    }

    word = create_word32_arr(bytes);
    free(bytes);
    bytes=NULL;

    pbv->content[index_of_word] = word;
    return pbv;
}

// ==== see bit_vector.h ========================================
bit_vector_t* bit_vector_create(size_t size, bit_t value)
{
    if (size == 0 || size > (size_t) (-FOUR_BYTES_SIZE)) {
        return NULL;
    }
    // ALlocate space in the memory for the vector
    bit_vector_t *vect = malloc(sizeof(bit_vector_t));

    if (vect != NULL) {
        bit_vector_t result = {0, 0, NULL};
        size_t alloc = size;

        // Check if size is a multiple of 32, because if not, the allocated number of bits
        // in the memory is the given size rounded upwards to the nearest multiple of 32
        if (size % FOUR_BYTES_SIZE != 0) {
            alloc = (alloc / FOUR_BYTES_SIZE + 1) * FOUR_BYTES_SIZE;
        }
        // Allocate space in the memory for the array of uint32_t
        result.content = calloc(alloc / FOUR_BYTES_SIZE+1, sizeof(uint32_t));
        if (result.content != NULL) {
            result.allocated = alloc;
            result.size = size;
        } else {
            return NULL;
        }
        *vect = result;
        // Set the bits inside the array to all ones or all zeroes (depends on value)
        if (value == 0) {
            memset(vect->content, 0, alloc * sizeof(uint32_t) / FOUR_BYTES_SIZE);
        } else {
            size_t rest = size % FOUR_BYTES_SIZE;
            size_t num_bytes = size / FOUR_BYTES_SIZE * sizeof(uint32_t);
            memset(vect->content, FULL_BYTE, num_bytes);

            // Create the last uint32_t by working on 8 bits instead of 32
            uint8_t bytes[BYTES_IN_WORD] = {FULL_BYTE, FULL_BYTE, FULL_BYTE, FULL_BYTE};

            for (size_t i = 0; i < BYTES_IN_WORD; ++i) {
                if (rest > i * ONE_BYTE_SIZE) {
                    create_byte(&bytes[i], rest % ONE_BYTE_SIZE);
                    break;
                }
            }
        // Joining the four 8-bit values into a 32-bit word
        uint32_t last_word = create_word32_arr(bytes);
        vect->content[size / FOUR_BYTES_SIZE] = last_word;
            
        }
    }
    return vect;
}

// ==== see bit_vector.h ========================================
bit_vector_t* bit_vector_cpy(const bit_vector_t* pbv)
{
    if (pbv == NULL) {
        return NULL;
    }

    // Initialize an empty vector of same size as pbv
    bit_vector_t *copy = bit_vector_create(pbv->size, 0);

    // Deep copy of the content of pbv into the copy
    for (size_t i = 0; i < (pbv-> allocated)/FOUR_BYTES_SIZE ; ++i) {
        copy->content[i] = pbv->content[i];
    }

    return copy;
}

// ==== see bit_vector.h ========================================
bit_t bit_vector_get(const bit_vector_t* pbv, size_t index)
{
    if (pbv == NULL || index >= pbv->size) {
        return 0;
    }

    bit_t value = 0;

    size_t index_in_word = index % FOUR_BYTES_SIZE; // Index of bit in target word
    size_t index_of_word = index / FOUR_BYTES_SIZE; // Index of word in content array

    uint32_t word = pbv->content[index_of_word];
    uint8_t *bytes = create_bytes_from_word(word);

    for (int i = 0; i < BYTES_IN_WORD; ++i) {
        if (index_in_word < (i + 1) * ONE_BYTE_SIZE) {
            value = bit_get(bytes[i], index_in_word % ONE_BYTE_SIZE);
            break;
        }
    }
    free(bytes);
    bytes=NULL;
    return value;
}

// ==== see bit_vector.h ========================================
bit_vector_t* bit_vector_not(bit_vector_t* pbv)
{
    if (pbv == NULL) {
        return NULL;
    }

    size_t num_words = pbv->size / FOUR_BYTES_SIZE;
    size_t rest = pbv->size % FOUR_BYTES_SIZE;

    for (size_t i = 0; i < num_words; ++i) {
        pbv->content[i] = ~pbv->content[i];
    }

    if (rest != 0) {
        uint32_t last_word = pbv->content[num_words];
        uint8_t *bytes = create_bytes_from_word(last_word); // First divide the last word as is

        for (size_t i = BYTES_IN_WORD - 1; i >= 0; --i) {
            if (rest > i * ONE_BYTE_SIZE) {
                // Invert the byte containing the bit at index pbv->size accordingly
                bytes[i] = invert_part_byte(bytes[i], rest % ONE_BYTE_SIZE);
                // Invert the lesser significant bytes completely,
                // the more significant ones stay the same
                for (size_t j = 0; j < i; ++j) {
                    bytes[j] = ~bytes[j];
                }
                break;
            }
        }

        last_word = create_word32_arr(bytes);
        free(bytes);
        bytes=NULL;
        pbv->content[num_words] = last_word;
    }
    return pbv;
}

// ==== see bit_vector.h ========================================
bit_vector_t* bit_vector_and(bit_vector_t* pbv1, const bit_vector_t* pbv2)
{
    if (pbv1 == NULL || pbv2 == NULL || pbv1->size != pbv2->size) {
        return NULL;
    }

    size_t num_words = pbv1->allocated / FOUR_BYTES_SIZE;

    for (size_t i = 0; i < num_words; ++i) {
        pbv1->content[i] = pbv1->content[i] & pbv2->content[i];
    }

    return pbv1;
}

// ==== see bit_vector.h ========================================
bit_vector_t* bit_vector_or(bit_vector_t* pbv1, const bit_vector_t* pbv2)
{
    if (pbv1 == NULL || pbv2 == NULL || pbv1->size != pbv2->size) {
        return NULL;
    }

    size_t num_words = pbv1->allocated / FOUR_BYTES_SIZE;

    for (size_t i = 0; i < num_words; ++i) {
        pbv1->content[i] = pbv1->content[i] | pbv2->content[i];
    }

    return pbv1;
}

// ==== see bit_vector.h ========================================
bit_vector_t* bit_vector_xor(bit_vector_t* pbv1, const bit_vector_t* pbv2)
{
    if (pbv1 == NULL || pbv2 == NULL || pbv1->size != pbv2->size) {
        return NULL;
    }

    size_t num_words = pbv1->allocated / FOUR_BYTES_SIZE;

    for (size_t i = 0; i < num_words; ++i) {
        pbv1->content[i] = pbv1->content[i] ^ pbv2->content[i];
    }
    return pbv1;
}

// ==== see bit_vector.h ========================================
bit_vector_t* bit_vector_extract_zero_ext(const bit_vector_t* pbv, int64_t index, size_t size)
{
    if (size == 0) {
        return NULL;
    }

    if (pbv == NULL) {
        return bit_vector_create(size, 0);
    }
    // Initialize the resulting vector as an empty one
    bit_vector_t *result = bit_vector_create(size, 0);

    // Go through pbv bit by bit and set the equivalent bit in result as such:
    // 1- We compute the new bit index (i + index)
    // 2- If index is within the range of pbv, take the bit to set from pbv,
    // 3- Else the bit is 0
    for (size_t i = 0; i< size; ++i) {
        bit_t value = 0;
        if (i+index < pbv->size) {
            value = bit_vector_get(pbv, i + index);
        }
        result = bit_vector_set(result, i, value);
    }
    return result;
}

// ==== see bit_vector.h ========================================
bit_vector_t* bit_vector_extract_wrap_ext(const bit_vector_t* pbv, int64_t index, size_t size)
{
    if (size == 0 || pbv == NULL) {
        return NULL;
    }

    // Initialize the resulting vector as an empty one
    bit_vector_t *result = bit_vector_create(size, 0);

    // Go through pbv bit by bit and set the equivalent bit in result as such:
    // 1- We compute the new bit index (i + index)
    // 2- If index is within the range of pbv, take the bit to set from pbv,
    // 3- Else compute a new index (old index % size of pbv) and take the bit from pbv
    for (size_t i = 0; i< size; ++i) {
        bit_t value = 0;
        if (i+index < pbv->size) {
            value = bit_vector_get(pbv, i + index);
        } else {
            value = bit_vector_get(pbv, (i+index) % pbv->size);
        }
        result = bit_vector_set(result, i, value);
    }
    return result;
}

// ==== see bit_vector.h ========================================
bit_vector_t* bit_vector_shift(const bit_vector_t* pbv, int64_t shift)
{
    if (pbv == NULL) {
        return NULL;
    }

    return bit_vector_extract_zero_ext(pbv, -shift, pbv->size);
}

// ==== see bit_vector.h ========================================
bit_vector_t* bit_vector_join(const bit_vector_t* pbv1, const bit_vector_t* pbv2, int64_t shift)
{
    if (pbv1 == NULL || pbv2 == NULL || pbv1->size != pbv2->size || shift < 0 || shift > pbv1->size) {
        return NULL;
    }
    // bit_vector_t *pbv1_cpy = bit_vector_cpy(pbv1);
    // bit_vector_t *pbv2_cpy = bit_vector_cpy(pbv2);
    bit_vector_t pbv1_cpy = *pbv1;
    bit_vector_t pbv2_cpy = *pbv2;

    // bit_vector_t *pbv_m = bit_vector_create(pbv1->size, 1);
    bit_vector_t *pbv_m = bit_vector_create(pbv1->size, 1);

    bit_vector_t *pbv_mask = bit_vector_extract_zero_ext(pbv_m, -shift, pbv1->size);
    // const bit_vector_t *pbv2_shift = bit_vector_and(pbv2_cpy, pbv_mask);
    // bit_vector_t *pbv1_shift = bit_vector_and(pbv1_cpy, bit_vector_not(pbv_mask));

    // bit_vector_t *result = bit_vector_or(pbv1_shift, pbv2_shift);
    bit_vector_t *result = 
        bit_vector_or(
            bit_vector_and(&pbv1_cpy, bit_vector_not(pbv_mask)), 
            bit_vector_and(&pbv2_cpy, pbv_mask));
    result = bit_vector_cpy(result);
    bit_vector_free(&pbv_m);
    bit_vector_free(&pbv_mask);
    // free(pbv_m->content);
    // free(pbv_mask->content);
    // bit_vector_free(pbv1_cpy);
    // bit_vector_free(pbv2_cpy);

    return result;
}

#define BYTE_TO_BINARY_PATTERN "%c%c%c%c%c%c%c%c"
#define BYTE_TO_BINARY(byte)  \
  (byte & 0x80 ? '1' : '0'), \
  (byte & 0x40 ? '1' : '0'), \
  (byte & 0x20 ? '1' : '0'), \
  (byte & 0x10 ? '1' : '0'), \
  (byte & 0x08 ? '1' : '0'), \
  (byte & 0x04 ? '1' : '0'), \
  (byte & 0x02 ? '1' : '0'), \
  (byte & 0x01 ? '1' : '0')

// ==== see bit_vector.h ========================================
int bit_vector_print(const bit_vector_t* pbv)
{
    if(pbv==NULL){
        fprintf(stderr, "bit vector to print is NULL\n");
        return 0;
    }
    for (size_t i = 0; i < pbv->size / 32 ; ++i) {
        printf(""BYTE_TO_BINARY_PATTERN""BYTE_TO_BINARY_PATTERN""BYTE_TO_BINARY_PATTERN""BYTE_TO_BINARY_PATTERN"",
               BYTE_TO_BINARY(pbv->content[i]>>24), BYTE_TO_BINARY(pbv->content[i]>>16), BYTE_TO_BINARY(pbv->content[i]>>8), BYTE_TO_BINARY(pbv->content[i]));
    }
    return pbv->size;
}

// ==== see bit_vector.h ========================================
int bit_vector_println(const char* prefix, const bit_vector_t* pbv)
{
    if(pbv==NULL){
        fprintf(stderr, "bit vector to print is NULL\n");
        return 0;
    }
    printf("%s", prefix);
    int printed = bit_vector_print(pbv);
    printf("\n");
    return strlen(prefix) + printed + 1; // +1 is for the '\n'
}

// ==== see bit_vector.h ========================================
void bit_vector_free(bit_vector_t** pbv)
{
    if(pbv == NULL){
        return;
    }
    free((*pbv)->content);

    free(*pbv);
    pbv = NULL;
}
