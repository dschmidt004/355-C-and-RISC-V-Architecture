#ifndef __EX_H__
#define __EX_H__

#include <stdlib.h>
#include <stdio.h>
#include <string.h>

#include "ID.h"

typedef struct EX EX;
typedef struct EX
{
    int64_t ALU_result;
    int64_t zero;
    int64_t ALUin2;
    uint64_t ALUctrl;
    
    /* shift left 1 & addSUM for branch (id->PC)
     * instr and jumps not yet implemented */
    
} EX;

EX *initEX(ID *id);

Signal ALU_MUX(Signal sel, Signal input_0, Signal input_1);

void ALU(Signal in1, Signal in2, Signal ALU_ctrl, 
         Signal *ALU_result, Signal *zero);          //, unsigned PC_addr)

Signal ShiftLeftImm(Signal input, Signal bits_to_shift);

Signal ALUControlUnit(Signal ALUOp, Signal Funct7, Signal Funct3);


#endif
