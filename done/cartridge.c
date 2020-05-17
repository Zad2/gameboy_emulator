#include <stdint.h>
#include <stdio.h>

#include "component.h"
#include "bus.h"

#include "cartridge.h"

int cartridge_init_from_file(component_t* c, const char* filename)
{
    if (c == NULL || filename == NULL || c->mem == NULL || c->mem->memory == NULL) {
        return ERR_BAD_PARAMETER;
    }
    FILE* input = NULL;
    input = fopen(filename, "rb");

    if (input == NULL) {
        return ERR_IO;
    } else {
        // == 1??
        // data_t bootrom_memory[BANK_ROM_SIZE];
        if(fread(c->mem->memory, sizeof(uint8_t), BANK_ROM_SIZE, input) != BANK_ROM_SIZE) {
            fclose(input);
            return ERR_IO;
        }

        // for (int i = 0; i < BANK_ROM_SIZE; ++i){
        //     c->mem->memory[i] = bootrom_memory[i];

        // }
        // for (int i = 0; i < 100; ++i){
        //     printf("%zx",c->mem->memory[i*2] );
        //     printf("%zx ",c->mem->memory[i*2+1] );

        // }
        c->mem->size = BANK_ROM_SIZE;
        if (c->mem->memory[CARTRIDGE_TYPE_ADDR] != 0) {
            fclose(input);
            return ERR_NOT_IMPLEMENTED;
        }
        fclose(input);

    }
    return ERR_NONE;
}

int cartridge_init(cartridge_t* cartridge, const char* filename)
{
    if (cartridge == NULL || filename == NULL) {
        return ERR_BAD_PARAMETER;
    }

    memset(cartridge, 0, sizeof(cartridge_t));

    int err = component_create(&cartridge->c, BANK_ROM_SIZE);
    if (err != ERR_NONE) {
        return err;
    }

    return cartridge_init_from_file(&cartridge->c, filename);
}

int cartridge_plug(cartridge_t* ct, bus_t bus)
{
    if (ct == NULL || bus == NULL) {
        return ERR_BAD_PARAMETER;
    }
    // Plugs the cartridge to the bus
    //fixme
    int err = bus_forced_plug(bus, &ct->c,BANK_ROM0_START, BANK_ROM1_END, 0);
    return err;
}

void cartridge_free(cartridge_t* ct)
{
    if (ct != NULL) {
        component_free(&ct->c);
    }
}
