#include "bit.h"

#include <stddef.h> // for size_t
#include <stdint.h>
#include <stdlib.h> // for allocs
#include <string.h> // for memset

#include "bit_vector.h"

#define ONE_BYTE_SIZE 8
#define TWO_BYTES_SIZE 16
#define THREE_BYTES_SIZE 24
#define FOUR_BYTES_SIZE 32

#define FULL_BYTE  0xff

uint8_t create_byte(uint8_t byte, size_t num_bits){
    for (size_t i = 0; i < num_bits; ++i){
        bit_set(&byte, i);
    }
    return byte;
}

uint8_t invert_part_byte(uint8_t byte, size_t until_bit){
    uint8_t mask = FULL_BYTE >> (ONE_BYTE_SIZE - until_bit);
    return (~byte) & mask;
}

uint32_t create_word32(uint8_t byte0, uint8_t byte1, uint8_t byte2, uint8_t byte3){
    return byte0 + (byte1 << ONE_BYTE_SIZE) + (byte2 << TWO_BYTES_SIZE) + (byte3 << THREE_BYTES_SIZE);
}

bit_vector_t* bit_vector_create(size_t size, bit_t value){
    if (size == 0 || size > (size_t) (-FOUR_BYTES_SIZE)){
        return NULL;
    }
    bit_vector_t *vect = malloc(sizeof(bit_vector_t));

    if (vect != NULL){
        bit_vector_t result = {0, 0, NULL};
        size_t s = size;
        if (size % FOUR_BYTES_SIZE != 0) {
            s = (s / FOUR_BYTES_SIZE + 1) * FOUR_BYTES_SIZE;
        }
        result.content = calloc(s / FOUR_BYTES_SIZE, sizeof(uint32_t));
        if (result.content != NULL) {
            result.allocated = s;
            result.size = size;
        } else {
            return NULL;
        }
        *vect = result;
        if (value == 0){
            memset(vect->content, 0, s * sizeof(uint32_t) / FOUR_BYTES_SIZE);
        } else {
            size_t rest = size % FOUR_BYTES_SIZE;
            size_t num_bytes = size / FOUR_BYTES_SIZE * sizeof(uint32_t);
            memset(vect->content, FULL_BYTE, num_bytes);
            
            // Create the last uint32_t
            uint8_t last_byte0 = 0;
            uint8_t last_byte1 = 0;
            uint8_t last_byte2 = 0;
            uint8_t last_byte3 = 0;

            if (rest > THREE_BYTES_SIZE){
                last_byte0 = FULL_BYTE;
                last_byte1 = FULL_BYTE;
                last_byte2 = FULL_BYTE;
                last_byte3 = create_byte(last_byte3, rest - THREE_BYTES_SIZE);
            } else if (rest > TWO_BYTES_SIZE) {
                last_byte0 = FULL_BYTE;
                last_byte1 = FULL_BYTE;
                last_byte2 = create_byte(last_byte3, rest - TWO_BYTES_SIZE);
            } else if (rest > ONE_BYTE_SIZE) {
                last_byte0 = FULL_BYTE;
                last_byte1 = create_byte(last_byte3, rest - ONE_BYTE_SIZE);
            } else {
                last_byte0 = create_byte(last_byte3, rest);
            }
            
            uint32_t last_word = create_word32(last_byte0, last_byte1, last_byte2, last_byte3);
            vect->content[size / FOUR_BYTES_SIZE] = last_word;
        }
    }

    return vect;
}

bit_vector_t* bit_vector_cpy(const bit_vector_t* pbv){
    if (pbv == NULL){
        return NULL;
    }

    bit_t value = 0;
    bit_vector_t *copy = bit_vector_create(pbv->size, value);

    for (size_t i = 0; i < (pbv-> allocated)/FOUR_BYTES_SIZE ; ++i){
        copy->content[i] = pbv->content[i];
    }

    return copy;
}

bit_t bit_vector_get(const bit_vector_t* pbv, size_t index){
    if (pbv == NULL || index >= pbv->size){
        return 0;
    }

    bit_t value = 0;

    size_t index_in_word = index % FOUR_BYTES_SIZE;
    size_t index_of_word = index / FOUR_BYTES_SIZE;

    uint32_t word = pbv->content[index_of_word];

    if (index_in_word < ONE_BYTE_SIZE){
        value = bit_get((uint8_t) word, index_in_word);
    } else if (index_in_word < TWO_BYTES_SIZE){
        value = bit_get((uint8_t) (word >> ONE_BYTE_SIZE), index_in_word - ONE_BYTE_SIZE);
    } else if (index_in_word < THREE_BYTES_SIZE) {
        value = bit_get((uint8_t) (word >> TWO_BYTES_SIZE), index_in_word - TWO_BYTES_SIZE);
    } else {
        value = bit_get((uint8_t) (word >> THREE_BYTES_SIZE), index_in_word - THREE_BYTES_SIZE);
    }

    return value;
}

bit_vector_t* bit_vector_not(bit_vector_t* pbv){
    if (pbv == NULL){
        return NULL;
    }

    size_t num_words = pbv->size / FOUR_BYTES_SIZE;
    size_t rest = pbv->size % FOUR_BYTES_SIZE;

    for (size_t i = 0; i < num_words; ++i){
        pbv->content[i] = ~pbv->content[i];
    }

    if (rest != 0){
        uint32_t last_word = pbv->content[num_words];
        
        uint8_t last_byte0 = (uint8_t) last_word;
        uint8_t last_byte1 = (uint8_t) (last_word >> ONE_BYTE_SIZE);
        uint8_t last_byte2 = (uint8_t) (last_word >> TWO_BYTES_SIZE);
        uint8_t last_byte3 = (uint8_t) (last_word >> THREE_BYTES_SIZE);

        if (rest > THREE_BYTES_SIZE){
            last_byte0 = ~last_byte0;
            last_byte1 = ~last_byte1;
            last_byte2 = ~last_byte2;
            last_byte3 = invert_part_byte(last_byte3, rest - THREE_BYTES_SIZE);
        } else if (rest > TWO_BYTES_SIZE) {
            last_byte0 = ~last_byte0;
            last_byte1 = ~last_byte1;
            last_byte2 = invert_part_byte(last_byte2, rest - TWO_BYTES_SIZE);
        } else if (rest > ONE_BYTE_SIZE) {
            last_byte0 = ~last_byte0;
            last_byte1 = invert_part_byte(last_byte1, rest - ONE_BYTE_SIZE);
        } else {
            last_byte0 = invert_part_byte(last_byte0, rest);
        }

        last_word = create_word32(last_byte0, last_byte1, last_byte2, last_byte3);
        pbv->content[num_words] = last_word;
    }

    return pbv;
}

bit_vector_t* bit_vector_and(bit_vector_t* pbv1, const bit_vector_t* pbv2){
    if (pbv1 == NULL || pbv2 == NULL || pbv1->size != pbv2->size){
        return NULL;
    }

    size_t num_words = pbv1->allocated / FOUR_BYTES_SIZE;

    for (size_t i = 0; i < num_words; ++i){
        pbv1->content[i] = pbv1->content[i] & pbv2->content[i];
    }

    return pbv1;
}

bit_vector_t* bit_vector_or(bit_vector_t* pbv1, const bit_vector_t* pbv2){
    if (pbv1 == NULL || pbv2 == NULL || pbv1->size != pbv2->size){
        return NULL;
    }

    size_t num_words = pbv1->allocated / FOUR_BYTES_SIZE;

    for (size_t i = 0; i < num_words; ++i){
        pbv1->content[i] = pbv1->content[i] | pbv2->content[i];
    }

    return pbv1;
}

bit_vector_t* bit_vector_xor(bit_vector_t* pbv1, const bit_vector_t* pbv2){
    if (pbv1 == NULL || pbv2 == NULL || pbv1->size != pbv2->size){
        return NULL;
    }

    size_t num_words = pbv1->allocated / FOUR_BYTES_SIZE;

    for (size_t i = 0; i < num_words; ++i){
        pbv1->content[i] = pbv1->content[i] ^ pbv2->content[i];
    }

    return pbv1;
}

bit_vector_t* bit_vector_extract_zero_ext(const bit_vector_t* pbv, int64_t index, size_t size){
    if (size == 0){
        return NULL;
    }
    
    if (pbv == NULL){
        return bit_vector_create(size, 0);
    }

    bit_vector_t *result = bit_vector_create(size, 0);

    if (index % FOUR_BYTES_SIZE == 0){
        for (int64_t i = index / FOUR_BYTES_SIZE; i < size / FOUR_BYTES_SIZE; ++i){
            if (i >= 0 && i < pbv->allocated / FOUR_BYTES_SIZE) {
                result->content[i - index / FOUR_BYTES_SIZE] = pbv->content[i];
            }
        }
    } else{
        size_t offset = FOUR_BYTES_SIZE - (index % FOUR_BYTES_SIZE);

        for (int64_t i = index / FOUR_BYTES_SIZE; i < size / FOUR_BYTES_SIZE; ++i){
            uint32_t word1 = 0;
            uint32_t word2 = 0;

            if (index > 0){
                if (i < pbv->allocated / FOUR_BYTES_SIZE ){
                    word1 = pbv->content[i];
                }
                if (i < pbv->allocated / FOUR_BYTES_SIZE - 1){
                    word2 = pbv->content[i+1];
                }
            } else {
                if (i > 0 && i <= pbv->allocated / FOUR_BYTES_SIZE){
                    word1 = pbv->content[i-1];
                }
                if (i >= 0 && i < pbv->allocated / FOUR_BYTES_SIZE){
                    word2 = pbv->content[i];
                } 
            }

            uint32_t word = (word1 >> (32 - offset)) + (word2 << offset);
            result->content[i - index / FOUR_BYTES_SIZE] = word;
        }

    } 
    return result;
}

bit_vector_t* bit_vector_extract_wrap_ext(const bit_vector_t* pbv, int64_t index, size_t size){
    if (size == 0 || pbv == NULL){
        return NULL;
    }

    bit_vector_t *result = bit_vector_create(size, 0);

    if (index % FOUR_BYTES_SIZE == 0){
        for (int64_t i = index / FOUR_BYTES_SIZE; i < size / FOUR_BYTES_SIZE; ++i){
            if (i >= 0 && i < pbv->allocated / FOUR_BYTES_SIZE) {
                result->content[i - index / FOUR_BYTES_SIZE] = pbv->content[i % (size / FOUR_BYTES_SIZE + 1)];
            }
        }
    } else{
        size_t offset = FOUR_BYTES_SIZE - (index % FOUR_BYTES_SIZE);

        for (int64_t i = index / FOUR_BYTES_SIZE; i < size / FOUR_BYTES_SIZE; ++i){
            uint32_t word1 = 0;
            uint32_t word2 = 0;

            if (index > 0){
                if (i < pbv->allocated / FOUR_BYTES_SIZE ){
                    word1 = pbv->content[i];
                }
                if (i < pbv->allocated / FOUR_BYTES_SIZE - 1){
                    word2 = pbv->content[i+1];
                }
            } else {
                if (i > 0 && i <= pbv->allocated / FOUR_BYTES_SIZE){
                    word1 = pbv->content[i-1];
                }
                if (i >= 0 && i < pbv->allocated / FOUR_BYTES_SIZE){
                    word2 = pbv->content[i];
                } 
            }

            uint32_t word = (word1 >> (32 - offset)) + (word2 << offset);
            result->content[i - index / FOUR_BYTES_SIZE] = word;
        }

    } 
    return result;
}

void bit_vector_free(bit_vector_t** pbv){
    free(*pbv);
    pbv = NULL;
}
