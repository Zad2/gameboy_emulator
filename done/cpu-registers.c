/**
 * @file cpu-registers.c
 * @author Joseph Abboud & Zad Abi Fadel
 * @brief Functions used to access the Gameboy's cpu (read/write registers (single or pairs) )
 * @date 2020
 *
 */

#include <stdint.h> // uint8_t

#include "bit.h"
#include "cpu.h" // cpu_t
#include "cpu-registers.h"

// ==== see cpu-registers.h ========================================
uint16_t cpu_reg_pair_get(const cpu_t *cpu, reg_pair_kind reg)
{
    if (cpu == NULL)
    {
        return 0;
    }

    // Check the register's code and return the equivalent register pair from the cpu
    switch (reg)
    {
    case REG_BC_CODE:
        return cpu->BC;
        break;
    case REG_DE_CODE:
        return cpu->DE;
        break;
    case REG_HL_CODE:
        return cpu->HL;
        break;
    case REG_AF_CODE:
        return cpu->AF;
        break;
    default: // Default return value is 0
        return 0;
        break;
    }
}

// ==== see cpu-registers.h ========================================
uint8_t cpu_reg_get(const cpu_t *cpu, reg_kind reg)
{
    if (cpu == NULL)
    {
        return 0;
    }

    // Check the register's code and return the equivalent register from the cpu, except for F
    switch (reg)
    {
    case REG_B_CODE:
        return cpu->B;
        break;
    case REG_C_CODE:
        return cpu->C;
        break;
    case REG_D_CODE:
        return cpu->D;
        break;
    case REG_E_CODE:
        return cpu->E;
        break;
    case REG_H_CODE:
        return cpu->H;
        break;
    case REG_L_CODE:
        return cpu->L;
        break;
    case REG_A_CODE:
        return cpu->A;
        break;
    default: // Default return value is 0 for register F and others
        return 0;
        break;
    }
}

// ==== see cpu-registers.h ========================================
void cpu_reg_pair_set(cpu_t *cpu, reg_pair_kind reg, uint16_t value)
{
    if (cpu == NULL)
    {
        return;
    }
    // Check the register's code and set the equivalent register pair in the cpu
    switch (reg)
    {
    case REG_BC_CODE:
        cpu->BC = value;
        break;
    case REG_DE_CODE:
        cpu->DE = value;
        break;
    case REG_HL_CODE:
        cpu->HL = value;
        break;
    case REG_AF_CODE:
        // This is a particular case, the 4 LSBs in F should be reset to 0
        cpu->AF = value;
        cpu->F = merge4(0, msb4(cpu->F)); // Set F's 4 LSBs to 0
        break;
    default:
        break;
    }
}

// ==== see cpu-registers.h ========================================
void cpu_reg_set(cpu_t *cpu, reg_kind reg, uint8_t value)
{
    if (cpu == NULL)
    {
        return;
    }

    // Check the register's code and set the equivalent register in the cpu, except for F
    switch (reg)
    {
    case REG_B_CODE:
        cpu->B = value;
        break;
    case REG_C_CODE:
        cpu->C = value;
        break;
    case REG_D_CODE:
        cpu->D = value;
        break;
    case REG_E_CODE:
        cpu->E = value;
        break;
    case REG_H_CODE:
        cpu->H = value;
        break;
    case REG_L_CODE:
        cpu->L = value;
        break;
    case REG_A_CODE:
        cpu->A = value;
        break;
    default:
        break;
    }
}
