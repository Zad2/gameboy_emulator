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
        return (data_t) 0;
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
        return (data_t) 0;
    }
    addr_t a = (addr_t)0;

    // Call bus_read16 from bus.c and read a word from the cpu's bus at addresses addr and addr+1
    bus_read16(*cpu->bus, addr, &a);
    return a;
}

// ==== see cpu-storage.h ========================================
int cpu_write_at_idx(cpu_t *cpu, addr_t addr, data_t data)
{
    M_REQUIRE_NON_NULL(cpu);
    M_REQUIRE_NON_NULL(cpu->bus);

    if(addr == 80 || addr == 80){
        int x = 0;
    }
    // Call bus_write from bus.c and write to the cpu's bus at address addr,
    // while getting potential errors
    M_EXIT_IF_ERR(bus_write(*cpu->bus, addr, data));

    

    cpu->write_listener = addr;
    return ERR_NONE;
}

// ==== see cpu-storage.h ========================================
int cpu_write16_at_idx(cpu_t *cpu, addr_t addr, addr_t data16)
{
    M_REQUIRE_NON_NULL(cpu);
    M_REQUIRE_NON_NULL(cpu->bus);

    if(addr == 80 || addr == 80){
        int x = 0;
    }
    // Call bus_write16 from bus.c and write to the cpu's bus at addresses addr and addr+1,
    // while getting potential errors
    M_EXIT_IF_ERR(bus_write16(*cpu->bus, addr, data16));



    cpu->write_listener = addr;
    return ERR_NONE;
}

// ==== see cpu-storage.h ========================================
int cpu_SP_push(cpu_t *cpu, addr_t data16)
{
    M_REQUIRE_NON_NULL(cpu);

    // Update the stack pointer value and write data16 to the new address
    cpu->SP = (uint16_t)(cpu->SP - WORD_SIZE);
    return cpu_write16_at_idx(cpu, cpu->SP, data16);
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
        M_EXIT_IF_ERR(cpu_write_at_idx(cpu, cpu_BC_get(cpu), cpu_reg_get(cpu, REG_A_CODE)));
        break;

    case LD_CR_A:
        M_EXIT_IF_ERR(cpu_write_at_idx(cpu, (addr_t) (REGISTERS_START + cpu_reg_get(cpu, REG_C_CODE)), cpu_reg_get(cpu, REG_A_CODE)));
        break;

    case LD_DER_A:
        M_EXIT_IF_ERR(cpu_write_at_idx(cpu, cpu_DE_get(cpu), cpu_reg_get(cpu, REG_A_CODE)));
        break;

    case LD_HLRU_A:
        M_EXIT_IF_ERR(cpu_write_at_HL(cpu, cpu_reg_get(cpu, REG_A_CODE)));
        cpu->HL += extract_HL_increment(lu->opcode);
        break;

    case LD_HLR_N8:
        M_EXIT_IF_ERR(cpu_write_at_HL(cpu, cpu_read_data_after_opcode(cpu)));
        break;

    case LD_HLR_R8:
        M_EXIT_IF_ERR(cpu_write_at_HL(cpu, cpu_reg_get(cpu, extract_reg(lu->opcode, 0))));
        break;

    case LD_N16R_A:
        M_EXIT_IF_ERR(cpu_write_at_idx(cpu, cpu_read_addr_after_opcode(cpu), cpu_reg_get(cpu, REG_A_CODE)));
        break;

    case LD_N16R_SP:
        M_EXIT_IF_ERR(cpu_write16_at_idx(cpu, cpu_read_addr_after_opcode(cpu), cpu_reg_pair_SP_get(cpu, REG_AF_CODE)));
        break;

    case LD_N8R_A:
        M_EXIT_IF_ERR(cpu_write_at_idx(cpu, (addr_t) (REGISTERS_START + cpu_read_data_after_opcode(cpu)), cpu_reg_get(cpu, REG_A_CODE)));
        break;

    case LD_R16SP_N16:{
        reg_pair_kind pair = extract_reg_pair(lu->opcode);
        addr_t add = cpu_read_addr_after_opcode(cpu);
        cpu_reg_pair_SP_set(cpu, pair, add);
        // fprintf(stderr, "in LD_R16SP_N16\n");
    }
        break;

    case LD_R8_HLR:
        cpu_reg_set(cpu, extract_reg(lu->opcode, 3), cpu_read_at_HL(cpu));
        break;

    case LD_R8_N8:
        if (extract_reg(lu->opcode, 3) == REG_C_CODE && cpu_read_data_after_opcode(cpu) == 0){
            int x = 0;
        }
        if (extract_reg(lu->opcode, 3) == REG_C_CODE && cpu_read_data_after_opcode(cpu) == 16){
            int x = 0;
        }

        cpu_reg_set(cpu, extract_reg(lu->opcode, 3), cpu_read_data_after_opcode(cpu));
        break;

    case LD_R8_R8: {
        reg_kind r = extract_reg(lu->opcode, 3);
        reg_kind s = extract_reg(lu->opcode, 0);
        if (r!=s){
            cpu_reg_set(cpu, r, cpu_reg_get(cpu, s));
        }else{
            M_EXIT_ERR(ERR_INSTR, "Used LD_R8_R8 with both registers equal\n\t reg_kind 1 = %zu\treg_kind 2 = %zu", r, s);
        }
    }
    break;

    case LD_SP_HL:
        cpu_reg_pair_SP_set(cpu, REG_AF_CODE, cpu_HL_get(cpu));
        break;

    case POP_R16:
        cpu_reg_pair_set(cpu, extract_reg_pair(lu->opcode), cpu_read16_at_idx(cpu, cpu_reg_pair_SP_get(cpu, REG_AF_CODE)));
        cpu_reg_pair_SP_set(cpu, REG_AF_CODE, cpu_reg_pair_SP_get(cpu, REG_AF_CODE) + WORD_SIZE);
        // fprintf(stderr, "In pop\n");
        break;

    case PUSH_R16:
        cpu_reg_pair_SP_set(cpu, REG_AF_CODE, cpu_reg_pair_SP_get(cpu, REG_AF_CODE) - WORD_SIZE);
        M_EXIT_IF_ERR(cpu_write16_at_idx(cpu, cpu_reg_pair_SP_get(cpu, REG_AF_CODE), cpu_reg_pair_get(cpu, extract_reg_pair(lu->opcode))));
        // fprintf(stderr, "In push\n");
        break;

    default:
        fprintf(stderr, "Unknown STORAGE instruction, Code: 0x%" PRIX8 "\n", cpu_read_at_idx(cpu, cpu->PC));
        return ERR_INSTR;
        break;
    } // switch

    // Update PC
    cpu->PC += lu->bytes;

    return ERR_NONE;
}
