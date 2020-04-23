#include <stdint.h>
#include <stdlib.h>

#include "memory.h"
#include "error.h"

int mem_create(memory_t* mem, size_t size)
{
    if (size <= 0 || mem == NULL)
        return ERR_BAD_PARAMETER;

    memory_t m = {0, 0, NULL};
    m.memory = calloc(size, sizeof(data_t));
    if (m.memory != NULL) {
        m.size = size;
        m.allocated = size; //fixme
    } else {
        return ERR_MEM;
    }
    *mem = m;

    return ERR_NONE;
}

void mem_free(memory_t* mem)
{
    if ((mem != NULL ) && (mem->memory != NULL)) {
        free(mem->memory);
        mem->memory = NULL;
        mem->size = 0;
        mem->allocated = 0;
    }
}