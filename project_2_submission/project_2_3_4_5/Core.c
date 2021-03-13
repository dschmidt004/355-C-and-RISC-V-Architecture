#include "Core.h"

// array to simulate  RISC-V registers x0-x31
// array to simulate data memory as per assignment
uint64_t values[32];
uint64_t mem[4] = {16,128,8,4};

Core *initCore(Instruction_Memory *i_mem) {

    Core *core = (Core *)malloc(sizeof(Core));
    core->clk = 0;
    core->PC = 0;
    core->instr_mem = i_mem;
    core->tick = tickFunc;
    
    // init req'd values stored in registers
    values[25] = 4;
    values[10] = 4;
    values[22] = 1;

    return core;
}

// *DONE* FIXME 
bool tickFunc(Core *core)
{
    // Steps may include
    // (Step 1) Reading instruction from instruction memory
    unsigned instruction = core->instr_mem->instructions[core->PC / 4].instruction;
    unsigned pc_mux = 0;
    struct ControlSignals *signals = malloc(sizeof(*signals));
    
    // PC addr. to pass into ALU to verify result matches instr
    unsigned PC_addr = core->PC;
  
    // below: bit masking & shifting operations to pull decimal values
    // out of the 32-bit instr.
    unsigned opcode = instruction & 127;
    unsigned rd = instruction & 3968;
    unsigned funct3 = instruction & 28672;
    unsigned src_reg_1 = instruction & 1015808;
    unsigned src_reg_2 = 0;
    unsigned immed;
    unsigned funct7;
    
    rd = (rd >> 7);
    funct3 = (funct3 >> 12);
    src_reg_1 = (src_reg_1 >> 15);
        
    // prints the 32-bit instr. in hex and the PC counter VALUE
    fprintf(stdout, "\nInstruction 0x%08X at PC: = %d", instruction, PC_addr);
    fprintf(stdout, "\n------------------------------------");    
    
    // opcode branching to assign funct7 & immed
    // R-type
    if (opcode == 51) {
    
        src_reg_2 = instruction & 32505856;
        src_reg_2 = (src_reg_2 >> 20);
      
        funct7 = instruction & 4261412864;
        funct7 = (funct7 >> 25);
    } 
    // I-type
    if (opcode == 19 || opcode == 3) {
        
        src_reg_2 = 0;
        
        immed = instruction & 4293918720;
        immed = (immed >> 20);
    }
    // SB-type instr
    if (opcode == 99) {
   
        src_reg_2 = instruction & 32505856;
        src_reg_2 = (src_reg_2 >> 20);
          
        immed = instruction & 4290772992;
        immed = (immed >> 20);   
    }
    
    ControlUnit(opcode, signals);
    
    // get ALU control input value
    unsigned ALU_ctrl = ALUControlUnit(signals->ALUOp,funct7,funct3);
    
    // init ptr's to hold value of ALU out and zero-line
    Signal *ALU_result= malloc(sizeof(unsigned ));
    Signal *zero = malloc(sizeof(int));
        
  	unsigned immedOut = ImmeGen(instruction);
	  unsigned ALU_in = MUX(signals->ALUSrc,values[src_reg_2],immedOut);
     
    fprintf(stdout, "\nValue stored in rs1, register x%d: %d", 
            src_reg_1, values[src_reg_1]);
    if (opcode == 3 || opcode == 19) {
        fprintf(stdout, "\nImmediate value for computation: %d", immedOut); 
    } else 
        fprintf(stdout, "\nValue stored in rs2, register x%d: %d", 
                src_reg_2, values[src_reg_2]); 
     
         
    ALU(values[src_reg_1], ALU_in, ALU_ctrl, ALU_result, zero, PC_addr);
    
    if(opcode == 3 || opcode == 19 || opcode == 51) {

	      values[rd] = *ALU_result;
        if(opcode == 3) 
            values[rd] = mem[*ALU_result/8];
    }
    if (opcode == 99) {
        fprintf(stdout, "\nValue asserted on ALU zero line: %d\n\n", *zero); 
    } else
        fprintf(stdout, "\nResultant value stored in rd, register x%d: %d\n\n",
                rd, values[rd]);
      
	  //AND gate output

	  int and_gate;
    if(signals->Branch && zero) 
        and_gate = 1;
    else  and_gate = 0;
	
	  //add Sum 
	  unsigned shift1 = ShiftLeft1(immedOut);
 	  unsigned sum = Add(shift1,core->PC);
	
	  // PC MUX
	  unsigned PC1 =  core->PC + 4;
	  unsigned PC2 =  sum + core->PC;        
	  core->PC = MUX(and_gate,PC1,PC2);
     
    ++core->clk;
    if (core->PC > core->instr_mem->last->addr) {
        return false;
    }
    return true;
}

// *DONE* FIXME (1). Control Unit. Refer to Figure 4.18.
void ControlUnit (Signal input, ControlSignals *signals) {

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

// *DONE* FIXME (2). ALU Control Unit. Refer to Figure 4.12.
Signal ALUControlUnit(Signal ALUOp, Signal Funct7, Signal Funct3) {

    // add
    if (ALUOp == 2 && Funct7 == 0 && Funct3 == 0) {
        return 2;
    }
    // sub    
    if (ALUOp == 2 && Funct7 == 32 && Funct3 == 0) {
         return 6;
    }
    // AND
    if (ALUOp == 2 && Funct7 == 0 && Funct3 == 7) {
        return 0;
    }
    // OR
    if (ALUOp == 2 && Funct7 == 0 && Funct3 == 6) {
        return 1;
    }

    if (ALUOp == 0 && Funct7 == 0 && Funct3 ==1) {
    return 7;
    }
    // ld or sd
    if (ALUOp == 0) {
        return 2;
    }
    // for beq ~> need interior ALU logic for beq vs. bne
    if (ALUOp == 1) {
        return 6;
    }
}

// *DONE* FIXME (3). Imme. Generator
Signal ImmeGen(Signal instr) {

    unsigned opcode = instr  & 127;
    
    if (opcode == 51) {
        unsigned immed = instr & 4261412864;
        immed = (immed >> 25);
	      return immed;
	  } 

    if ( opcode == 3 || opcode == 19) {
    
        unsigned immed = instr & 4293918720;
        immed = (immed >> 20);
        return immed;
    }

    if ( opcode == 99) {
    
        signed immed = instr & 4293918720;
        immed = (immed >> 20);
        return immed;
    }
}

// *DONE* FIXME (4). ALU
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
     * add, ld, addi, slli, bne 
     *********************************************/
    
    // print PC instr. #
    // fprintf(stdout, "\n\nALU output for instr. at PC: %d\n", PC_addr);
    
    // AND ~> ALU ctrl line = 0
    // OR ~> ALU ctrl line = 1
 
    // add ~> ALU ctrl line = 2
    if (ALU_ctrl_signal == 2) *ALU_result = (input_0 + input_1);

    // sub ~> ALU ctrl line = 6
    else if (ALU_ctrl_signal == 6) *ALU_result = (input_0 - input_1);
    
    // shift left logical ~> ALU ctrl line = 7
    // ***found appendix A-37***
    else if (ALU_ctrl_signal == 7) *ALU_result = ShiftLeftImm(input_0, input_1);

    // assert zero line if ALU_result == 0    
    if (*ALU_result == 0) *zero = 1; 
    else *zero = 0;
    
    // fprintf(stdout, "ALU_result = %d\nzero = %d\n\n",*ALU_result,*zero);
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

// (7). ShiftLeftImm ~> updated for any shift 
Signal ShiftLeftImm(Signal input, Signal bits_to_shift) {
   return input << bits_to_shift;
}
