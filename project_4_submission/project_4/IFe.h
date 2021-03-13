#ifndef __IF_H__
#define __IF_H__

#include <stdlib.h>
#include <stdio.h>
#include <string.h>

#include "Core.h"
#include "Instruction_Memory.h"


typedef struct IFe IFe;
typedef struct IFe
{
    //int prevPC;
    uint64_t PC;
    uint32_t instr;
} IFe;

IFe *initIFe(Core *core);

Signal PC_MUX(Signal sel, Signal input_0, Signal input_1);

#endif
