#include <stdio.h>
#include "Core.h"
#include "Parser.h"

int main(int argc, const char *argv[]) {	
    if (argc != 2) {
        
        printf("Usage: %s %s\n", argv[0], "<trace-file>");
        return 0;
    }
    
    /* Task One 
     * (1) parse and translate all the assembly instructions into binary format;
     * (2) store the translated binary instructions into instruction memory. */
    Instruction_Memory instr_mem;
    instr_mem.last = NULL;
    loadInstructions(&instr_mem, argv[1]);
    
    // print binary instr. translated from assembly in trace file
    unsigned PC = 0;
    while (1) {
    
        Instruction *instr = &(instr_mem.instructions[PC / 4]);
        printf("\nInstruction at PC: %u\n", PC);
        unsigned mask = (1 << 31);
        for (int i = 31; i >= 0; i--) {
        
            if (instr->instruction & mask) { printf("1");}
            else { printf("0"); }
            
            if (i == 25 | i == 20 | i == 15 | i == 12 | i == 7) printf(" ");

            mask >>= 1;
        }
        printf("\n");
        if (instr == instr_mem.last) { break; }
        PC += 4;
    }
    /* Task Two */
    // TODO, implement Core.{h,c}
    Core *core = initCore(&instr_mem);
    
    // reg[0] = 0 (ALWAYS)
    core->reg[0] = 0;
    
    // project 4 initial values mem[] & reg[]
    core->reg[1] = 0;
    core->reg[2] = 10;
    core->reg[3] = -15;
    core->reg[4] = 20;
    core->reg[5] = 30;
    core->reg[6] = -35;
    core->mem[40] = -63;
    core->mem[48] = 63;
    
    /* Task Three - Simulation */
    while (core->tick(core));
    
    fprintf(stdout, "\n-------------------------------");
    fprintf(stdout, "\n*** Simulation is finished. ***\n");
    fprintf(stdout, "-------------------------------\n");
    
    /* print values stored in relevant registers/memory PROJECT 4 */
    uint8_t k;
    fprintf(stdout, "initialized reg[] & mem[] values\n");
    for (k=1; k<=6; k++) { fprintf(stdout, "reg[%d] = %d\n", k, core->reg[k]); }
    fprintf(stdout, "mem[40] = %d\nmem[48] = %d\n", core->mem[40], core->mem[48]);
    
    fprintf(stdout, "------------------------------\n");
    fprintf(stdout, "resultant reg[] & mem[] values\n"); 
    for (k=10; k<=14; k++) { fprintf(stdout, "reg[%d] = %d\n", k, core->reg[k]); }
    fprintf(stdout, "------------------------------\n");
    
    free(core);    
}
