#include <stdint.h>
#include <stdbool.h>
#include <stdlib.h>

#include "memory.h"
#include "error.h"
#include "component.h"

int component_create(component_t* c, size_t mem_size)
{
    if (c == NULL) {
        return ERR_BAD_PARAMETER;
    }
    component_t component = {NULL, 0, 0};
    component.start =0;
    component.end=0;
    if (mem_size == 0) {
        component.mem = NULL;
        *c = component;
        return ERR_NONE;
    }
    component.mem = calloc(mem_size, sizeof(memory_t));

    int err = mem_create(component.mem, mem_size);
    if (err == ERR_NONE) {
        *c = component;
    }

    return err;
}


void component_free(component_t* c)
{
    if ((c != NULL ) && (c->mem != NULL)) {
        mem_free(c->mem);
        free(c->mem);

        c->mem = NULL;
        c->start = 0;
        c->end = 0;
    }
}

int component_shared(component_t* c, component_t* c_old)
{
    if ( c == NULL || c_old == NULL) {
        return ERR_BAD_PARAMETER;
    }
    c->start = 0;
    c->end = 0;

    c->mem = c_old->mem;
    return ERR_NONE;
}
