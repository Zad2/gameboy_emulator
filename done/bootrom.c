#include "bus.h"
#include "component.h"
#include "gameboy.h"

#include "bootrom.h"

int bootrom_init(component_t* c)
{
    if (c == NULL || c->mem == NULL || c->mem->memory == NULL) {
        return ERR_BAD_PARAMETER;
    }

    component_t bootROM;
    memset(&bootROM, 0, sizeof(component_t));

    int err = component_create(&bootROM, MEM_SIZE(BOOT_ROM));
    if (err != ERR_NONE) {
        return err;
    }

    uint8_t instructions[MEM_SIZE(BOOT_ROM)] = GAMEBOY_BOOT_ROM_CONTENT;
    memcpy(&bootROM.mem->memory, instructions, sizeof(instructions));

    *c = bootROM;
    return ERR_NONE;

}

int bootrom_bus_listener(gameboy_t* gameboy, addr_t addr)
{
    if (gameboy == NULL) {
        return ERR_BAD_PARAMETER;
    }
    if(addr == REG_BOOT_ROM_DISABLE) {
        //fixme
        // addr_t trigger = cpu_read16_at_idx(&gameboy->cpu, addr);
        // if (trigger != NULL && gameboy->boot == 1) {
        // Deactivates the bootrom
        int err = bus_unplug(gameboy->bus, &gameboy->bootrom);
        if (err != ERR_NONE) {
            return err;
        }
        // Maps the component to the corresponding part of the bus
        //cartridge_plug(&gameboy->cartridge, gameboy->bus);
        // Set boot bit to 0 to mark end of boot
        gameboy->boot = 0;

        // }
    }
    return ERR_NONE;
}