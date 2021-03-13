#include "WB.h"

WB *initWB(ID *id, EX *ex, MEM *mem, Core *core) {

    WB *wb = (WB *)malloc(sizeof(WB));
    
    wb->valueToWrite = wbMUX(id->signals->MemtoReg, ex->ALU_result, mem->memReadValue);
    
    core->reg[id->rd] = wb->valueToWrite;
    
    if (id->signals->MemtoReg == 1) {
        fprintf(stdout, "\n*** PRINTED IN WB STRUCT ***\n");
        fprintf(stdout, "reg[%d] = mem[%d] = %d\n", id->rd, ex->ALU_result, core->mem[ex->ALU_result]);
        //fprintf(stdout, "reg[%d] = %d\n", id->rd, core->mem[ex->ALU_result]);
    }
    else {
        fprintf(stdout, "\n*** PRINTED IN WB STRUCT ***\n");
        fprintf(stdout, "reg[%d] = ALU_result = %d\n", id->rd, ex->ALU_result);
        //fprintf(stdout, "reg[%d] = %d\n", id->rd, ex->ALU_result);
    }
    
    // wb->PC;
    // wb->result;
    // wb->r_mem_data;
    // wb->rd;
    
    return wb;
}

// (4). MUX
Signal wbMUX(Signal sel, Signal input_0, Signal input_1) {
    if (sel == 0) { return input_0; } else { return input_1; }
}