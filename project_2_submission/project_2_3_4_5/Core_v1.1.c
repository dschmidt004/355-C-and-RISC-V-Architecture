#include "Core.h"

Core *initCore(Instruction_Memory *i_mem) {

    Core *core = (Core *)malloc(sizeof(Core));
    core->clk = 0;
    core->PC = 0;
    core->instr_mem = i_mem;
    core->tick = tickFunc;

    // i think fixed  FIXME, initialize register file here.
    //core->data_mem[0] = NULL;

    // i think fixed FIXME, initialize data memory here.
    //core->reg_file[0] = NULL;

    return core;
}

// FIXME, implement this function
bool tickFunc(Core *core)
{
    // Steps may include
    // (Step 1) Reading instruction from instruction memory
    unsigned instruction = core->instr_mem->instructions[core->PC / 4].instruction;
    unsigned pc_mux = 0;
    struct ControlSignals *signals = malloc(sizeof(*signals));
    
    // *mark 2/19* - want to pass this into ALU()
    unsigned PC_addr = core->PC;

    unsigned opcode = instruction & 127;
    unsigned rd = instruction & 3968;
    unsigned funct3 = instruction & 28672;
    unsigned src_reg_1 = instruction & 1015808;
    unsigned src_reg_2 = 0; 
    rd = (rd >> 7);
    funct3 = (funct3 >> 12);
    src_reg_1 = (src_reg_1 >> 15);
    unsigned immed;
    
    // prints the 32-bit instr. in hex and the PC counter VALUE
    fprintf(stdout, "\n\ninstruction (in hex) = %x\n", instruction);
    fprintf(stdout, "Instr. at PC: = %d\n", PC_addr);    
    
    if(opcode == 51) {
      src_reg_2 = instruction & 32505856;
      // here immed == funct7
      immed = instruction & 4261412864;
   
      src_reg_2 = (src_reg_2 >> 20);
      immed = (immed >> 25);
    } 
    if(opcode == 19 || opcode == 3) {
      src_reg_2 = 0;
      // here immed == immed
      immed = instruction & 4293918720;
      immed = (immed >> 20);
    }
    // SB-type instr
    // needs to be updated
    if(opcode == 99) {
    
      // 
      src_reg_2 = rd;
      // here immed == immed    
      immed = instruction & 4290772992;
      immed = (immed >> 20);   
    }
    
    ControlUnit(opcode, signals);
    
    // immed == funct7 ~> passed argument below
    unsigned ALU_ctrl = ALUControlUnit(signals->ALUOp,immed,funct3);
    unsigned immedOut = ImmeGen(4);
    
    if(opcode == 51 || opcode ==  99) {

      unsigned ALU_in = MUX(signals->ALUSrc,src_reg_2,immedOut); 
      Signal *ALU_result= malloc(sizeof(unsigned ));;
      Signal *zero = malloc(sizeof(int));;
      *zero = 1;
      *ALU_result = 2;
      ALU(src_reg_1, ALU_in, ALU_ctrl, ALU_result, zero, PC_addr);
    }
	
    // This is used to control the mux and PC counter 
    // iprintf("%d \n\n", input);
    // f(signal->branch == 1 && zero == 1) pc_mux = 1;
    // else pc_mux = 0;
    unsigned value_after_addsum = 1; //fixme! need to write this function 
    
    if (pc_mux == 1) core->PC = value_after_addsum;
    else core->PC += 4;  

    ++core->clk;
    // Are we reaching the final instruction?
    if (core->PC > core->instr_mem->last->addr) {
        return false;
    }
    return true;
}

//think its fixed  FIXME (1). Control Unit. Refer to Figure 4.18.
void ControlUnit(Signal input, ControlSignals *signals) {

    // R-type
    if (input == 51) {
    
        signals->ALUSrc = 0;
        signals->MemtoReg = 0;
        signals->RegWrite = 1;
        signals->MemRead = 0;
        signals->MemWrite = 0;
        signals->Branch = 0;
        signals->ALUOp = 2;
    }

    // I-type 
    if (input == 3 || input == 19) {
    
        signals->ALUSrc = 1;
        signals->MemtoReg = 1;
        signals->RegWrite = 1;
        signals->MemRead = 1;
        signals->MemWrite = 0;
        signals->Branch = 0;
        signals->ALUOp = 0;
    }
    
    // SB-type
    if (input == 99) {
    
        signals->ALUSrc = 0;
        signals->MemtoReg = 0;
        signals->RegWrite = 0;
        signals->MemRead = 0;
        signals->MemWrite = 0;
        signals->Branch = 1;
        signals->ALUOp = 1;
    }
}

// think its fixed  FIXME (2). ALU Control Unit. Refer to Figure 4.12.
Signal ALUControlUnit(Signal ALUOp, Signal Funct7, Signal Funct3) {

    // For add
    if (ALUOp == 2 && Funct7 == 0 && Funct3 == 0) {
        return 2;
    }
    // sub    
    if (ALUOp == 2 && Funct7 == 32 && Funct3 == 0) {
         return 6;
    }
    // and
    if (ALUOp == 2 && Funct7 == 0 && Funct3 == 7) {
        return 0;
    }
    // or
    if (ALUOp == 2 && Funct7 == 0 && Funct3 == 6) {
        return 1;
    }

    // For ld or sd
    if (ALUOp == 0) {
        return 2;
    }
    // for beq
    if (ALUOp == 1) {
        return 6;
    }
}

// FIXME (3). Imme. Generator
Signal ImmeGen(Signal input)
{
	return 4;

}

// FIXME (4). ALU
void ALU(Signal input_0,
         Signal input_1,
         Signal ALU_ctrl_signal,
         Signal *ALU_result,
         Signal *zero,
         unsigned PC_addr) {
    
    /* void funct ~> no return value
     * do necessary computation & update
     * value stored in ptr's *ALU_result & *zero 
     *********************************************
     * ALU needs to support:
     * add, ld, addi, slli, sd 
     *********************************************/
    
    // print PC instr. #
    fprintf(stdout, "\n\nALU output for instr. at PC: %d\n", PC_addr);
    
    
    // AND ~> ALU ctrl line = 0
    // OR ~> ALU ctrl line = 1
    
    // add ~> ALU ctrl line = 2
    if (ALU_ctrl_signal == 2) {
    
        *ALU_result = (input_0 + input_1);
        if (*ALU_result == 0) { *zero = 1; } else { *zero = 0; }
    }
    // sub ~> ALU ctrl line = 6
    if (ALU_ctrl_signal == 2) {

    }
    fprintf(stdout, "ALU_result = %d\nzero = %d\n\n",*ALU_result,*zero);

}

// (4). MUX
Signal MUX(Signal sel, Signal input_0, Signal input_1) {
    if (sel == 0) { return input_0; } else { return input_1; }
}

// (5). Add
Signal Add(Signal input_0, Signal input_1) {
    return (input_0 + input_1);
}

// (6). ShiftLeft1
Signal ShiftLeft1(Signal input) {
   return input << 1;
}
