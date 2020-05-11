/**
 * @file alu.c
 * @brief Basic functions used for arithmetic operations
 * @date 2020
 *
 */

#include <stdio.h>
#include <stdint.h>   // for uint8_t and uint16_t types
#include <inttypes.h> // for PRIx8, etc.
#include "alu.h"
#include "bit.h"
#include "error.h"

// ==== see alu.h ========================================
flag_bit_t get_flag(flags_t flags, flag_bit_t flag)
{
    if (flag == FLAG_Z || flag == FLAG_N || flag == FLAG_H || flag == FLAG_C) {
        return flags & flag;
    }
    return 0;
    // Add errors (maybe)
}

// ==== see alu.h ========================================
void set_flag(flags_t *flags, flag_bit_t flag)
{
    if (flag == FLAG_Z || flag == FLAG_N || flag == FLAG_H || flag == FLAG_C) {
        *flags = *flags | (flags_t)flag;
    }
    // Add errors (maybe)
}

// ==== see alu.h ========================================
int alu_add8(alu_output_t *result, uint8_t x, uint8_t y, bit_t c0)
{
    if (result == NULL) {
        return ERR_BAD_PARAMETER;
    }

    uint8_t temp = (uint8_t)(lsb4(x) + lsb4(y) + c0);          // Compute the 4 LSBs of the result
    uint8_t temp1 = (uint8_t)(msb4(x) + msb4(y) + msb4(temp)); // Compute the 4 MSBs of the result
    uint16_t rslt = merge8(merge4(temp, temp1), 0);            // Transform 8-bit result to 16 bits

    result->value = rslt;
    result->flags = 0;

    if (rslt == 0) {
        set_Z(&result->flags);
    }
    if (msb4(temp) != 0) {
        set_H(&result->flags);
    }
    if (msb4(temp1) != 0) {
        set_C(&result->flags);
    }

    return ERR_NONE;
}

// ==== see alu.h ========================================
int alu_sub8(alu_output_t *result, uint8_t x, uint8_t y, bit_t b0)
{
    if (result == NULL) {
        return ERR_BAD_PARAMETER;
    }

    uint8_t temp = (uint8_t)(lsb4(x) - lsb4(y) - b0);          //Compute the 4 LSBs of the result
    uint8_t temp1 = (uint8_t)(msb4(x) - msb4(y) + msb4(temp)); // Compute the 4 MSBs of the result
    uint16_t rslt = merge8(merge4(temp, temp1), 0);            // Transform 8-bit result to 16 bits

    result->value = rslt;
    result->flags = FLAG_N;

    if (rslt == 0) {
        set_Z(&result->flags);
    }
    if (y + b0 > x) {
        set_C(&result->flags);
    }
    if (lsb4(x) < (lsb4(y) + b0)) {
        set_H(&result->flags);
    }
    return ERR_NONE;
}

// ==== see alu.h ========================================
int alu_add16_low(alu_output_t *result, uint16_t x, uint16_t y)
{
    if (result == NULL) {
        return ERR_BAD_PARAMETER;
    }

    uint8_t X = lsb8(x);
    uint8_t Y = lsb8(y);

    uint8_t temp0 = (uint8_t)(lsb4(X) + lsb4(Y));                // Compute the 4 LSBs of the result
    uint8_t temp01 = (uint8_t)(msb4(X) + msb4(Y) + msb4(temp0)); // Compute the 4 MSBs of the result

    uint16_t temp = (uint16_t)(X + Y);                           //Compute the 8 LSBs of the result
    uint16_t temp1 = (uint16_t)(msb8(x) + msb8(y) + msb8(temp)); // Compute the 8 MSBs of the result
    uint16_t rslt = merge8((uint8_t)temp, (uint8_t)temp1);

    result->value = rslt;

    if (rslt == 0) {
        set_Z(&result->flags);
    }
    if (msb4(temp0) != 0) {
        set_H(&result->flags);
    }
    if (msb4(temp01) != 0) {
        set_C(&result->flags);
    }
    return ERR_NONE;
}

// ==== see alu.h ========================================
int alu_add16_high(alu_output_t *result, uint16_t x, uint16_t y)
{
    if (result == NULL) {
        return ERR_BAD_PARAMETER;
    }

    uint8_t X = msb8(x);
    uint8_t Y = msb8(y);

    uint8_t temp0 = (uint8_t)(lsb4(X) + lsb4(Y));                // Compute the 4 LSBs of the result
    uint8_t temp01 = (uint8_t)(msb4(X) + msb4(Y) + msb4(temp0)); // Compute the 4 MSBs of the result

    uint16_t temp = (uint16_t)(lsb8(x) + lsb8(y));   //Compute the 8 LSBs of the result
    uint16_t temp1 = (uint16_t)(X + Y + msb8(temp)); // Compute the 8 MSBs of the result
    uint16_t rslt = merge8((uint8_t)temp, (uint8_t)temp1);

    result->value = rslt;

    if (rslt == 0) {
        set_Z(&result->flags);
    }
    if (msb4(temp0 + msb8(temp)) != 0) {
        set_H(&result->flags);
    }
    if (msb8(temp1) != 0) {
        set_C(&result->flags);
    }
    return ERR_NONE;
}

// ==== see alu.h ========================================
int alu_shift(alu_output_t *result, uint8_t x, rot_dir_t dir)
{
    if (result == NULL || (dir != RIGHT && dir != LEFT)) {
        return ERR_BAD_PARAMETER;
    }

    bit_t ejected = 0;
    result->flags = 0;
    if (dir == LEFT) {
        ejected = bit_get(x, SIZE_BYTE - 1);
        x = (uint8_t)(x << 1);
    } else if (dir == RIGHT) {
        ejected = bit_get(x, 0);
        x = (uint8_t)(x >> 1);
    }

    result->value = merge8(x, 0);

    if (x == 0) {
        set_Z(&result->flags);
    }
    if (ejected == 1) {
        set_C(&result->flags);
    }

    return ERR_NONE;
}

// ==== see alu.h ========================================
int alu_shiftR_A(alu_output_t *result, uint8_t x)
{
    if (result == NULL) {
        return ERR_BAD_PARAMETER;
    }

    bit_t msb = bit_get(x, SIZE_BYTE - 1);
    bit_t ejected = bit_get(x, 0);
    x = x >> 1;

    result->value = (uint16_t)((msb << (SIZE_BYTE - 1)) | x);
    result->flags = 0;

    if (x == 0) {
        set_Z(&result->flags);
    }
    if (ejected == 1) {
        set_C(&result->flags);
    }

    return ERR_NONE;
}

// ==== see alu.h ========================================
int alu_rotate(alu_output_t *result, uint8_t x, rot_dir_t dir)
{
    if (result == NULL || (dir != RIGHT && dir != LEFT)) {
        return ERR_BAD_PARAMETER;
    }

    bit_t ejected = 0;
    if (dir == LEFT) {
        ejected = bit_get(x, SIZE_BYTE - 1);
    } else if (dir == RIGHT) {
        ejected = bit_get(x, 0);
    }

    bit_rotate(&x, dir, 1);
    result->value = x;

    if (x == 0) {
        set_Z(&result->flags);
    }
    if (ejected == 1) {
        set_C(&result->flags);
    }

    return ERR_NONE;
}

// ==== see alu.h ========================================

int alu_carry_rotate(alu_output_t *result, uint8_t x, rot_dir_t dir, flags_t flags)
{
    if (result == NULL || (dir != RIGHT && dir != LEFT)) {
        return ERR_BAD_PARAMETER;
    }

    bit_t carry = 0;
    if (get_C(flags) == FLAG_C) {
        carry = 1;
    }
    result->flags = 0;
    bit_t ejected;

    if (dir == LEFT) {
        ejected = bit_get(x, SIZE_BYTE - 1);
        result->value = ((x << 1) | carry) & 0xff;
    } else if (dir == RIGHT) {
        ejected = bit_get(x, 0);
        result->value = ((x >> 1) | (carry << (SIZE_BYTE - 1))) & 0xff;
    }

    if (result->value == 0) {
        set_Z(&result->flags);
    }
    if (ejected != 0) {
        set_C(&result->flags);
    }

    return ERR_NONE;
}
