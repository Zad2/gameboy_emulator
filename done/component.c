#include <stdint.h>
#include <stdbool.h>

#include "memory.h"
#include "error.h"
#include "component.h"

int component_create(component_t* c, size_t mem_size) 
{
    if (c == NULL || mem_size < 0 || mem_size > c->mem.size) 
    {
        return ERR_BAD_PARAMETER;
    }

    c->mem.size = mem_size;
    component_free(c);
    return ERR_NONE;
}


void component_free(component_t* c)
{
    c->start = 0;
    c->end = 0;
}