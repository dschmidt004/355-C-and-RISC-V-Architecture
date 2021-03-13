#ifndef __CTRLSIGS_H__
#define __CTRLSIGS_H__

#include <stdlib.h>
#include <stdint.h>
typedef int64_t Signal;
typedef struct ControlSignals ControlSignals;
typedef struct ControlSignals
{
    Signal Branch;
    Signal MemRead;
    Signal MemtoReg;
    Signal ALUOp;
    Signal MemWrite;
    Signal ALUSrc;
    Signal RegWrite;
} ControlSignals;

#endif
