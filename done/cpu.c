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

// ==== see cpu.h ========================================
int cpu_init(cpu_t *cpu)
{
    if (cpu == NULL) {
        return ERR_BAD_PARAMETER;
    }

    // Set all of the cpu's elements to 0
    memset(cpu, 0, sizeof(cpu_t));
    cpu->idle_time = 0; // Unnecessary?
    return ERR_NONE;
}

// ==== see cpu.h ========================================
int cpu_plug(cpu_t *cpu, bus_t *bus)
{
    if (cpu == NULL || bus == NULL) {
        return ERR_BAD_PARAMETER;
    }

    cpu->bus = bus;
    return ERR_NONE;
}

// ==== see cpu.h ========================================
void cpu_free(cpu_t *cpu)
{
    if (cpu != NULL) {
        cpu->bus = NULL;
    }
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
    } else if(lu->family >= ADD_A_HLR && lu->family <= CHG_U3_R8) {
        err = cpu_dispatch_alu(lu, cpu);
    }

    // Update idle_time
    cpu->idle_time += lu->cycles-1;

    // Update PC
    cpu->PC += lu->bytes;

    return err;

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

// ==== see cpu.h ========================================
int cpu_cycle(cpu_t *cpu)
{

    if (cpu == NULL) {
        return ERR_BAD_PARAMETER;
    }

    if (cpu->idle_time != 0) {
        --cpu->idle_time;
    }

    return ERR_NONE;
}
