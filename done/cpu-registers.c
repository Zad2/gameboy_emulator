#include <stdint.h> // uint8_t

#include "bit.h"
#include "cpu.h"     // cpu_t
#include "cpu-registers.h"

uint16_t cpu_reg_pair_get(const cpu_t* cpu, reg_pair_kind reg){
    if (cpu == NULL){
        return 0;
    }

    switch(reg){
        case REG_BC_CODE : 
            return cpu->BC.BC;
            break;
        case REG_DE_CODE :
            return cpu->DE.DE;
            break;
        case REG_HL_CODE :
            return cpu->HL.HL;
            break;
        case REG_AF_CODE :
            return cpu->AF.AF;
            break;
        default:
            return 0;
            break;
    }

}

uint8_t cpu_reg_get(const cpu_t* cpu, reg_kind reg){
    if (cpu == NULL){
        return 0;
    }

    switch(reg){
        case REG_B_CODE : 
            return cpu->BC.B;
            break;
        case REG_C_CODE :
            return cpu->BC.C;
            break;
        case REG_D_CODE :
            return cpu->DE.D;
            break;
        case REG_E_CODE :
            return cpu->DE.E;
            break;
        case REG_H_CODE : 
            return cpu->HL.H;
            break;
        case REG_L_CODE :
            return cpu->HL.L;
            break;
        case REG_A_CODE :
            return cpu->AF.A;
            break;
        default:
            return 0;
            break;
    }
}

void cpu_reg_pair_set(cpu_t* cpu, reg_pair_kind reg, uint16_t value){
    if (cpu == NULL){
        return;
    }

    switch(reg){
        case REG_BC_CODE : 
            cpu->BC.BC = value;
            break;
        case REG_DE_CODE :
            cpu->DE.DE = value;
            break;
        case REG_HL_CODE :
            cpu->HL.HL = value;
            break;
        case REG_AF_CODE :
            cpu->AF.AF = value;
            cpu->AF.F = merge4(0,cpu->AF.F);
            break;
        default:
            break;
    }
}
