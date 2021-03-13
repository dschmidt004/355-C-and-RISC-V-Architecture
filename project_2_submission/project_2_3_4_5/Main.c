#include <stdio.h>

#include "Core.h"
#include "Parser.h"

int main(int argc, const char *argv[]) {	
    if (argc != 2) {
        
        printf("Usage: %s %s\n", argv[0], "<trace-file>");
        return 0;
    }

    /* Task One */
    // TODO, (1) parse and translate all the assembly instructions into binary format;
    // (2) store the translated binary instructions into instruction memory.
    Instruction_Memory instr_mem;
    instr_mem.last = NULL;
    loadInstructions(&instr_mem, argv[1]);


    // print binary instr translated from assembly in trace file
    unsigned PC = 0;
    while (1) {
    
        Instruction *instr = &(instr_mem.instructions[PC / 4]);
        printf("\nInstruction at PC: %u\n", PC);
        unsigned mask = (1 << 31);
        for (int i = 31; i >= 0; i--) {
        
            if (instr->instruction & mask) { printf("1 ");}
            else { printf("0 "); }

            mask >>= 1;
        }
        printf("\n");
        if (instr == instr_mem.last) { break; }
        PC += 4;
    }

    /* Task Two */
    // TODO, implement Core.{h,c}
    Core *core = initCore(&instr_mem);

    /* Task Three - Simulation */
    while (core->tick(core));

    printf("Simulation is finished.\n");

    free(core);    
}
