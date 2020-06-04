/**
 * @file bootrom.c
 * @author Joseph Abboud & Zad Abi Fadel
 * @brief Functions used to create, free and manipulate an instance of a Gameboy's bootROM
 * @date 2020
 *
 */

#include "bus.h"
#include "component.h"
#include "gameboy.h"

#include "bootrom.h"

// ==== see bootrom.h ========================================
int bootrom_init(component_t* c)
{
    M_REQUIRE_NON_NULL(c);
    M_REQUIRE_NON_NULL(c->mem);
    M_REQUIRE_NON_NULL(c->mem->memory);

    component_t bootROM;
    memset(&bootROM, 0, sizeof(component_t));

    M_EXIT_IF_ERR(component_create(&bootROM, MEM_SIZE(BOOT_ROM)));

    uint8_t instructions[MEM_SIZE(BOOT_ROM)] = GAMEBOY_BOOT_ROM_CONTENT;
    memcpy(&bootROM.mem->memory, instructions, sizeof(instructions));

    *c = bootROM;
    return ERR_NONE;

}

// ==== see bootrom.h ========================================
int bootrom_bus_listener(gameboy_t* gameboy, addr_t addr)
{   
    M_REQUIRE_NON_NULL(gameboy);

    if(addr == REG_BOOT_ROM_DISABLE && gameboy->boot != 0) {
        // Deactivates the bootrom
        M_EXIT_IF_ERR(bus_unplug(gameboy->bus, &gameboy->bootrom));
        // Maps the component to the corresponding part of the bus
        M_EXIT_IF_ERR(cartridge_plug(&gameboy->cartridge, gameboy->bus));
        // Set boot bit to 0 to mark end of boot
        gameboy->boot = 0;
    }
    return ERR_NONE;
}