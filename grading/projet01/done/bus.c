/**
 * @file bus.c
 * @brief Functions used for the Gameboy bus
 * @date 2020
 *
 */

#include <stdint.h>
#include <stdbool.h>

#include "memory.h" // addr_t and data_t
#include "component.h"
#include "bit.h"
#include "bus.h"
#include "error.h"

// ==== see bus.h ========================================
int bus_remap(bus_t bus, component_t *c, addr_t offset)
{
    addr_t start = c->start;
    addr_t end = c->end;

    // Check if the span of the zone is larger than the component's memory,
    // Or if the component's start address is bigger than its end
    if (c->mem == NULL || end - start + offset >= c->mem->allocated) {
        //allocated?

        return ERR_ADDRESS;
    }

    // Make sure of the correctness of the component's limits
    // correcteur : remove condition end == 0
    if (start > end || end == 0) {
        return ERR_BAD_PARAMETER;
    }

    // Go through all the data of the bus and assigning the pointers in the memory
    for (int i = 0; i <= end - start; ++i) {
        bus[start + i] = &(c->mem->memory[offset + i]);
    }

    return ERR_NONE;
}

// ==== see bus.h ========================================
int bus_forced_plug(bus_t bus, component_t *c, addr_t start, addr_t end, addr_t offset)
{
    if (c == NULL) {
        return ERR_BAD_PARAMETER;
    }

    // Same error checks as bus_remap
    if (c->mem == NULL || end - start + offset >= c->mem->allocated || start > end) {
        return ERR_ADDRESS;
    }

    c->start = start;
    c->end = end;

    // Call bus_remap and get potential errors
    int err = bus_remap(bus, c, offset);

    if (err != ERR_NONE) {
        c->start = 0;
        c->end = 0;
        return err;
    }

    return ERR_NONE;
}

// ==== see bus.h ========================================
int bus_plug(bus_t bus, component_t *c, addr_t start, addr_t end)
{
    if (c == NULL) {
        return ERR_BAD_PARAMETER;
    }

    for (int i = start; i <= end; i++) {
        //revert
        if (bus[i] != NULL) {
            return ERR_ADDRESS;
        }
    }

    // After checking for errors, just simply call bus_force_plug and return its error code
    return bus_forced_plug(bus, c, start, end, 0);
}

// ==== see bus.h ========================================
int bus_unplug(bus_t bus, component_t *c)
{
    if (c == NULL) {
        return ERR_BAD_PARAMETER;
    }

    addr_t start = c->start;
    addr_t end = c->end;

    // Get throug all data_t pointers from c's start till end and unplug the pointers from the data
    for (int i = start; i <= end; ++i) {
        bus[i] = NULL;
    }

    c->start = 0;
    c->end = 0; // Maybe use component_free
    return ERR_NONE;
}

// ==== see bus.h ========================================
int bus_read(const bus_t bus, addr_t address, data_t *data)
{
    if (data == NULL) {
        return ERR_BAD_PARAMETER;
    }

    data_t dat = 0xff;
    *data = dat;

    // Get the data pointer from the bus and make the given data pointer point to it
    if (bus[address] != NULL) {
        *data = *bus[address];
    }

    return ERR_NONE;
}

// ==== see bus.h ========================================
int bus_read16(const bus_t bus, addr_t address, addr_t *data16)
{
    if (data16 == NULL) {
        return ERR_BAD_PARAMETER;
    }

    data_t dat16 = 0xff; // Initialize dat16 to the default value

    *data16 = (addr_t)dat16;
    if (bus[address] != NULL) {
        data_t ptr1 = dat16;
        data_t ptr2 = dat16;

        // Extract LSBs from bus at addr_t address
        int err = bus_read(bus, address, &ptr1);
        if (err != ERR_NONE) {
            return err;
        }

        //Extract MSBs from bus at addr_t address + 1
        err = bus_read(bus, (addr_t)(address + 1), &ptr2);
        if (err != ERR_NONE) {
            return err;
        }

        *data16 = (addr_t)merge8(ptr1, ptr2);
    }

    return ERR_NONE;
}

// ==== see bus.h ========================================
int bus_write(bus_t bus, addr_t address, data_t data)
{
    if (bus[address] == NULL) {
        return ERR_BAD_PARAMETER;
    }

    // Make the required pointer point the given data_t
    *bus[address] = data;
    return ERR_NONE;
}

// ==== see bus.h ========================================
int bus_write16(bus_t bus, addr_t address, addr_t data16)
{
    if (bus[address] == NULL) {
        return ERR_BAD_PARAMETER;
    }

    data_t data1 = lsb8(data16);
    data_t data2 = msb8(data16);

    // Same as bus_write but write on 2 consecutive addresses (1 word = 2 bytes)
    *bus[address] = data1;
    *bus[address + 1] = data2;

    return ERR_NONE;
}
