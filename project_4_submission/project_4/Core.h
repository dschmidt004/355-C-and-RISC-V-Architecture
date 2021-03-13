#ifndef __CORE_H__
#define __CORE_H__

#include "Instruction_Memory.h"

#include <stdbool.h>
#include <stdlib.h>
#include <stdio.h>
#include <stdint.h>

#define BOOL bool

#define REGISTERS_32 32
#define MEM_BYTE_SIZE 1024

typedef uint8_t Byte;
typedef int64_t Signal;
typedef int64_t Register;

struct Core;
typedef struct Core Core;
typedef struct Core
{
    // Addr & Tick are typedef's ~> uint64_t
    
    // core clock
    Tick clk; 
    // PC counter
    Addr PC; 

    // this is a struct that gets the instruction
    // **need to understand this better**
    Instruction_Memory *instr_mem;
     
    // **double check accuracy re: signed/unsigned**
    //----------------------------------------------
    // data memory
    int8_t mem[MEM_BYTE_SIZE]; // data memory
    // register file
    uint64_t reg[REGISTERS_32]; // register file.
    
    // Byte mem[MEM_BYTE_SIZE]; // data memory
    // Register reg[REGISTERS_32]; // register file.
    
    bool (*tick)(Core *core);
}Core;

Core *initCore(Instruction_Memory *i_mem);

bool tickFunc(Core *core);

// (4). MUX
Signal MUX(Signal sel, Signal input_0, Signal input_1);

// (5). Add
Signal Add(Signal input_0, Signal input_1);

// (6). ShiftLeft1
Signal ShiftLeft1(Signal input);

#endif
