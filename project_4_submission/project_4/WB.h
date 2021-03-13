#ifndef __WB_H__
#define __WB_H__

#include "Core.h"
#include "ID.h"
#include "EX.h"
#include "MEM.h"

typedef struct WB WB;
typedef struct WB
{
    int64_t valueToWrite;
        
    // Addr PC;
    
    // unsigned r_mem_data;
    // unsigned result;
    // unsigned rd;
} WB;


WB *initWB(ID *id, EX *ex, MEM *mem, Core *core);

Signal wbMUX(Signal sel, Signal input_0, Signal input_1);

#endif
