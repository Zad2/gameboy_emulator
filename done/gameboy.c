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

    M_REQUIRE_NON_NULL(gameboy);

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

    component_t echoRAM;
    memset(&echoRAM, 0, sizeof(component_t));

    M_EXIT_IF_ERR(cpu_init(&gameboy->cpu));


    // Create the components
    M_EXIT_IF_ERR(component_create(&workRAM, MEM_SIZE(WORK_RAM)));
    ++gameboy->nb_components;

    M_EXIT_IF_ERR(component_create(&registers, MEM_SIZE(REGISTERS)));
    ++gameboy->nb_components;
    M_EXIT_IF_ERR(component_create(&externRAM, MEM_SIZE(EXTERN_RAM)));
    ++gameboy->nb_components;
    M_EXIT_IF_ERR(component_create(&videoRAM, MEM_SIZE(VIDEO_RAM)));
    ++gameboy->nb_components;
    M_EXIT_IF_ERR(component_create(&graphRAM, MEM_SIZE(GRAPH_RAM)));
    ++gameboy->nb_components;
    M_EXIT_IF_ERR(component_create(&useless, MEM_SIZE(USELESS)));
    ++gameboy->nb_components;
    M_EXIT_IF_ERR(component_create(&echoRAM, MEM_SIZE(ECHO_RAM)));

    M_EXIT_IF_ERR(component_create(&gameboy->bootrom, MEM_SIZE(BOOT_ROM)));
    M_EXIT_IF_ERR(timer_init(&gameboy->timer, &gameboy->cpu));
    M_EXIT_IF_ERR(cartridge_init(&gameboy->cartridge, filename));

    // Plug the components to the bus
    M_EXIT_IF_ERR(bus_plug(gameboy->bus, &workRAM, WORK_RAM_START, WORK_RAM_END));
    M_EXIT_IF_ERR(component_shared(&echoRAM, &workRAM)); // echoRam and workRam point to the same memory space
    M_EXIT_IF_ERR(bus_plug(gameboy->bus, &registers, REGISTERS_START, REGISTERS_END));
    M_EXIT_IF_ERR(bus_plug(gameboy->bus, &externRAM, EXTERN_RAM_START, EXTERN_RAM_END));
    M_EXIT_IF_ERR(bus_plug(gameboy->bus, &videoRAM, VIDEO_RAM_START, VIDEO_RAM_END));
    M_EXIT_IF_ERR(bus_plug(gameboy->bus, &graphRAM, GRAPH_RAM_START, GRAPH_RAM_END));
    M_EXIT_IF_ERR(bus_plug(gameboy->bus, &useless, USELESS_START, USELESS_END));
    M_EXIT_IF_ERR(bootrom_plug(&gameboy->bootrom, gameboy->bus));
    M_EXIT_IF_ERR(cartridge_plug(&gameboy->cartridge, gameboy->bus));


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

    gameboy->boot = (bit_t) 1;

    M_EXIT_IF_ERR(cpu_plug(&gameboy->cpu, &gameboy->bus));
    M_EXIT_IF_ERR(joypad_init_and_plug(&gameboy->pad, &gameboy->cpu));
    M_EXIT_IF_ERR(lcdc_init(gameboy));
    M_EXIT_IF_ERR(lcdc_plug(&gameboy->screen, gameboy->bus));


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
        }
        bus_unplug(gameboy->bus, &gameboy->bootrom);
        bus_unplug(gameboy->bus, &gameboy->cartridge.c);
        //component_free(&gameboy->echoram);
        component_free(&gameboy->cartridge.c);
        component_free(&gameboy->bootrom);
        lcdc_free(&gameboy->screen);
        cpu_free(&gameboy->cpu);
        
        

        gameboy->cycles = 0;
        gameboy->nb_components = 0;
        gameboy->boot = (bit_t) 0;
    }
}

#ifdef BLARGG
static int blargg_bus_listener(gameboy_t* gameboy, addr_t addr)
{
    M_REQUIRE_NON_NULL(gameboy);

    if (addr == BLARGG_REG) {
        data_t data = cpu_read_at_idx(&gameboy->cpu, addr);
        printf("%c", data);
    }
    return ERR_NONE;
}
#endif

void bus_dump(bus_t* bus){

    
FILE* file = fopen("dump_bus.txt", "w");

        for(int i = 0; i<BUS_SIZE; ++i){
            fprintf(file, "%u\t", bus[i]);
        }
    fclose(file);

    return;
}

// ==== see gameboy.h ========================================
int gameboy_run_until(gameboy_t* gameboy, uint64_t cycle)
{
    M_REQUIRE_NON_NULL(gameboy);

    while (gameboy->cycles < cycle) {
#ifdef BLARGG
        if (gameboy->cycles % 17556 == 0) {
            cpu_request_interrupt(&gameboy->cpu, VBLANK);
        }
#endif

        if (gameboy->cpu.idle_time == 0){
            int x = 0;
        }

        M_EXIT_IF_ERR(timer_cycle(&gameboy->timer));
        M_EXIT_IF_ERR(cpu_cycle(&gameboy->cpu));
        // printf("cycle %d\n", gameboy->cycles);
        // M_EXIT_IF_ERR(lcdc_cycle(&gameboy->screen, gameboy->cycles));

        if (gameboy->cpu.write_listener!= 0){
            M_EXIT_IF_ERR(timer_bus_listener(&gameboy->timer, gameboy->cpu.write_listener));
            M_EXIT_IF_ERR(bootrom_bus_listener(gameboy, gameboy->cpu.write_listener));
        }
        // M_EXIT_IF_ERR(joypad_bus_listener(&gameboy->pad, gameboy->cpu.write_listener));
        // M_EXIT_IF_ERR(lcdc_bus_listener(&gameboy->screen, gameboy->cpu.write_listener));
#ifdef BLARGG
        M_EXIT_IF_ERR(blargg_bus_listener(gameboy, gameboy->cpu.write_listener));
#endif
        ++gameboy->cycles;
    }
    bus_dump(&gameboy->bus);

    return ERR_NONE;
}

