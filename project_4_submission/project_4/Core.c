#include "Core.h"
#include "IFe.h"
#include "EX.h"
#include "ID.h"
#include "MEM.h"
#include "WB.h"

/***********************************************
 ** GLOBAL VARS TO SAVE VALUES BETWEEN CYCLES **
 ***********************************************/
// CURRENTLY UNUSED ~> may be needed for pipeline simulation?
uint64_t currPC = 0;
uint64_t branchPC = 0;
uint32_t branchInstr = 0;
Signal zeroANDgate = 0;

Core *initCore(Instruction_Memory *i_mem) {

    Core *core = (Core *)malloc(sizeof(Core));
    core->clk = 0;
    core->PC = 0;
    core->instr_mem = i_mem;
    core->tick = tickFunc;
    return core;
}

// *DRIVER FUNCTION FOR SINGLE STAGE ARCHITECTURE*
bool tickFunc(Core *core) {
    
    /***************
     * FETCH STAGE *
     ***************/
     
    IFe *fetch = initIFe(core);
    // prints the 32-bit instr. in hex and the PC counter VALUE
    fprintf(stdout, "\n-------------------------------------------"); 
    fprintf(stdout, "\n    Instruction 0x%08X at PC: = %d    ", fetch->instr, fetch->PC);
    fprintf(stdout, "\n-------------------------------------------\n");    
    
    /* multiple structs can access the PC via structName->PC */
  
    /****************
     * DECODE STAGE *
     ****************/
    ID *id =  initID(fetch, core);
    if (id->opcode == 51) {
        fprintf(stdout, "*** ID STRUCT: R-TYPE ***\n");
        fprintf(stdout, "opcode: %d\n", id->opcode);
        fprintf(stdout, "rd: %d\n", id->rd);
        fprintf(stdout, "funct3: %d\n", id->funct3);
        fprintf(stdout, "rs1: %d\n", id->rs1);
        fprintf(stdout, "rs2: %d\n", id->rs2);
        fprintf(stdout, "funct7: %d\n", id->funct7);
        fprintf(stdout, "ALUop: %d\n", id->signals->ALUOp);
        fprintf(stdout, "ALU_in1: %d\n", id->ALU_in1);
        fprintf(stdout, "ALU_in2: %d\n", id->ALU_in2);
    }
    if (id->opcode == 3) {
        fprintf(stdout, "*** ID STRUCT: I-TYPE ***\n");
        fprintf(stdout, "opcode: %d\n", id->opcode);
        fprintf(stdout, "rd: %d\n", id->rd);
        fprintf(stdout, "funct3: %d\n", id->funct3);
        fprintf(stdout, "rs1: %d\n", id->rs1);
        fprintf(stdout, "imm: %d\n", id->imm);
        fprintf(stdout, "ALUop: %d\n", id->signals->ALUOp);
        fprintf(stdout, "ALU_in1: %d\n", id->ALU_in1);
        fprintf(stdout, "ALU_in2 (imm): %d\n", id->imm);
    }
    
    /*****************
     * EXECUTE STAGE *
     *****************/ 
    EX *ex = initEX(id);
    
    /****************
     * MEMORY STAGE *
     ****************/ 
    MEM *mem = initMEM(id, ex, core);
    
    /********************
     * WRITE BACK STAGE *
     ********************/
    WB *wb = initWB(id, ex, mem, core);
    
    
    /*
	  // AND gate for PC inc./branch/jump
	  uint8_t and_gate;
    if(signals->Branch && *zero) 
        and_gate = 1;
    else  and_gate = 0;
    */
	
    /*
	  //add Sum 
	  unsigned shift1 = ShiftLeft1(immedOut);
 	  unsigned sum = Add(shift1,core->PC);
	
	  // PC MUX
	  unsigned PC1 =  core->PC + 4;
	  unsigned PC2 =  sum + core->PC;        // THIS IS THE LINE CALLING THE LOOP
	  core->PC = MUX(and_gate,PC1,PC2);
    */
    core->PC = core->PC + 4;
    
    free(fetch);
    free(id->signals);
    free(id);
    free(ex);
    free(mem);
    free(wb);
    
    
    ++core->clk;
    if (core->PC > core->instr_mem->last->addr) { return false; }
    return true;
}

/****************
 * END TICKFUNC *
 ****************/

// MUX
Signal MUX(Signal sel, Signal input_0, Signal input_1) {
    if (sel == 0) { return input_0; } else { return input_1; }
}

// Add
Signal Add(Signal input_0, Signal input_1) {
    return (input_0 + input_1);
}

// ShiftLeft1
Signal ShiftLeft1(Signal input) {
   return input << 1;
}
