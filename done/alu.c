#include <stdio.h>
#include <stdint.h>   // for uint8_t and uint16_t types
#include <inttypes.h> // for PRIx8, etc.
#include "alu.h"
#include "bit.h"
#include "error.h"

flag_bit_t get_flag(flags_t flags, flag_bit_t flag)
{
    if (flag == FLAG_Z || flag == FLAG_N || flag == FLAG_H || flag == FLAG_C) {
        return flags & flag;
    }
    return 0;
    // Add errors (maybe)
}

void set_flag(flags_t *flags, flag_bit_t flag)
{
    if (flag == FLAG_Z || flag == FLAG_N || flag == FLAG_H || flag == FLAG_C) {
        *flags = *flags | flag;
    }
    // Add errors (maybe)
}

int alu_add8(alu_output_t *result, uint8_t x, uint8_t y, bit_t c0)
{
    if (result == NULL) {
        return ERR_BAD_PARAMETER;
    }

    uint8_t temp = lsb4(x) + lsb4(y) + c0;          // Compute the 4 LSBs of the result
    uint8_t temp1 = msb4(x) + msb4(y) + msb4(temp); // Compute the 4 MSBs of the result
    uint16_t rslt = merge8(merge4(temp, temp1), 0); // Transforming 8-bit result to 16 bits

    result->value = rslt;
    result->flags = 0;

    if (rslt == 0) {
        set_flag(&(*result).flags, FLAG_Z);
    }
    if (msb4(temp) != 0) {
        set_flag(&(*result).flags, FLAG_H);
    }
    if (msb4(temp1) != 0) {
        set_flag(&(*result).flags, FLAG_C);
    }


    return ERR_NONE;
}

int alu_sub8(alu_output_t *result, uint8_t x, uint8_t y, bit_t b0)
{
    if (result == NULL) {
        return ERR_BAD_PARAMETER;
    }

    uint8_t temp = lsb4(x) - lsb4(y) - b0;          //Compute the 4 LSBs of the result
    uint8_t temp1 = msb4(x) - msb4(y) + msb4(temp); // Compute the 4 MSBs of the result
    uint16_t rslt = merge8(merge4(temp, temp1), 0); // Transforming 8-bit result to 16 bits

    result->value = rslt;
    result->flags = FLAG_N;

    if (rslt == 0) {
        set_flag(&(*result).flags, FLAG_Z);
    }
    if (y + b0 > x) {
        set_flag(&(*result).flags, FLAG_C);
    }
    if (lsb4(x) < (lsb4(y) + b0)) {
        set_flag(&(*result).flags, FLAG_H);
    }
    return ERR_NONE;
}

int alu_add16_low(alu_output_t *result, uint16_t x, uint16_t y)
{
    if (result == NULL) {
        return ERR_BAD_PARAMETER;
    }

    uint8_t X = lsb8(x);
    uint8_t Y = lsb8(y);

    uint8_t temp0 = lsb4(X) + lsb4(Y);          // Compute the 4 LSBs of the result
    uint8_t temp01 = msb4(X) + msb4(Y) + msb4(temp0); // Compute the 4 MSBs of the result

    uint16_t temp = X + Y; //Compute the 8 LSBs of the result
    uint16_t temp1 = msb8(x) + msb8(y) + msb8(temp); // Compute the 8 MSBs of the result
    uint16_t rslt = merge8(temp, temp1);

    result->value = rslt;

    if (rslt == 0) {
        set_flag(&(*result).flags, FLAG_Z);
    }
    if (msb4(temp0) != 0) {
        set_flag(&(*result).flags, FLAG_H);
    }
    if (msb4(temp01) != 0) {
        set_flag(&(*result).flags, FLAG_C);
    }
    return ERR_NONE;
}

int alu_add16_high(alu_output_t *result, uint16_t x, uint16_t y)
{
    if (result == NULL) {
        return ERR_BAD_PARAMETER;
    }

    uint8_t X = msb8(x);
    uint8_t Y = msb8(y);

    uint8_t temp0 = lsb4(X) + lsb4(Y);          // Compute the 4 LSBs of the result
    uint8_t temp01 = msb4(X) + msb4(Y) + msb4(temp0); // Compute the 4 MSBs of the result

    uint16_t temp = lsb8(x) + lsb8(y);               //Compute the 8 LSBs of the result
    uint16_t temp1 = X + Y + msb8(temp); // Compute the 8 MSBs of the result
    uint16_t rslt = merge8(temp, temp1);

    result->value = rslt;

    if (rslt == 0) {
        set_flag(&(*result).flags, FLAG_Z);
    }
    if (msb4(temp0) != 0) {
        set_flag(&(*result).flags, FLAG_H);
    }
    if (msb4(temp01) != 0) {
        set_flag(&(*result).flags, FLAG_C);
    }
    return ERR_NONE;
}

int alu_shift(alu_output_t *result, uint8_t x, rot_dir_t dir)
{
    if (result == NULL || (dir != RIGHT && dir != LEFT)) {
        return ERR_BAD_PARAMETER;
    }

    bit_t ejected = 0;
    result->flags = 0;
    if (dir == LEFT) {
        ejected = bit_get(x, SIZE_BYTE - 1);
        x = x << 1;
    } else if (dir == RIGHT) {
        ejected = bit_get(x, 0);
        x = x >> 1;
    }

    result->value = merge8(x, 0);

    if (x == 0) {
        set_flag(&(*result).flags, FLAG_Z);
    }
    if (ejected == 1) {
        set_flag(&(*result).flags, FLAG_C);
    }

    return ERR_NONE;
}

int alu_shiftR_A(alu_output_t *result, uint8_t x)
{
    if (result == NULL) {
        return ERR_BAD_PARAMETER;
    }

    bit_t msb = bit_get(x, SIZE_BYTE - 1);
    bit_t ejected = bit_get(x, 0);
    x = x >> 1;

    result->value = (msb << (SIZE_BYTE - 1)) | x;
    result->flags = 0;

    if (x == 0) {
        set_flag(&(*result).flags, FLAG_Z);
    }
    if (ejected == 1) {
        set_flag(&(*result).flags, FLAG_C);
    }

    return ERR_NONE;
}

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
        set_flag(&(*result).flags, FLAG_Z);
    }
    if (ejected == 1) {
        set_flag(&(*result).flags, FLAG_C);
    }

    return ERR_NONE;
}

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
        result->value =((x << 1) | carry) & 0xff;
    } else if(dir == RIGHT) {
        ejected = bit_get(x, 0);
        result->value = ((x >> 1) | (carry << (SIZE_BYTE - 1))) & 0xff;
    }

    if ((*result).value == 0) {
        set_flag(&(*result).flags, FLAG_Z);
    }
    if (ejected != 0) {
        set_flag(&(*result).flags, FLAG_C);
    }

    return ERR_NONE;
}
