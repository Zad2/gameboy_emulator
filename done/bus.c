#include <stdint.h>
#include <stdbool.h>

#include "memory.h" // addr_t and data_t
#include "component.h"
#include "bit.h"
#include "bus.h"
#include "error.h"

//------Auxiliary functions------
/**
 * @brief Checks if an address is inside given bounds
 * 
 * @param address The address to check
 * @param min The minimum possible address (included)
 * @param max The maximum possible addres (included)
 * @return int error code
 */
int check_bounds(addr_t address, addr_t min, addr_t max)
{
    return (address < min || address > max) ? ERR_BAD_PARAMETER : ERR_NONE;
}
//-------------------------------

int bus_remap(bus_t bus, component_t *c, addr_t offset)
{
    addr_t start = c->start;
    addr_t end = c->end;
    memory_t mem = c->mem;

    // Check if the span of the zone is larger than the component's memory,
    // Or if the component's start address is bigger than its end
    if (end - start + offset >= mem.size)
    {
        return ERR_ADDRESS;
    }
    if (start > end || end == 0)
    {
        return ERR_BAD_PARAMETER;
    }

    for (int i = 0; i <= end - start; ++i) //should it be addr_t i
    {
        bus[start + i] = &(c->mem.memory[offset + i]);
    }

    return ERR_NONE;
}

int bus_forced_plug(bus_t bus, component_t *c, addr_t start, addr_t end, addr_t offset)
{
    if (c == NULL)
    {
        return ERR_BAD_PARAMETER;
    }

    c->start = start;
    c->end = end;
    int err = bus_remap(bus, c, offset);

    if (err != ERR_NONE)
    {
        c->start = 0;
        c->end = 0;
        return err;
    }

    return ERR_NONE;
}

int bus_plug(bus_t bus, component_t *c, addr_t start, addr_t end)
{
    if (c == NULL)
    {
        return ERR_BAD_PARAMETER;
    }

    for (int i = start; i <= end; i++) //revert
    {
        if (bus[i] != NULL)
        {
            return ERR_ADDRESS;
        }
    }

    return bus_forced_plug(bus, c, start, end, 0);
}

int bus_unplug(bus_t bus, component_t *c)
{
    if (c == NULL)
    {
        return ERR_BAD_PARAMETER;
    }

    addr_t start = c->start;
    addr_t end = c->end;

    for (int i = start; i <= end; ++i)
    {
        bus[i] = NULL;
    }

    c->start = 0;
    c->end = 0; // See if we have to use component_free
    return ERR_NONE;
}

int bus_read(const bus_t bus, addr_t address, data_t *data)
{
    int err = ERR_NONE;
    if ((err = check_bounds(address, 0, BUS_SIZE)) != ERR_NONE)
    {
        //return err;
    }

    if (data == NULL)
    {
        return ERR_BAD_PARAMETER;
    }

    data_t dat = 0xff;
    *data = dat;
    //printf("&bus[0] = %zx\n", *bus[0]);

    //printf("pre data = %u\n", *data);
    //printf("&bus[address] = %zx\n", &bus[address]);
    //printf("bus[address] = %zx\n", bus[address]);
    //printf("*bus[address] = %zx\n", *bus[address]);

    if (bus[address] != NULL)
    {
        *data = *bus[address];
    }

    //printf("post data = %u\n", *data);

    printf("--------------------------------------------------\n");
    return ERR_NONE;
}

int bus_read16(const bus_t bus, addr_t address, addr_t *data16)
{
    int err = ERR_NONE;
    if ((err = check_bounds(address, 0, BUS_SIZE)) != ERR_NONE)
    {
        return err;
    }

    if (data16 == NULL)
    {
        return ERR_BAD_PARAMETER;
    }
    addr_t dat16 = 0xff;

    if (bus[address] != NULL)
    {
        data_t *ptr1 = &dat16;
        data_t *ptr2 = &dat16;

        // Extracting LSBs from bus at addr_t adress
        err = bus_read(bus, address, ptr1);
        if (err != ERR_NONE)
        {
            return err;
        }

        //Extracting MSBs from bus at addr_t address + 1
        err = bus_read(bus, address + 1, ptr2);
        if (err != ERR_NONE)
        {
            return err;
        }

        dat16 = merge8(*ptr1, *ptr2);
    }

    data16 = &dat16;

    return ERR_NONE;
}

int bus_write(bus_t bus, addr_t address, data_t data)
{
    int err = ERR_NONE;
    if ((err = check_bounds(address, 0, BUS_SIZE)) != ERR_NONE)
    {
        return err;
    }

    if (address == 0)
    {
        return ERR_BAD_PARAMETER;
    }

    bus[address] = &data;
    return ERR_NONE;
}

int bus_write16(bus_t bus, addr_t address, addr_t data16)
{
    int err = ERR_NONE;
    if ((err = check_bounds(address, 0, BUS_SIZE)) != ERR_NONE)
    {
        return err;
    }

    if (address == 0)
    {
        return ERR_BAD_PARAMETER;
    }

    data_t data1 = lsb8(data16);
    data_t data2 = msb8(data16);

    bus[address] = &data1;
    bus[address + 1] = &data2;

    return ERR_NONE;
}
