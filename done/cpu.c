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
    M_REQUIRE_NON_NULL(cpu);

    // Set all of the cpu's elements to 0
    memset(cpu, 0, sizeof(cpu_t));

    return component_create(&cpu->high_ram, HIGH_RAM_SIZE);
}

// ==== see cpu.h ========================================
int cpu_plug(cpu_t *cpu, bus_t *bus)
{
    M_REQUIRE_NON_NULL(cpu);
    M_REQUIRE_NON_NULL(bus);

    cpu->bus = bus;

    (*bus)[REG_IE]=&cpu->IE;
    (*bus)[REG_IF]=&cpu->IF;

    return bus_plug(*cpu->bus, &cpu->high_ram, HIGH_RAM_START, HIGH_RAM_END);
}

// ==== see cpu.h ========================================
void cpu_free(cpu_t *cpu)
{
    if (cpu != NULL) {
        // bus_unplug(*cpu->bus, &cpu->high_ram);
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
    if (cpu != NULL) {
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
    }
    return 0;
}

static int cpu_dispatch(const instruction_t* lu, cpu_t* cpu)
{
    M_REQUIRE_NON_NULL(lu);
    M_REQUIRE_NON_NULL(cpu);

    // Set flags and value to 0
    cpu->alu.flags = (flags_t) 0;
    cpu->alu.value = (uint16_t) 0;
    if (lu->opcode != 0){
        int x =0;
    }
    switch (lu->family) {

    // ALU
    case AND_A_N8: {int x = 0;}
    case ADD_A_HLR:
    case ADD_A_N8:
    case ADD_A_R8:
    case INC_HLR:
    case INC_R8:
    case ADD_HL_R16SP:
    case INC_R16SP:
    case SUB_A_HLR:
    case SUB_A_N8:
    case SUB_A_R8:
    case DEC_HLR:
    case DEC_R8:
    case DEC_R16SP:
    case AND_A_HLR:
    case AND_A_R8:
    case OR_A_HLR:
    case OR_A_N8:
    case OR_A_R8:
    case XOR_A_HLR:
    case XOR_A_N8:
    case XOR_A_R8:
    case CPL:
    case CP_A_HLR:
    case CP_A_N8:
    case CP_A_R8:
    case SLA_HLR:
    case SLA_R8:
    case SRA_HLR:
    case SRA_R8:
    case SRL_HLR:
    case SRL_R8:
    case ROTCA:
    case ROTA:
    case ROTC_HLR:
    case ROT_HLR:
    case ROTC_R8:
    case ROT_R8:
    case SWAP_HLR:
    case SWAP_R8:
    case BIT_U3_HLR:
    case BIT_U3_R8:
    case CHG_U3_HLR:
    case CHG_U3_R8:
    case LD_HLSP_S8:
    case DAA:
    case SCCF:
        M_EXIT_IF_ERR(cpu_dispatch_alu(lu, cpu));
        break;

    // STORAGE
    case LD_A_BCR:
    case LD_A_CR:
    case LD_A_DER:
    case LD_A_HLRU:
    case LD_A_N16R:
    case LD_A_N8R:
    case LD_BCR_A:
    case LD_CR_A:
    case LD_DER_A:
    case LD_HLRU_A:
    case LD_HLR_N8:
    case LD_HLR_R8:
    case LD_N16R_A:
    case LD_N16R_SP:
    case LD_N8R_A:
    case LD_R16SP_N16:
    case LD_R8_HLR:
    case LD_R8_N8:
    case LD_R8_R8:
    case LD_SP_HL:
    case POP_R16:
    case PUSH_R16:
        M_EXIT_IF_ERR(cpu_dispatch_storage(lu, cpu));
        break;


    // JUMP
    case JP_CC_N16:
    if (check_CC(cpu, lu->opcode)) {
                cpu->PC = cpu_read_addr_after_opcode(cpu);
                cpu->idle_time += lu->xtra_cycles;
            } else {
                cpu->PC += lu->bytes;
            }
        break;

    case JP_HL:
        cpu->PC = cpu_HL_get(cpu);
        break;

    case JP_N16:
        cpu->PC = cpu_read_addr_after_opcode(cpu);
        break;

    case JR_CC_E8:
     if (check_CC(cpu, lu->opcode)) {
                cpu->PC += lu->bytes + (int8_t) cpu_read_data_after_opcode(cpu);
                cpu->idle_time += lu->xtra_cycles;
                // fprintf(stderr, "In JR_CC_E8: cpu->PC = %d\n", cpu->PC);
            } else {
                cpu->PC += lu->bytes;
            }
        break;

    case JR_E8:
        cpu->PC += lu->bytes + (int8_t) cpu_read_data_after_opcode(cpu);
        break;


    // CALLS
    case CALL_CC_N16:
        if (check_CC(cpu, lu->opcode)) {
            M_EXIT_IF_ERR(cpu_SP_push(cpu, cpu->PC + lu->bytes));
            cpu->PC = cpu_read_addr_after_opcode(cpu);
            cpu->idle_time += lu->xtra_cycles;
        } else {
            cpu->PC += lu->bytes;
        }
        break;

    case CALL_N16:
     M_EXIT_IF_ERR(cpu_SP_push(cpu, cpu->PC + lu->bytes));
            cpu->PC = cpu_read_addr_after_opcode(cpu);
        break;


    // RETURN (from call)
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

    case RST_U3:
    // fprintf(stderr, "opcode = %d\t", lu->opcode);
            // fprintf(stderr, "old PC = %d \t", cpu->PC);
            M_EXIT_IF_ERR(cpu_SP_push(cpu, cpu->PC + lu->bytes));
            // fprintf(stderr, "in RST_U3: cpu->PC = %d \t lu->bytes = %d \t n3 = %d \t", cpu->PC , lu->bytes,extract_n3(lu->opcode) );
            cpu->PC = extract_n3(lu->opcode) << 3;
            // fprintf(stderr, "new PC = %d \n", cpu->PC);
            // data_t dat = cpu_read_at_idx(cpu, extract_n3(lu->opcode) << 3 );
            // fprintf(stderr, "data at new PC = %d\n", dat);
            //new data is 195 (= JP n16) => jump to 50220 => NOPS
            // fprintf(stderr, "data at new PC = %d\n", cpu_read16_at_idx(cpu, cpu->PC+2));

        break;


    // INTERRUPT & MISC.
    case EDI:
        cpu->IME = extract_ime(lu->opcode);
        cpu->PC += lu->bytes;
        // fprintf(stderr, "in EDI\n");
        break;

    case RETI:
        cpu->IME = 1;
        cpu->PC = cpu_SP_pop(cpu);
        break;

    case HALT:
        cpu->HALT = 1;
        cpu->PC += lu->bytes;
        // fprintf(stderr, "in HALT\n");
        break;

    case STOP:
    case NOP:
        // ne rien faire
        cpu->PC += lu->bytes;
        break;

    default: {
        fprintf(stderr, "Unknown instruction, Code: 0x%" PRIX8 "\n", cpu_read_at_idx(cpu, cpu->PC));
        return ERR_INSTR;
    } break;

    } // switch
    // Update idle_time
    cpu->idle_time += lu->cycles-1;

    return ERR_NONE;
}

/**
 * @brief Outputs the least significant index for which both arguments' respective bits are 1
 *
 * @param IE The interrupt enable
 * @param IF The interrupt flags
 * @return interrupt_t The index of the least significant common 1 bit
 */
interrupt_t first_interrupt(data_t IE, data_t IF)
{
    data_t interrupts = IE & IF;

    for (interrupt_t i = VBLANK; i <= JOYPAD ; ++i) {
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
    M_REQUIRE_NON_NULL(cpu);
    M_REQUIRE_NON_NULL(cpu->bus);


    if ( (cpu->IME !=0)) {
        interrupt_t i = first_interrupt(cpu->IE, cpu->IF);
        if (i <= JOYPAD) {
            cpu->IME = 0;
            data_t data = cpu_read_at_idx(cpu, REG_IF);
            bit_unset(&data, i);
            M_EXIT_IF_ERR(cpu_write_at_idx(cpu, REG_IF, data));
            M_EXIT_IF_ERR(cpu_SP_push(cpu, cpu->PC));//+lu->bytes?
            cpu->PC = 0x40 + (i<<3);
            cpu->idle_time += 5;
            return ERR_NONE;
            
        }
        // return ERR_NONE;
    } 

    data_t prefix = cpu_read_at_idx(cpu, cpu->PC);
    if (prefix == (data_t) 0xCB) {
        data_t opcode = cpu_read_data_after_opcode(cpu);
        return cpu_dispatch(&instruction_prefixed[opcode], cpu);
    }

    return cpu_dispatch(&instruction_direct[prefix], cpu);
}

//==== see cpu.h ========================================
int cpu_cycle(cpu_t *cpu)
{
    M_REQUIRE_NON_NULL(cpu);
    M_REQUIRE_NON_NULL(cpu->bus);

    cpu->write_listener = (addr_t) 0;
    if (cpu->idle_time != 0) {
        --cpu->idle_time;
        return ERR_NONE;
    }
    


    interrupt_t i = first_interrupt(cpu->IE, cpu->IF);

    if ((cpu->HALT == 1 && i <= JOYPAD) ) {
        cpu->HALT = 0;
        return cpu_do_cycle(cpu);
    }else if( cpu->HALT == 0){
        return cpu_do_cycle(cpu);
    }
    return ERR_NONE;
}

// ==== see cpu.h ========================================
void cpu_request_interrupt(cpu_t* cpu, interrupt_t i)
{
    M_REQUIRE_NON_NULL(cpu);

    if (i>=VBLANK && i <= JOYPAD ){
        data_t data = cpu_read_at_idx(cpu, REG_IF);
        bit_set(&data, i);
        M_EXIT_IF_ERR(cpu_write_at_idx(cpu, REG_IF, data));

    }
}

