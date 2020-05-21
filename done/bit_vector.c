#include "bit.h"

#include <stddef.h> // for size_t
#include <stdint.h>

#include "bit_vector.h"

#define VECTOR_SIZE 32

uint8_t create_byte(uint8_t byte, size_t num_bits){
    for (size_t i = 0; i < num_bits; ++i){
        bit_set(&byte, i);
    }
    return byte;
}

bit_vector_t* bit_vector_create(size_t size, bit_t value){
    if (size == 0 || size >= (size_t) (-31)){
        return NULL;
    }
    bit_vector_t *vect = malloc(sizeof(bit_vector_t));

    if (vect != NULL){
        bit_vector_t result = {0, 0, NULL};
        size_t s = size;
        if (size % VECTOR_SIZE != 0) {
            s = (s / VECTOR_SIZE + 1) * VECTOR_SIZE;
        }
        result.content = calloc(s / VECTOR_SIZE, sizeof(uint32_t));
        if (result.content != NULL) {
            result.allocated = s;
            result.size = size;
        } else {
            return NULL;
        }
        *vect = result;
        if (value == 0){
            memset(vect->content, 0, s * sizeof(uint32_t) / VECTOR_SIZE);
        } else {
            const uint8_t full_byte = 0xff;
            size_t rest = size % VECTOR_SIZE;
            size_t num_bytes = size / VECTOR_SIZE * sizeof(uint32_t);
            memset(vect->content, full_byte, num_bytes);
            
            // Create the last uint32_t
            uint8_t last_byte0 = 0;
            uint8_t last_byte1 = 0;
            uint8_t last_byte2 = 0;
            uint8_t last_byte3 = 0;

            if (rest > 24){
                last_byte0 = full_byte;
                last_byte1 = full_byte;
                last_byte2 = full_byte;
                last_byte3 = create_byte(last_byte3, rest - 24);
            } else if (rest > 16) {
                last_byte0 = full_byte;
                last_byte1 = full_byte;
                last_byte2 = create_byte(last_byte3, rest - 16);
            } else if (rest > 8) {
                last_byte0 = full_byte;
                last_byte1 = create_byte(last_byte3, rest - 8);
            } else {
                last_byte0 = create_byte(last_byte3, rest);
            }
            
            uint32_t last_word = last_byte0 + (last_byte1 << 8) + (last_byte2 << 16) + (last_byte3 << 24);
            vect->content[size / VECTOR_SIZE] = last_word;
        }
    }

    return vect;
}


void bit_vector_free(bit_vector_t** pbv){
    free(pbv);
    pbv = NULL;
}
