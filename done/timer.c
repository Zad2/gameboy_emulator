/**
 * @file timer.c
 * @author Joseph Abboud & Zad Abi Fadel
 * @brief Functions used to create, free and manipulate an instance of a Gameboy's timer
 * @date 2020
 *
 */

#include <stdint.h>

#include "component.h"
#include "bit.h"
#include "cpu.h"
#include "bus.h"

#include "timer.h"

// ==== see timer.h ========================================
int timer_init(gbtimer_t* timer, cpu_t* cpu)
{
    M_REQUIRE_NON_NULL(timer);
    M_REQUIRE_NON_NULL(cpu);

    timer->cpu = cpu;

    timer->counter = (uint16_t) 0;
    return ERR_NONE;
}

/**
 * @brief Output the current state (boolean) of given timer
 *
 * @param timer The given timer
 * @return bit_t The boolean value of current_state
 */
bit_t timer_state(gbtimer_t* timer)
{
    if (timer == NULL) {
        return 0;
    }

    data_t tac = cpu_read_at_idx(timer->cpu, REG_TAC);
    bit_t x = bit_get((uint8_t) tac, 2);
    data_t div = cpu_read_at_idx(timer->cpu, REG_DIV);
    bit_t y = 0;
    enum {BIT9, BIT3, BIT5, BIT7}; // Enum created locally for the switch
    uint8_t mask2 = 3;
    switch(tac & mask2) {
    case BIT9:
        //Bit 9 is bit 1 of the 8 MSBs
        y = bit_get((uint8_t) div, 1);
        break;
    case BIT3:
        y = bit_get((uint8_t) timer->counter, 3);
        break;
    case BIT5:
        y = bit_get((uint8_t) timer->counter, 5);
        break;
    case BIT7:
        y = bit_get((uint8_t) timer->counter, 7);
        break;
    default:
        break;
    }
    return x & y;
}

/**
 * @brief Increment the given timer if a change in states occurs
 *
 * @param timer The given timer
 * @param old_state The timer's state when entering the method
 * @return int Error code
 */
int timer_incr_if_state_change(gbtimer_t* timer, bit_t old_state)
{
    M_REQUIRE_NON_NULL(timer);
    data_t tima = cpu_read_at_idx(timer->cpu, REG_TIMA);

    if (old_state && !timer_state(timer)) {
        // ++tima;

        if (tima == 0xFF) {
            //raise timer interrupt
            bit_set(&timer->cpu->IF, TIMER);

            //reload value
            tima = cpu_read_at_idx(timer->cpu, REG_TMA);
        }else{
            ++tima;
        }
    }

    return cpu_write_at_idx(timer->cpu, REG_TIMA, tima);
}

// ==== see timer.h ========================================
int timer_cycle(gbtimer_t* timer)
{
    M_REQUIRE_NON_NULL(timer);

    // Get current state of principal timer
    bit_t current_state = timer_state(timer);

    // Increment counter by 4 (A cycle is 4 clock ticks)
    timer->counter += 4;

    // copy 8 MSB from timer principal counter to DIV register
    M_EXIT_IF_ERR(cpu_write_at_idx(timer->cpu, REG_DIV, msb8(timer->counter)));
    

    return timer_incr_if_state_change(timer, current_state);
}

// ==== see timer.h ========================================
int timer_bus_listener(gbtimer_t* timer, addr_t addr)
{

    M_REQUIRE_NON_NULL(timer);

    bit_t current_state = timer_state(timer);

    switch(addr) {
    case REG_DIV:
        // Reset initial counter to 0
        timer->counter = 0;
        M_EXIT_IF_ERR(cpu_write_at_idx(timer->cpu, REG_DIV, 0));
        // current_state = timer_state(timer);
        return timer_incr_if_state_change(timer, current_state);
            fprintf(stderr, "\tin TIMER div\n");

        break;
    case REG_TAC:
        return timer_incr_if_state_change(timer, current_state);
            fprintf(stderr, "\tin TIMER tac\n");

        break;
    default :
        break;
    }

    return ERR_NONE;
}
