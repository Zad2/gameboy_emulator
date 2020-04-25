/**
 * @file memory.c
 * @author Joseph Abboud & Zad Abi Fadel
 * @brief Functions used to create, free and manipulate the memory of the Gameboy
 * @date 2020
 *
 */

#include <stdint.h>
#include <stdlib.h>

#include "memory.h"
#include "error.h"

// ==== see memory.h ========================================
int mem_create(memory_t *mem, size_t size)
{
    if (size <= 0 || mem == NULL) {
        return ERR_BAD_PARAMETER;
    }

    // Initialize a memory_t instance and allocate necessary
    // space for it in the computer memory
    memory_t m = {0, 0, NULL};
    m.memory = calloc(size, sizeof(data_t));

    if (m.memory != NULL) {
        m.size = size;
        m.allocated = size; //fixme
    } else {
        return ERR_MEM;
    }

    // Point the given memory to the created one
    *mem = m;

    return ERR_NONE;
}

// ==== see memory.h ========================================
void mem_free(memory_t *mem)
{
    if ((mem != NULL) && (mem->memory != NULL)) {
        // Free the memory from the computer's memory and setting all of its
        // elements to default state
        free(mem->memory);
        mem->memory = NULL;
        mem->size = 0;
        mem->allocated = 0;
    }
}