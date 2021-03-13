#ifndef __MEM_H__
#define __MEM_H__

#include <stdlib.h>
#include <stdio.h>
#include <string.h> 

#include "Core.h"
#include "ID.h"
#include "EX.h"

typedef struct MEM MEM;
typedef struct MEM
{
    uint8_t memRead;
    int64_t memReadValue;
    
    uint8_t memWrite;
    
    uint8_t ANDgate;
    
    
} MEM;
#endif

MEM *initMEM(ID *id, EX *ex, Core *core);