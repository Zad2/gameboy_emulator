#include <stdint.h>
#include <stdio.h>

#include "component.h"
#include "bus.h"

#include "cartridge.h"

int cartridge_init_from_file(component_t* c, const char* filename)
{
    M_REQUIRE_NON_NULL(c);
    M_REQUIRE_NON_NULL(filename);
    M_REQUIRE_NON_NULL(c->mem);
    M_REQUIRE_NON_NULL(c->mem->memory);

    FILE* input = NULL;
    input = fopen(filename, "rb");

    if (input == NULL) {
        return ERR_IO;
    } else {
        if(fread(c->mem->memory, sizeof(uint8_t), BANK_ROM_SIZE, input) != BANK_ROM_SIZE) {
            fclose(input);
            return ERR_IO;
        }
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
    M_REQUIRE_NON_NULL(cartridge);
    M_REQUIRE_NON_NULL(filename);

    memset(cartridge, 0, sizeof(cartridge_t));

    M_EXIT_IF_ERR(component_create(&cartridge->c, BANK_ROM_SIZE));

    return cartridge_init_from_file(&cartridge->c, filename);
}

int cartridge_plug(cartridge_t* ct, bus_t bus)
{
    M_REQUIRE_NON_NULL(ct);
    M_REQUIRE_NON_NULL(bus);

    // Plugs the cartridge to the bus
    return bus_forced_plug(bus, &ct->c,BANK_ROM0_START, BANK_ROM1_END, 0);
}

void cartridge_free(cartridge_t* ct)
{
    if (ct != NULL) {
        component_free(&ct->c);
    }
}
