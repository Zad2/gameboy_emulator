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
int cpu_dispatch(cpu_t *cpu, const instruction_t *lu)
{
    if (cpu == NULL || lu == NULL) {
        return ERR_BAD_PARAMETER;
    }

    // Set flags and value to 0
    cpu->alu.flags = 0;
    cpu->alu.value = 0;

    // Execute instruction
    int err = cpu_dispatch_storage(lu, cpu);

    // Update idle_time
    cpu->idle_time += lu->cycles;

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
    data_t opcode = cpu_read_at_idx(cpu, cpu->PC);
    return cpu_dispatch(cpu, &instruction_direct[opcode]);
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

    return cpu_do_cycle(cpu);
}
