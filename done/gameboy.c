/**
 * @file gameboy.c
 * @author Joseph Abboud & Zad Abi Fadel
 * @brief Functions used to create, free and manipulate an instance of the Gameboy
 * @date 2020
 *
 */

#include <stdint.h>
#include <stdlib.h>

#include "bus.h"
#include "component.h"
#include "gameboy.h"
#include "cpu.h"
#include "bootrom.h"
#include "timer.h"

// ==== see gameboy.h ========================================
int gameboy_create(gameboy_t *gameboy, const char *filename)
{

    if (gameboy == NULL) {
        return ERR_BAD_PARAMETER;
    }

    // Instanciate a Gameboy
    gameboy_t gb;
    memset(&gb, 0, sizeof(gameboy_t));

    // Instanciate components
    component_t workRAM;
    memset(&workRAM, 0, sizeof(component_t));


    // memset(&gb.echoram, 0, sizeof(component_t));

    component_t registers;
    memset(&registers, 0, sizeof(component_t));

    component_t externRAM;
    memset(&externRAM, 0, sizeof(component_t));

    component_t videoRAM;
    memset(&videoRAM, 0, sizeof(component_t));

    component_t graphRAM;
    memset(&graphRAM, 0, sizeof(component_t));

    component_t useless;
    memset(&useless, 0, sizeof(component_t));

    // component_t bootROM;
    // memset(&bootROM, 0, sizeof(component_t));

    // Instanciate the Gameboy's bus
    // bus_t bus;
    // memset(&bus, 0, sizeof(bus_t));

    // Create the Gameboy's cpu
    // cpu_t cpu;
    // memset(&cpu, 0, sizeof(cpu_t));
    int err = cpu_init(&gameboy->cpu);
    if (err != ERR_NONE) {
        return err;
    }

    // Create the components
    err = component_create(&workRAM, MEM_SIZE(WORK_RAM));
    if (err != ERR_NONE) {
        return err;
    }
    ++gameboy->size_components;

    // err = component_create(&gb.echoram, MEM_SIZE(ECHO_RAM));
    // if (err != ERR_NONE) {
    //     return err;
    // }
    err = component_create(&registers, MEM_SIZE(REGISTERS));
    if (err != ERR_NONE) {
        return err;
    }
    ++gameboy->size_components;

    err = component_create(&externRAM, MEM_SIZE(EXTERN_RAM));
    if (err != ERR_NONE) {
        return err;
    }
    ++gameboy->size_components;

    err = component_create(&videoRAM, MEM_SIZE(VIDEO_RAM));
    if (err != ERR_NONE) {
        return err;
    }
    ++gameboy->size_components;

    err = component_create(&graphRAM, MEM_SIZE(GRAPH_RAM));
    if (err != ERR_NONE) {
        return err;
    }
    ++gameboy->size_components;

    err = component_create(&useless, MEM_SIZE(USELESS));
    if (err != ERR_NONE) {
        return err;
    }
    ++gameboy->size_components;

    err = component_create(&gameboy->bootrom, MEM_SIZE(BOOT_ROM));
    if (err != ERR_NONE) {
        return err;
    }

    err = timer_init(&gameboy->timer, &gameboy->cpu);
    if (err != ERR_NONE) {
        return err;
    }

    err = cartridge_init(&gameboy->cartridge, filename);
    if (err != ERR_NONE) {
        return err;
    }



    // Plug the components to the bus
    err = bus_plug(gameboy->bus, &workRAM, WORK_RAM_START, WORK_RAM_END);
    if (err != ERR_NONE) {
        return err;
    }
    component_shared(&gameboy->echoram, &workRAM); // echoRam and workRam point to the same memory space
    // err = bus_plug(bus, &gb.echoram, ECHO_RAM_START, ECHO_RAM_END);
    // if (err != ERR_NONE) {
    //     return err;
    // }
    err = bus_plug(gameboy->bus, &registers, REGISTERS_START, REGISTERS_END);
    if (err != ERR_NONE) {
        return err;
    }
    err = bus_plug(gameboy->bus, &externRAM, EXTERN_RAM_START, EXTERN_RAM_END);
    if (err != ERR_NONE) {
        return err;
    }
    err = bus_plug(gameboy->bus, &videoRAM, VIDEO_RAM_START, VIDEO_RAM_END);
    if (err != ERR_NONE) {
        return err;
    }
    err = bus_plug(gameboy->bus, &graphRAM, GRAPH_RAM_START, GRAPH_RAM_END);
    if (err != ERR_NONE) {
        return err;
    }
    err = bus_plug(gameboy->bus, &useless, USELESS_START, USELESS_END);
    if (err != ERR_NONE) {
        return err;
    }



    err = bootrom_plug(&gameboy->bootrom, gameboy->bus);
    if (err != ERR_NONE) {
        return err;
    }
    err = cartridge_plug(&gameboy->cartridge, &gameboy->bus);
    if (err != ERR_NONE) {
        return err;
    }






    // Copy the created bus to the Gameboy's bus
    // for (size_t i = 0; i < BUS_SIZE; ++i) {
    //     gameboy->bus[i] = bus[i];
    // }
    gameboy->components[WORK_RAM] = workRAM;
    gameboy->components[REGISTERS] = registers;
    gameboy->components[EXTERN_RAM] = externRAM;
    gameboy->components[VIDEO_RAM] = videoRAM;
    gameboy->components[GRAPH_RAM] = graphRAM;
    gameboy->components[USELESS] = useless;
    // gameboy->cpu = cpu;

    gameboy->boot = (bit_t) 1;

    err = cpu_plug(&gameboy->cpu, &gameboy->bus);
    if (err != ERR_NONE) {
        return err;
    }
    // *gameboy = gb;
    return ERR_NONE;
}


// ==== see gameboy.h ========================================
void gameboy_free(gameboy_t *gameboy)
{
    if (gameboy != NULL) {
        for (size_t i = 0; i < GB_NB_COMPONENTS; ++i) {
            // Unplug the bus from all components and free them
            bus_unplug(gameboy->bus, &gameboy->components[i]);
            component_free(&gameboy->components[i]);
            //&gameboy->components[i]= NULL;
        }
        // bus_unplug(gameboy->bus, &gameboy->echoram);
        bus_unplug(gameboy->bus, &gameboy->bootrom);
        //free timer??
        bus_unplug(gameboy->bus, &gameboy->cartridge);
        component_free(&gameboy->echoram);
        component_free(&gameboy->cartridge.c);
        component_free(&gameboy->bootrom);


        gameboy->cycles = 0;
        gameboy->size_components = 0;
    }
}

#ifdef BLARGG
static int blargg_bus_listener(gameboy_t* gameboy, addr_t addr)
{
    if (gameboy == NULL) {
        return ERR_BAD_PARAMETER;
    }
    if (addr == BLARGG_REG) {
        data_t data = cpu_read_at_idx(&gameboy->cpu, addr);
        printf("%c", data);
        // gameboy->cpu.write_listener = 0;
    }
    return ERR_NONE;
}
#endif

int gameboy_run_until(gameboy_t* gameboy, uint64_t cycle)
{
    if (gameboy == NULL) {
        return ERR_BAD_PARAMETER;
    }

    int err = ERR_NONE;
    while (gameboy->cycles < cycle) {
#ifdef BLARGG
        if (gameboy->cycles % VBLANK_PERIOD == 0) {
            // bit_set(&gameboy->cpu.IE, VBLANK);
            // bit_set(&gameboy->cpu.IF, VBLANK);
            // bit_set(&gameboy->cpu.IME, VBLANK);
            cpu_request_interrupt(&gameboy->cpu, VBLANK);
        }
#endif

        err = timer_cycle(&gameboy->timer);
        if (err != ERR_NONE) {
            return err;
        }

        err = cpu_cycle(&gameboy->cpu);
        if (err != ERR_NONE) {
            return err;
        }
        ++gameboy->cycles;


        err = timer_bus_listener(&gameboy->timer, gameboy->cpu.write_listener);
        if (err != ERR_NONE) {
            return err;
        }

        err = bootrom_bus_listener(gameboy, gameboy->cpu.write_listener);
        if (err != ERR_NONE) {
            return err;
        }
#ifdef BLARGG
        M_EXIT_IF_ERR(blargg_bus_listener(gameboy, gameboy->cpu.write_listener));
#endif


    }

    return err;
}

