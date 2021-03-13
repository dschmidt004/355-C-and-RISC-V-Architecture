#include "Core.h"

// array to simulate  RISC-V registers x0-x31
// array to simulate data memory as per assignment
int64_t mat[15];
int64_t out[4]={0};
uint64_t values[32]={0};
uint64_t mem[4] = {16,128,8,4};

Core *initCore(Instruction_Memory *i_mem) {

    Core *core = (Core *)malloc(sizeof(Core));
    core->clk = 0;
    core->PC = 0;
    core->instr_mem = i_mem;
    core->tick = tickFunc;
    // init req'd values stored in registers
    for(int i=0; i<16;i++)
	{
		mat[i]=i;
	}

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
	printf("\nfunct 7 =%d \n",funct7); 
    }
    //JAL
    if (opcode == 111) {
        //src_reg_2 not used
        src_reg_2 = 0;
        immed = instruction & 4294963200;
        immed = (immed >> 12);
    }
 
    // I-type
    if (opcode == 19 || opcode == 3||opcode == 103) {
        //src_reg_2 not used
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
	//pretty sure we need to implement immediate correctly!!!  
    }

// FIX ME?  have we implemented the rd immed for SB and S type codes?
    if (opcode == 35) {

        src_reg_2 = instruction & 32505856;
        src_reg_2 = (src_reg_2 >> 20);

    }

    
    ControlUnit(opcode, signals);
    
    // get ALU control input value
    unsigned ALU_ctrl = ALUControlUnit(signals->ALUOp,funct7,funct3);
    
    // init ptr's to hold value of ALU out and zero-line
    Signal *ALU_result= malloc(sizeof(unsigned ));
    Signal *zero = malloc(sizeof(int));
    
    // **should be signed for SB type??????  
   unsigned immedOut = ImmeGen(instruction);
   //
   fprintf(stdout, "\n\nimmedOut = %d\n", immedOut);
   fprintf(stdout, "(immedOut << 1) = %d\n\n\n\n\n\n", (immedOut << 1));
   //
    
    unsigned ALU_in_2 = MUX(signals->ALUSrc, values[src_reg_2], immedOut);
     
    fprintf(stdout, "\nValue stored in rs1, register x%d: %d",src_reg_1, values[src_reg_1]);
    
    if (opcode == 3 || opcode == 19) {
        fprintf(stdout, "\nImmediate value for computation: %d", immedOut); 
    } else 
        fprintf(stdout, "\nValue stored in rs2, register x%d: %d", 
                src_reg_2, values[src_reg_2]); 
     
    printf("\n value of src_reg_2 input from muxi=%d\n",ALU_in_2);         
    ALU(values[src_reg_1], ALU_in_2, ALU_ctrl, ALU_result, zero, PC_addr);
    
    if(opcode == 3 || opcode == 19 || opcode == 51) {

	      values[rd] = *ALU_result;
        if(opcode == 3) {
	    	
            values[rd] = mem[*ALU_result/8];
	    out[*ALU_result/8]=5;
	}
    }
    if (opcode == 99) {
        fprintf(stdout, "\nValue asserted on ALU zero line: %d\n\n", *zero); 
    } else
        fprintf(stdout, "\nResultant value stored in rd, register x%d: %d\n\n",
                rd, values[rd]);
      
	  //AND gate output
   
	  int and_gate;
    if(signals->Branch && *zero) 
        and_gate = 1;
    else  and_gate = 0;
	
	  //add Sum 
	  unsigned shift1 = ShiftLeft1(immedOut);
 	  unsigned sum = Add(shift1,core->PC);
	
	  // PC MUX
	  unsigned PC1 =  core->PC + 4;
	  unsigned PC2 =  sum + core->PC;        // THIS IS THE LINE CALLING THE LOOP
	  core->PC = MUX(and_gate,PC1,PC2);
    for(int i = 0; i<4; i++){
	printf("\n out[%d]=%d \n",i,out[i]);

    }  
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
    //JALR
    if (input ==103) {

        signals->ALUSrc = 1;
        signals->MemtoReg = 1;
        signals->RegWrite = 1;
        signals->MemRead = 1;
        signals->MemWrite = 0;
        signals->Branch = 1;
        signals->ALUOp = 4;
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
    //JAL
    if (input == 111) {

        signals->ALUSrc = 0;
        signals->MemtoReg = 0;
        signals->RegWrite = 0;
        signals->MemRead = 0;
        signals->MemWrite = 0;
        signals->Branch = 1;
        signals->ALUOp = 3;
    }
    //sw
    if (input == 35) {

        signals->ALUSrc = 1;
        signals->MemtoReg = 0;
        signals->RegWrite = 0;
        signals->MemRead = 0;
        signals->MemWrite = 1;
        signals->Branch = 0;
        signals->ALUOp = 0;

    }


   
}

// *DONE* FIXME (2). ALU Control Unit. Refer to Figure 4.12.
Signal ALUControlUnit(Signal ALUOp, Signal Funct7, Signal Funct3) {
	printf("\n ALUop= %d Funct7 = %d Funct3 = %d \n",ALUOp,Funct7,Funct3);

    // add
    if (ALUOp == 2 && Funct7 == 0 && Funct3 == 0) {
        return 2;
    }
    // sub    
    else if (ALUOp == 2 && Funct7 == 32 && Funct3 == 0) {
         return 6;
    }
    // AND
    else if (ALUOp == 2 && Funct7 == 0 && Funct3 == 7) {
        return 0;
    }
    // OR
    else if (ALUOp == 2 && Funct7 == 0 && Funct3 == 6) {
        return 1;
    }
    // slli ?!?!?!*********************************************
    else if (ALUOp == 0  && Funct3 ==1) {
        return 7;
    }

    else if (ALUOp == 1  && Funct3 ==1) {
        return 7;
    }

    //sw 
    else if (ALUOp == 0 && Funct3 == 2) {
        return 2;
    }
    //JAL
    else if (ALUOp == 3&& Funct3==0 ) {
        return 9;
    }
    //addi
        else if (ALUOp == 0 && Funct3==0 ) {
        	printf("fuck");
	        return 2;
        }
                   


    // ld, sd and lw
    else if (ALUOp == 0 && Funct3 == 3) {
        return 2;
    }
    //JALR
    else if (ALUOp == 4) {
        return 2;
    }

    //Jal also runs into BEQ
    // for beq ~> need interior ALU logic for beq vs. bne
    else if (ALUOp == 1) {
        // beq
        if (Funct3 == 0)
            return 6;
        // bne
        else if (Funct3 == 1)
            return 8;
	
    }
}

// *DONE* FIXME (3). Imme. Generator
Signal ImmeGen(Signal instr) {

    unsigned opcode = instr  & 127;
    
    if (opcode == 51) {
        unsigned immed = instr & 4261412864;
        immed = (immed >> 25);
	      return immed;
             
	  } else if ( opcode == 3 || opcode == 19) {
    
        unsigned immed = instr & 4293918720;
        immed = (immed >> 20);
        return immed;
        
    }
    // JALR 
    else if (opcode == 103) {

        unsigned immed;
        immed = instr & 4293918720;
        return immed;
     }
    //S-type
    else if (opcode == 35) {

	unsigned immed;
        unsigned immed11_5 = instr & 4261412864;
        immed11_5 = (immed >> 25);
        unsigned immed4_0 = instr & 1920;
        immed4_0= (immed4_0 >> 7);
	
	immed = immed4_0;
        immed |= (immed11_5 << 4);
	
	return immed;
     }
     //JAL
     else if (opcode == 111) {

        signed immed;
        unsigned immed_masked = instr & 4294963200;
     	immed_masked = (immed_masked >> 12); 
        unsigned immed19_12 = immed_masked & 255;
	immed_masked = (immed_masked >> 8);
	unsigned immed11 = immed_masked & 1;
	immed_masked = (immed_masked >> 1);

        unsigned immed10_1 = immed_masked & 1023;
	immed_masked = (immed_masked >> 10);

	unsigned immed20 = immed_masked & 1;	
	
	immed = 0;
	immed |= (immed10_1 << 1);

        immed |= (immed11 << 10);

	immed |= (immed19_12 << 11);

	immed |= (immed20 << 20);
	return immed;

     }

       
    //BNE
    else if ( opcode == 99) { 
        
        // bit masking operation to parse instr for immed fields
        signed immed_masked = instr & 4261416832;
 
        // 1 111111 0000000000000 1111 1 0000000
        // 12 10:5                4:1  11
        
        // 1 000000 0000000000000 0000 0 0000000
        unsigned imm12 = immed_masked & 2147483648;
        // 0 000000 0000000000000 0000 1 0000000
        unsigned imm11 = immed_masked & 128;
        // 0 111111 0000000000000 0000 0 0000000
        unsigned imm10_5 = immed_masked & 2113929216;
        // 0 000000 0000000000000 1111 0 0000000
        unsigned imm4_1 = immed_masked & 3840;
                
        signed immed = (imm4_1 >> 8);
        immed |= (imm10_5 >> 21);
        immed |= (imm11 << 3);
        immed |= (imm12 >> 20);
        if (imm12 == 0) {
            return immed;
        } else {
            immed |= (4294963200);
            return immed;     
        }
        
        /*
        // edited to include the leftshift later in the datapath
        // for debugging
        signed immed = (imm4_1 >> 7);
        immed |= (imm10_5 >> 20);
        immed |= (imm11 << 4);
        immed |= (imm12 >> 19);
        // need a conditional for (+) or (-) immed value
        if (imm12 == 0) {
            return immed;
        } else {
            immed |= (4294959104);
            return immed;     
        }
        //immed |= (4294959104);
        */
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
    fprintf(stdout, "\n\nALU_ctrl_signal: %d\n", ALU_ctrl_signal);
    
    // add ~> ALU ctrl line = 2
    if (ALU_ctrl_signal == 2) *ALU_result = (input_0 + input_1);

    // sub ~> ALU ctrl line = 6
    else if (ALU_ctrl_signal == 6) *ALU_result = (input_0 - input_1);
    
    // shift left logical ~> ALU ctrl line = 7
    // ***found appendix A-37***
    else if (ALU_ctrl_signal == 7){
    	*ALU_result = ShiftLeftImm(input_0, input_1);
    }

    else if(ALU_ctrl_signal == 9) {

	*zero = 1;

    }    
    // bne
    else if (ALU_ctrl_signal == 8) {
        *ALU_result = (input_0 - input_1);
        if (*ALU_result == 0) *zero = 0; 
        else *zero = 1; 
        fprintf(stdout, "\nALU_result = %d\nzero = %d\n\n",*ALU_result,*zero);
    }

    // assert zero line if ALU_result == 0 (with exception for bne) 
    if (ALU_ctrl_signal != 8) {
        
        if (*ALU_result == 0) *zero = 1; 
        else *zero = 0;
        fprintf(stdout, "\nALU_result = %d\nzero = %d\n\n",*ALU_result,*zero);
    }
    
    
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
        printf(" \n input = %d \n bits to shift = %d\n",input,bits_to_shift);
	printf(" \n answer %d \n ",(input << bits_to_shift));

	return (input << bits_to_shift);
}
