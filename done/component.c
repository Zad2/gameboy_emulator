/**
 * @file component.c
 * @author Joseph Abboud & Zad Abi Fadel
 * @brief Functions used to create, free and share components for the Gameboy
 * @date 2020
 *
 */

#include <stdint.h>
#include <stdbool.h>
#include <stdlib.h>

#include "memory.h"
#include "error.h"
#include "component.h"

// ==== see component.h ========================================
int component_create(component_t *c, size_t mem_size)
{
    if (c == NULL) {
        return ERR_BAD_PARAMETER;
    }

    component_t component = {NULL, 0, 0};
    component.start = 0;
    component.end = 0;

    // If the given memory_size is zero, the component's memory doesn't exist
    if (mem_size == 0) {
        component.mem = NULL;
        *c = component;
        return ERR_NONE;
    }

    // Allocate memory space to the component
    component.mem = calloc(mem_size, sizeof(memory_t));
    if (component.mem == NULL) {
        return ERR_MEM;
    }

    // Call mem_create and get potential errors
    int err = mem_create(component.mem, mem_size);
    if (err == ERR_NONE) {
        *c = component;
    } else {
        free(component.mem);
    }

    return err;
}

// ==== see component.h ========================================
void component_free(component_t *c)
{
    if ((c != NULL) && (c->mem != NULL)) {

        // Call mem_free before freeing c->mem directly from the computer's memory
        mem_free(c->mem);
        free(c->mem);

        c->mem = NULL;
        c->start = 0;
        c->end = 0;
    }
}

// ==== see component.h ========================================
int component_shared(component_t *c, component_t *c_old)
{
    if (c == NULL || c_old == NULL
        || c->mem == NULL || c_old->mem == NULL
        || c->mem->memory == NULL || c_old->mem->memory == NULL) {
        return ERR_BAD_PARAMETER;
    }

    // Reset the component's start and end and plug c'memory to c_old's memory
    c->start = 0;
    c->end = 0;
    c->mem = c_old->mem;

    return ERR_NONE;
}
