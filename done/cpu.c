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
    if (cpu == NULL)
    {
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
    if (cpu == NULL || bus == NULL)
    {
        return ERR_BAD_PARAMETER;
    }

    cpu->bus = bus;
    return ERR_NONE;
}

// ==== see cpu.h ========================================
void cpu_free(cpu_t *cpu)
{
    if (cpu != NULL)
    {
        cpu->bus = NULL;
    }
}

// ==== see cpu.h ========================================
int cpu_cycle(cpu_t *cpu)
{

    if (cpu == NULL)
    {
        return ERR_BAD_PARAMETER;
    }

    if (cpu->idle_time != 0)
    {
        --cpu->idle_time;
    }

    return ERR_NONE;
}
