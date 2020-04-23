#pragma once

/**
 * @file cpu.h
 * @brief CPU model for PPS-GBemul project, high level interface
 *
 * @author J.-C. Chappelier & C. Hölzl, EPFL
 * @date 2019
 */

#ifdef __cplusplus
extern "C" {
#endif

#include <stdint.h>

#include "alu.h"
#include "bus.h"

//=========================================================================
/**
 * @brief Type to represent CPU interupts
 */
typedef enum {
    VBLANK, LCD_STAT, TIMER, SERIAL, JOYPAD
} interrupt_t ;


#define REG_IF          0xFF0F
#define REG_IE          0xFFFF
#define HIGH_RAM_START   0xFF80
#define HIGH_RAM_END     0xFFFE
#define HIGH_RAM_SIZE ((HIGH_RAM_END - HIGH_RAM_START)+1)

//=========================================================================
/**
 * @brief Type to represent CPU
 */
union reg_AF {
  struct {
    uint8_t A;
    uint8_t F;
  };
  uint16_t AF;
};
union reg_BC {
  struct {
    uint8_t B;
    uint8_t C;
  };
  uint16_t BC;
};
union reg_DE {
  struct {
    uint8_t D;
    uint8_t E;
  };
  uint16_t DE;
};
union reg_HL {
  struct {
    uint8_t H;
    uint8_t L;
  };
  uint16_t HL;
};
typedef struct {
    alu_output_t alu;
    union reg_AF AF;
    union reg_BC BC;
    union reg_DE DE;
    union reg_HL HL;
    uint16_t PC;
    uint16_t SP;
} cpu_t; 


//=========================================================================
/**
 * @brief Run one CPU cycle
 * @param cpu (modified), the CPU which shall run
 * @param cycle, the cycle number to run, starting from 0
 * @return error code
 */
int cpu_cycle(cpu_t* cpu);


/**
 * @brief Plugs a bus into the cpu
 *
 * @param cpu cpu to plug into
 * @param bus bus to plug
 *
 * @return error code
 */
int cpu_plug(cpu_t* cpu, bus_t* bus);


/**
 * @brief Starts the cpu by initializing all registers at zero
 *
 * @param cpu cpu to start
 *
 * @return error code
 */
int cpu_init(cpu_t* cpu);


/**
 * @brief Frees a cpu
 *
 * @param cpu cpu to free
 */
void cpu_free(cpu_t* cpu);


/**
 * @brief Set an interruption
 */
void cpu_request_interrupt(cpu_t* cpu, interrupt_t i);


#ifdef __cplusplus
}
#endif
