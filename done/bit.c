/**
 * @file bit.c
 * @author Joseph Abboud & Zad Abi Fadel
 * @brief Basic functions used for bit operations and manipualtions
 * @date 2020
 *
 */

#include <stdio.h>
#include <stdint.h>   // for uint8_t and uint16_t types
#include <inttypes.h> // for PRIx8, etc.
#include "bit.h"

// ======================================================================
/**
 * @brief a type to represent 1 single bit.
 */
typedef uint8_t bit_t;

// ======================================================================
#define size(T) (sizeof T / sizeof T[0])

// ==== see bit.h ========================================
uint8_t lsb4(uint8_t value)
{
    return value & 0x0f;
}

// ==== see bit.h ========================================
uint8_t msb4(uint8_t value)
{
    return (value & 0xf0) >> 4;
}

// ==== see bit.h ========================================
uint8_t lsb8(uint16_t value)
{
    return (uint8_t)(value & 0x00ff);
}

// ==== see bit.h ========================================
uint8_t msb8(uint16_t value)
{
    return (uint8_t)((value & 0xff00) >> 8);
}

// ==== see bit.h ========================================
uint16_t merge8(uint8_t v1, uint8_t v2)
{
    return (uint16_t)(v1 | v2 << 8);
}

// ==== see bit.h ========================================
uint8_t merge4(uint8_t v1, uint8_t v2)
{
    return (uint8_t)(lsb4(v1) | (lsb4(v2) << 4));
}

// ==== see bit.h ========================================
bit_t bit_get(uint8_t value, int index)
{
    int k = CLAMP07(index);
    return (bit_t)((value & (1 << k)) >> k);
}

// ==== see bit.h ========================================
void bit_set(uint8_t *value, int index)
{
    if (value != NULL) {
        int k = CLAMP07(index);
        *value = (uint8_t)((1 << k) | *value);
    }
}

// ==== see bit.h ========================================
void bit_unset(uint8_t *value, int index)
{
    if (value != NULL) {
        int k = CLAMP07(index);
        *value = (uint8_t)(~(1 << k) & *value);
    }
}

// ==== see bit.h ========================================
void bit_rotate(uint8_t *value, rot_dir_t dir, int d)
{
    if (value != NULL) {
        int k = CLAMP07(d);

        uint8_t temp = 0;
        int shift_complement = SIZE_BYTE - k;

        if (dir == LEFT) {
            temp = (uint8_t)(*value >> shift_complement);
            *value = (uint8_t)((*value << k) | temp);
        } else if (dir == RIGHT) {
            temp = (uint8_t)(*value << shift_complement);
            *value = (uint8_t)((*value >> k) | temp);
        }
    }
}

// ==== see bit.h ========================================
void bit_edit(uint8_t *value, int index, uint8_t v)
{
    if (value != NULL) {
        // Check if we want to set (v = 1) or unset (v = 0) the indexed bit
        if (v == 0) {
            bit_unset(value, index);
        } else if (v == 1) {
            bit_set(value, index);
        }
    }
}
