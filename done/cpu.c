#include <stdint.h>

#include "alu.h"
#include "bus.h"
#include "cpu.h"
#include "error.h"

int cpu_init(cpu_t* cpu)
{
    if (cpu == NULL) {
        return ERR_BAD_PARAMETER;
    }

    memset(cpu, 0, sizeof(cpu_t));
    cpu->idle_time = 0;
    return ERR_NONE;

}

int cpu_plug(cpu_t* cpu, bus_t* bus)
{
    if (cpu == NULL || bus == NULL) {
        return ERR_BAD_PARAMETER;
    }

    cpu->bus = bus;
    return ERR_NONE;
}

void cpu_free(cpu_t* cpu)
{
    if (cpu != NULL) {
        cpu->bus = NULL;
    }
}

int cpu_cycle(cpu_t* cpu)
{

    if (cpu == NULL) {
        return ERR_BAD_PARAMETER;
    }

    if(cpu->idle_time != 0) {
        --cpu->idle_time;
    }
    //cpu->PC;
    return ERR_NONE;
}
