#include <stdint.h>

#include "error.h"

int mem_create(memory_t* mem, size_t size){
    if (size == 0)
        return ERR_BAD_PARAMETER;
}

void mem_free(memory_t* mem){
    
}