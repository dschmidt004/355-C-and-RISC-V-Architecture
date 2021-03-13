#include "IFe.h"


IFe *initIFe(Core *core) {

    IFe *fetch = (IFe *)malloc(sizeof(IFe));
    // fetch->prevPC;
    fetch->PC = core->PC;
    fetch->instr = core->instr_mem->instructions[core->PC / 4].instruction;
    
    // #TODO ~> add PC arithmetic functionality
    //       ~> don't forget to add same to Ife.h
    
    return fetch;
}

Signal PC_MUX(Signal sel, Signal input_0, Signal input_1) {
    if (sel == 0) { return input_0; } else { return input_1; }
}