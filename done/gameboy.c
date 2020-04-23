#include <stdint.h>
#include <stdlib.h>

#include "bus.h"
#include "component.h"
#include "gameboy.h"
#include "cpu.h"

int gameboy_create(gameboy_t* gameboy, const char* filename)
{

    if (gameboy == NULL) {
        return ERR_BAD_PARAMETER;
    }

    gameboy_t gb;
    memset(&gb, 0, sizeof(gameboy_t));

    component_t workRAM;
    memset(&workRAM, 0, sizeof(component_t));
    component_t echoRAM;
    memset(&echoRAM, 0, sizeof(component_t));

    bus_t bus ;
    memset(&bus, 0, sizeof(bus_t));


    int err = component_create(&workRAM, MEM_SIZE(WORK_RAM));
    if (err != ERR_NONE) {
        return err;
    }
    err = component_create(&echoRAM, MEM_SIZE(ECHO_RAM));
    if (err != ERR_NONE) {
        return err;
    }

    err = bus_plug(bus, &workRAM, WORK_RAM_START, WORK_RAM_END);
    if (err != ERR_NONE) {
        return err;
    }
    component_shared(&echoRAM, &workRAM);
    err = bus_plug(bus, &echoRAM, ECHO_RAM_START, ECHO_RAM_END);
    if (err != ERR_NONE) {
        return err;
    }


    for (size_t i = 0; i< BUS_SIZE; ++i) {
        gb.bus[i] = bus[i];
    }
    gb.components[WORK_RAM] = workRAM;
    *gameboy = gb;

    return ERR_NONE;
}

void gameboy_free(gameboy_t* gameboy)
{
    if (gameboy != NULL) {
        for (size_t i = 0; i < GB_NB_COMPONENTS; ++i) {
            bus_unplug(gameboy->bus, &gameboy->components[i]);
            component_free(&gameboy->components[i]);
            //&gameboy->components[i]= NULL;
        }
    }

}