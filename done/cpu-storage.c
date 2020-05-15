/**
 * @file cpu-registers.c
 * @brief Game Boy CPU simulation, register part
 *
 * @date 2019
 */

#include "error.h"
#include "cpu-storage.h"   // cpu_read_at_HL
#include "cpu-registers.h" // cpu_BC_get
#include "gameboy.h"       // REGISTER_START
#include "util.h"
#include <inttypes.h> // PRIX8
#include <stdio.h>    // fprintf

// ==== see cpu-storage.h ========================================
data_t cpu_read_at_idx(const cpu_t *cpu, addr_t addr)
{
    if (cpu == NULL || cpu->bus == NULL) {
        return NULL;
    }
    data_t data = (data_t)0;

    // Call bus_read from bus.c and reads from the cpu's bus at address addr
    bus_read(*cpu->bus, addr, &data);
    return data;

}

// ==== see cpu-storage.h ========================================
addr_t cpu_read16_at_idx(const cpu_t *cpu, addr_t addr)
{
    if (cpu == NULL || cpu->bus == NULL) {
        return NULL;
    }
    addr_t a = (addr_t)0;

    // Call bus_read16 from bus.c and read a word from the cpu's bus at addresses addr and addr+1
    bus_read16(*cpu->bus, addr, &a);
    return a;
}

// ==== see cpu-storage.h ========================================
int cpu_write_at_idx(cpu_t *cpu, addr_t addr, data_t data)
{
    if (cpu == NULL || cpu->bus == NULL) {
        return ERR_BAD_PARAMETER;
    }

    // Call bus_write from bus.c and write to the cpu's bus at address addr,
    // while getting potential errors
    int err = bus_write(*cpu->bus, addr, data);
    return err;
}

// ==== see cpu-storage.h ========================================
int cpu_write16_at_idx(cpu_t *cpu, addr_t addr, addr_t data16)
{
    if (cpu == NULL || cpu->bus == NULL) {
        return ERR_BAD_PARAMETER;
    }

    // Call bus_write16 from bus.c and write to the cpu's bus at addresses addr and addr+1,
    // while getting potential errors
    int err = bus_write16(*cpu->bus, addr, data16);
    return err;
}

// ==== see cpu-storage.h ========================================
int cpu_SP_push(cpu_t *cpu, addr_t data16)
{
    if (cpu == NULL) {
        return ERR_BAD_PARAMETER;
    }

    // Update the stack pointer value and write data16 to the new address
    cpu->SP = (uint16_t)(cpu->SP - WORD_SIZE);
    int err = cpu_write16_at_idx(cpu, cpu->SP, data16);
    return err;
}

// ==== see cpu-storage.h ========================================
addr_t cpu_SP_pop(cpu_t *cpu)
{
    // Read the word at address SP, then increment SP by a word
    addr_t a = cpu_read16_at_idx(cpu, cpu->SP);
    cpu->SP = (uint16_t)(cpu->SP + WORD_SIZE);
    return a;
}

// ==== see cpu-storage.h ========================================
int cpu_dispatch_storage(const instruction_t *lu, cpu_t *cpu)
{
    M_REQUIRE_NON_NULL(cpu);

    switch (lu->family) {
    case LD_A_BCR:
        cpu_reg_set(cpu, REG_A_CODE, cpu_read_at_idx(cpu, cpu_BC_get(cpu)));
        break;

    case LD_A_CR:
        cpu_reg_set(cpu, REG_A_CODE, cpu_read_at_idx(cpu, (addr_t) (REGISTERS_START + cpu_reg_get(cpu, REG_C_CODE))));
        break;

    case LD_A_DER:
        cpu_reg_set(cpu, REG_A_CODE, cpu_read_at_idx(cpu, cpu_DE_get(cpu)));
        break;

    case LD_A_HLRU:
        cpu_reg_set(cpu, REG_A_CODE, cpu_read_at_HL(cpu));
        cpu->HL += extract_HL_increment(lu->opcode);
        break;

    case LD_A_N16R:
        cpu_reg_set(cpu, REG_A_CODE, cpu_read_at_idx(cpu, cpu_read_addr_after_opcode(cpu)));
        break;

    case LD_A_N8R:
        cpu_reg_set(cpu, REG_A_CODE, cpu_read_at_idx(cpu, (addr_t) (REGISTERS_START + cpu_read_data_after_opcode(cpu))));
        break;

    case LD_BCR_A:
        cpu_write_at_idx(cpu, cpu_BC_get(cpu), cpu_reg_get(cpu, REG_A_CODE));
        break;

    case LD_CR_A:
        cpu_write_at_idx(cpu, (addr_t) (REGISTERS_START + cpu_reg_get(cpu, REG_C_CODE)), cpu_reg_get(cpu, REG_A_CODE));
        break;

    case LD_DER_A:
        cpu_write_at_idx(cpu, cpu_DE_get(cpu), cpu_reg_get(cpu, REG_A_CODE));
        break;

    case LD_HLRU_A:
        cpu_write_at_HL(cpu, cpu_reg_get(cpu, REG_A_CODE));
        cpu->HL += extract_HL_increment(lu->opcode);
        break;

    case LD_HLR_N8:
        cpu_write_at_HL(cpu, cpu_read_data_after_opcode(cpu));
        break;

    case LD_HLR_R8:
        cpu_write_at_HL(cpu, cpu_reg_get(cpu, extract_reg(lu->opcode, 0)));
        break;

    case LD_N16R_A:
        cpu_write_at_idx(cpu, cpu_read_addr_after_opcode(cpu), cpu_reg_get(cpu, REG_A_CODE));
        break;

    case LD_N16R_SP:
        cpu_write16_at_idx(cpu, cpu_read_addr_after_opcode(cpu), cpu_reg_pair_SP_get(cpu, REG_AF_CODE));
        break;

    case LD_N8R_A:
        cpu_write_at_idx(cpu, (addr_t) (REGISTERS_START + cpu_read_data_after_opcode(cpu)), cpu_reg_get(cpu, REG_A_CODE));
        break;

    case LD_R16SP_N16:
        cpu_reg_pair_SP_set(cpu, extract_reg_pair(lu->opcode), cpu_read_addr_after_opcode(cpu));
        break;

    case LD_R8_HLR:
        cpu_reg_set(cpu, extract_n3(lu->opcode), cpu_read_at_HL(cpu));
        break;

    case LD_R8_N8:
        cpu_reg_set(cpu, extract_n3(lu->opcode), cpu_read_data_after_opcode(cpu));
        break;

    case LD_R8_R8: {
        reg_kind r = extract_n3(lu->opcode);
        reg_kind s = extract_reg(lu->opcode, 0);
        (r != s) ? cpu_reg_set(cpu, r, cpu_reg_get(cpu, s)) : cpu_reg_set(cpu, r, r) /*Do nothing*/;
    }
    break;

    case LD_SP_HL:
        cpu_reg_pair_SP_set(cpu, REG_AF_CODE, cpu_HL_get(cpu));
        break;

    case POP_R16:
        cpu_reg_pair_set(cpu, extract_reg_pair(lu->opcode), cpu_read16_at_idx(cpu, cpu_reg_pair_SP_get(cpu, REG_AF_CODE)));
        cpu_reg_pair_SP_set(cpu, REG_AF_CODE, cpu_reg_pair_SP_get(cpu, REG_AF_CODE) + WORD_SIZE);
        break;

    case PUSH_R16:
        cpu_reg_pair_SP_set(cpu, REG_AF_CODE, cpu_reg_pair_SP_get(cpu, REG_AF_CODE) - WORD_SIZE);
        cpu_write16_at_idx(cpu, cpu_reg_pair_SP_get(cpu, REG_AF_CODE), cpu_reg_pair_get(cpu, extract_reg_pair(lu->opcode)));
        break;

    default:
        fprintf(stderr, "Unknown STORAGE instruction, Code: 0x%" PRIX8 "\n", cpu_read_at_idx(cpu, cpu->PC));
        return ERR_INSTR;
        break;
    } // switch

    return ERR_NONE;
}
