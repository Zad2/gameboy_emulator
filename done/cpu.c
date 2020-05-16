/**
 * @file cpu.c
 * @author Joseph Abboud & Zad Abi Fadel
 * @brief Functions used to create, free and manipulate the cpu of the Gameboy
 * @date 2020
 *
 */

#include <stdint.h>

#include "alu.h"
#include "bus.h"
#include "cpu.h"
#include "error.h"
#include "opcode.h"
#include "cpu-storage.h"
#include "cpu-registers.h"
#include "cpu-alu.h"

// ==== see cpu.h ========================================
int cpu_init(cpu_t *cpu)
{
    if (cpu == NULL) {
        return ERR_BAD_PARAMETER;
    }

    // Set all of the cpu's elements to 0
    memset(cpu, 0, sizeof(cpu_t));

    int err = component_create(&cpu->high_ram, HIGH_RAM_SIZE);
    return err;
}

// ==== see cpu.h ========================================
int cpu_plug(cpu_t *cpu, bus_t *bus)
{
    if (cpu == NULL || bus == NULL) {
        return ERR_BAD_PARAMETER;
    }

    cpu->bus = bus;

    (*bus)[REG_IE]=&cpu->IE;
    (*bus)[REG_IF]=&cpu->IF;

    int err = bus_plug(*cpu->bus, &cpu->high_ram, HIGH_RAM_START, HIGH_RAM_END);
    return err;
}

// ==== see cpu.h ========================================
void cpu_free(cpu_t *cpu)
{
    if (cpu != NULL) {
        //bus_unplug(*cpu->bus, &cpu->high_ram);
        component_free(&cpu->high_ram);
        cpu->bus = NULL;
    }
}

/**
 * @brief Maps cc to condition
 *
 */
typedef enum { NZ, Z, NC, C, CC_COUNT } cc_t;

/**
 * @brief Extract cc code from instruction opcode and check the appropriate flag
 *
 * @param cpu the cpu that contains the flags
 * @param op the instruction's opcode
 * @return int 0 (false) if the corresponding flag is not correct, else true (any value) otw.
 */
int check_CC(cpu_t *cpu, opcode_t op)
{
    uint8_t cc = extract_cc(op);
    flags_t f = lsb8(cpu_reg_pair_get(cpu, REG_AF_CODE));

    switch(cc) {
    case NZ:
        return  get_Z(f) == 0;
        break;
    case Z:
        return get_Z(f) != 0;
        break;
    case NC:
        return get_C(f) == 0;
        break;
    case C:
        return get_C(f) != 0;
        break;
    default:
        break;
    }
    return 0;
}
/**
 * @brief Obtain next instruction to execute, then call cpu_dispatch
 *
 * @param cpu Cpu which shall execute
 * @param lu Instruction to execute
 * @return int Error code
 */
int cpu_dispatch(const instruction_t *lu, cpu_t *cpu)
{
    if (cpu == NULL || lu == NULL) {
        return ERR_BAD_PARAMETER;
    }

    // Set flags and value to 0
    cpu->alu.flags = (flags_t) 0;
    cpu->alu.value = (uint16_t) 0;

    // Execute instruction
    int err = ERR_NONE;
    if (lu->family >= LD_A_BCR && lu->family <= LD_SP_HL) {
        err = cpu_dispatch_storage(lu, cpu);
    } else if(lu->family >= ADD_A_HLR && lu->family <= SCCF) {
        err = cpu_dispatch_alu(lu, cpu);

    } else  {
        switch (lu->family) {
        case JP_N16:
            cpu->PC = cpu_read_addr_after_opcode(cpu);
            break;
        case JP_HL:
            cpu->PC = cpu_HL_get(cpu);
            break;
        case JR_E8:
            cpu->PC += lu->bytes + (int8_t) cpu_read_data_after_opcode(cpu);
            break;
        case JP_CC_N16:
            if (check_CC(cpu, lu->opcode)) {
                cpu->PC = cpu_read_addr_after_opcode(cpu);
                cpu->idle_time += lu->xtra_cycles;
            } else {
                cpu->PC += lu->bytes;
            }
            break;
        case JR_CC_E8:
            if (check_CC(cpu, lu->opcode)) {
                cpu->PC += lu->bytes + (int8_t) cpu_read_data_after_opcode(cpu);
                cpu->idle_time += lu->xtra_cycles;
            } else {
                cpu->PC += lu->bytes;
            }
            break;
        case CALL_N16:
            cpu_SP_push(cpu, cpu->PC + lu->bytes);
            cpu->PC = cpu_read_addr_after_opcode(cpu);
            break;
        case CALL_CC_N16:
            if (check_CC(cpu, lu->opcode)) {
                cpu_SP_push(cpu, cpu->PC + lu->bytes);
                cpu->PC = cpu_read_addr_after_opcode(cpu);
                cpu->idle_time += lu->xtra_cycles;
            } else {
                cpu->PC += lu->bytes;
            }
            break;
        case RST_U3:
            cpu_SP_push(cpu, cpu->PC + lu->bytes);
            cpu->PC = extract_n3(lu->opcode) << 3;
            break;
        case RET:
            cpu->PC = cpu_SP_pop(cpu);
            break;
        case RET_CC:
            if (check_CC(cpu, lu->opcode)) {
                cpu->PC = cpu_SP_pop(cpu);
                cpu->idle_time += lu->xtra_cycles;
            } else {
                cpu->PC += lu->bytes;
            }
            break;
        case EDI:
            cpu->IME = extract_ime(lu->opcode);
            break;
        case RETI:
            cpu->IME = 1;
            cpu->PC = cpu_SP_pop(cpu);
            break;
        case HALT:
            cpu->HALT = 1;
            break;
        case STOP:
            //acts like a NOP
            break;
        case NOP:
            cpu->PC += lu->bytes;
            break;
        default:
            cpu->PC += lu->bytes;
            break;
        }

        // Update idle_time
        cpu->idle_time += lu->cycles-1;
        return err;

    }

    // Update idle_time
    cpu->idle_time += lu->cycles-1;

    // Update PC
    cpu->PC += lu->bytes;

    return err;

}
interrupt_t first_interrupt(data_t IE, data_t IF)
{
    data_t interrupts = IE & IF;

    for (interrupt_t i = 0; i <= JOYPAD ; ++i) {
        if (bit_get(interrupts, i) == 1) {
            return i;
        }
    }

    return JOYPAD + 1;
}

/**
* @brief Update ALU of cpu, execute instruction, update idle_time and PC
*
* @param cpu Cpu which shall execute
* @return Error code
*/
int cpu_do_cycle(cpu_t *cpu)
{
    if (cpu == NULL) {
        return ERR_BAD_PARAMETER;
    }

    data_t prefix = cpu_read_at_idx(cpu, cpu->PC);
    if (prefix == (data_t) 0xCB) {
        data_t opcode = cpu_read_data_after_opcode(cpu);
        return cpu_dispatch(&instruction_prefixed[opcode], cpu);
    }

    if ( (cpu->IME !=0) && (cpu->IE & cpu->IF) != 0) { // ie != 0 && if != 0
        cpu->IME = 0;
        interrupt_t i = first_interrupt(cpu->IE, cpu->IF);
        if (i <= JOYPAD) {
            bit_unset(&cpu->IF, i);
            cpu_SP_push(cpu, cpu->PC);
            cpu->PC = 0x40 + (i<<3);
            cpu->idle_time += 5;
        }

    }

    // data_t opcode = cpu_read_data_after_opcode(cpu);
    return cpu_dispatch(&instruction_direct[prefix], cpu);
}

//==== see cpu.h ========================================
int cpu_cycle(cpu_t *cpu)
{

    if (cpu == NULL) {
        return ERR_BAD_PARAMETER;
    }

    if (cpu->idle_time != 0) {
        --cpu->idle_time;
        return ERR_NONE;
    }
    cpu->write_listener = (addr_t) 0;


    interrupt_t i = first_interrupt(cpu->IE, ~cpu->IF);
    // if (cpu->HALT == 1 && i <= JOYPAD) {
    //     cpu->HALT = 0;
    //     return cpu_do_cycle(cpu);
    // }
    if ((cpu->HALT == 1 && i <= JOYPAD) || cpu->HALT == 0) {
        cpu->HALT = 0;
        return cpu_do_cycle(cpu);
    }
    return ERR_NONE;
}

void cpu_request_interrupt(cpu_t* cpu, interrupt_t i)
{
    if (bit_get(cpu->IE, i) == 1) { //cpu->IME ==1 &&
        bit_set(&cpu->IF, i);
    }
}

