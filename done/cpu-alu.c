/**
 * @file cpu-alu.c
 * @brief Game Boy CPU simulation, ALU part asked to students
 *
 * @date 2019
 */

#include "error.h"
#include "bit.h"
#include "alu.h"
#include "cpu-alu.h"
#include "cpu-storage.h" // cpu_read_at_HL
#include "cpu-registers.h" // cpu_HL_get

// external library provided later to lower workload
extern int cpu_dispatch_alu_ext(const instruction_t* lu, cpu_t* cpu);

#include <assert.h>
#include <stdbool.h>

// ======================================================================
/**
 * @brief Returns flag bit value based on source preferences
 *
 * @param src source to select
 * @param cpu_f flags from cpu
 * @param alu_f flags from alu
 *
 * @return resulting flag bit value
 */
static bool flags_src_value(flag_src_t src, flag_bit_t cpu_f, flag_bit_t alu_f)
{
    switch (src) {
    case CLEAR:
        return false;

    case SET:
        return true;

    case ALU:
        return alu_f;

    case CPU:
        return cpu_f;

    default:
        return false;
    }

    return false;
}

// ======================================================================
/**
* @brief Checks if x is a valid flag source
*/
#define IS_VALID_FLAG_SRC(x) \
    (x == CLEAR || x == SET || x == ALU || x == CPU)
#define CHECK_FLAG_SRC(x) \
    M_REQUIRE(IS_VALID_FLAG_SRC(x), ERR_BAD_PARAMETER, "Parameter %d for " #x " is not valid", x)

// ==== see cpu-alu.h ========================================
int cpu_combine_alu_flags(cpu_t* cpu,
                          flag_src_t Z, flag_src_t N, flag_src_t H, flag_src_t C)
{
    M_REQUIRE_NON_NULL(cpu);
    CHECK_FLAG_SRC(Z);
    CHECK_FLAG_SRC(N);
    CHECK_FLAG_SRC(H);
    CHECK_FLAG_SRC(C);

    flags_t res_f = 0;

    if (flags_src_value(Z, get_Z(cpu->F), get_Z(cpu->alu.flags)))
        set_Z(&res_f);

    if (flags_src_value(N, get_N(cpu->F), get_N(cpu->alu.flags)))
        set_N(&res_f);

    if (flags_src_value(H, get_H(cpu->F), get_H(cpu->alu.flags)))
        set_H(&res_f);

    if (flags_src_value(C, get_C(cpu->F), get_C(cpu->alu.flags)))
        set_C(&res_f);

    cpu->F = res_f;

    return ERR_NONE;
}

// ======================================================================
/**
* @brief Tool function usefull for CHG_U3_R8:
*        Do a SET or a RESET(=unset) of data bit,
*          according to SR and N3 bits of instruction's opcode
*/
static void do_set_or_res(const instruction_t* lu, data_t* data)
{
    assert(lu   != NULL);
    assert(data != NULL);

#pragma GCC diagnostic push
#pragma GCC diagnostic ignored "-Wconversion"

    if (extract_sr_bit(lu->opcode)) {
        *data |=   (data_t) (1 << extract_n3(lu->opcode)) ;
    } else {
        *data &= ~((data_t) (1 << extract_n3(lu->opcode)));
    }

#pragma GCC diagnostic pop
}

// ==== see cpu-alu.h ========================================
int cpu_dispatch_alu(const instruction_t* lu, cpu_t* cpu)
{
    M_REQUIRE_NON_NULL(cpu);

    switch (lu->family) {

    // ADD
    case ADD_A_HLR: {
        alu_add8(&cpu->alu, cpu_reg_get(cpu, REG_A_CODE),
                 cpu_read_at_HL(cpu),extract_carry(cpu, lu->opcode));

        cpu_reg_set_from_alu8(cpu, REG_A_CODE);
        cpu_combine_alu_flags(cpu, ADD_FLAGS_SRC);

        //gives undefined reference
        //do_cpu_arithm(cpu, alu_add8, cpu_read_at_HL(cpu), ADD_FLAGS_SRC);
    } break;

    case ADD_A_N8: {
        cpu_reg_set(cpu, REG_A_CODE,
                    cpu_reg_get(cpu, REG_A_CODE) + cpu_read_data_after_opcode(cpu) +
                    extract_carry(cpu, lu->opcode));

        return cpu_combine_alu_flags(cpu, ADD_FLAGS_SRC);
    } break;

    case ADD_A_R8: {
        cpu_reg_set(cpu, REG_A_CODE,
                    cpu_reg_get(cpu, REG_A_CODE) + cpu_reg_get(cpu, extract_reg(lu->opcode,0)) +
                    extract_carry(cpu, lu->opcode));

        return cpu_combine_alu_flags(cpu, ADD_FLAGS_SRC);
    } break;

    case INC_HLR: {
        cpu_write_at_HL(cpu, cpu_read_at_HL(cpu) + 1);

        return cpu_combine_alu_flags(cpu, INC_FLAGS_SRC);
    } break;

    case INC_R8: {
        cpu_reg_set(cpu,extract_n3(lu->opcode), cpu_reg_get(cpu, extract_n3(lu->opcode) + 1));

        return cpu_combine_alu_flags(cpu, INC_FLAGS_SRC);
    } break;

    case ADD_HL_R16SP: {
        cpu_HL_set(cpu, cpu_HL_get(cpu) + cpu_reg_pair_get(cpu, extract_reg_pair(lu->opcode)));

        //return cpu_combine_alu_flags(cpu, );
    } break;

    case INC_R16SP: {
    } break;


    // COMPARISONS
    case CP_A_R8: {
    } break;


    // BIT MOVE (rotate, shift)
    case SLA_R8: {
    } break;

    case ROT_R8: {
    } break;


    // BIT TESTS (and set)
    case BIT_U3_R8: {
    } break;

    case CHG_U3_R8: {
    } break;

    // ---------------------------------------------------------
    // All the others are handled elsewhere by provided library
    default:
        // uncomment this line if you have the cs212gbcpuext library
        // M_EXIT_IF_ERR(cpu_dispatch_alu_ext(lu, cpu));
        break;
    } // switch

    return ERR_NONE;
}
