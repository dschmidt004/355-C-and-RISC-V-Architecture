#ifndef __ID_H__
#define __ID_H__
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include "ControlSignals.h"
#include "Instruction_Memory.h"
#include "Core.h"
#include "IFe.h"

typedef struct ID ID;
typedef struct ID
{
    uint64_t PC;
    
    unsigned opcode;
    ControlSignals *signals;
    
    unsigned rd;
    unsigned funct3;
    unsigned rs1; 
    unsigned rs2; 
    unsigned funct7;
    unsigned imm;
    
    int64_t ALU_in1;
    int64_t ALU_in2;
    
} ID;

ID *initID(IFe *fetch, Core *core);

void ControlUnit (Signal input, ControlSignals *signals);

Signal ImmeGen(Signal instr);

#endif
