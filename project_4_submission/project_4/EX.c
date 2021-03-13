#include "EX.h"



EX *initEX(ID *id) {

    EX *ex = (EX *)malloc(sizeof(EX));
    // have access to the PC count with id->PC
    /* shift left 1 & addSUM for branch (id->PC)
     * instr and jumps not yet implemented */
    
    fprintf(stdout, "\nprinted in initEX before ALU_MUX() call:\n"); 
    fprintf(stdout, "select = %d, MUXin1 = %d, MUXin2 = %d\n",
                            id->signals->ALUSrc, id->ALU_in2, id->imm); 
    
    ex->ALUin2 = ALU_MUX(id->signals->ALUSrc, id->ALU_in2, id->imm);
    
    ex->ALUctrl = ALUControlUnit(id->signals->ALUOp, id->funct7, id->funct3);
    
    fprintf(stdout, "\nprinted in initEX after ALU_MUX() call:\n"); 
    fprintf(stdout, "ALU_in1 = %d, ALUin2 = %d\n", id->ALU_in1, ex->ALUin2); 
    
    
    Signal *ALU_result = malloc(sizeof(int64_t));
    Signal *zero = malloc(sizeof(int64_t));
    ALU(id->ALU_in1, ex->ALUin2, ex->ALUctrl, ALU_result, zero); 
    
    ex->ALU_result = *ALU_result;
    ex->zero = *zero;
    
    fprintf(stdout, "\nprinted in initEX after ALU() call:\n"); 
    fprintf(stdout, "ALU_result = %d, zero = %d\n", ex->ALU_result, ex->zero); 
    
    free(ALU_result);
    free(zero);
    
    return ex;
}

Signal ALU_MUX(Signal sel, Signal input_0, Signal input_1) {
    if (sel == 0) { return input_0; } else { return input_1; }
}

void ALU(Signal in1, Signal in2, Signal ALU_ctrl, Signal *ALU_result, Signal *zero) {
    
    // add
    if (ALU_ctrl == 2) { *ALU_result = (in1 + in2); }

    // sub ~> ALU ctrl line = 6
    else if (ALU_ctrl == 6) { *ALU_result = (in1 - in2); }

    // shift left logical ~> ALU ctrl line = 7
    else if (ALU_ctrl == 7) { *ALU_result = ShiftLeftImm(in1, in2); }

    // assert zero line if ALU_result == 0
    if (*ALU_result == 0) *zero = 1;
    else *zero = 0;
}


Signal ShiftLeftImm(Signal input, Signal bits_to_shift) {
    // printf(" \n input = %d \n bits to shift = %d\n",input,bits_to_shift);
  	// printf(" \n answer %d \n ",(input << bits_to_shift));
	  return (input << bits_to_shift);
}


Signal ALUControlUnit(Signal ALUOp, Signal Funct7, Signal Funct3) {

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
//--WHAT'S THIS?????
    else if (ALUOp == 1  && Funct3 ==1) {
        return 7;
    }
    // sw 
    else if (ALUOp == 0 && Funct3 == 2) {
        return 2;
    }
    // jal
    else if (ALUOp == 3&& Funct3==0 ) {
        return 9;
    }
    // addi
    else if (ALUOp == 0 && Funct3==0 ) {
        return 2;
    }
    // ld, sd and lw
    else if (ALUOp == 0 && Funct3 == 3) {
        return 2;
    }
    // jalr
    else if (ALUOp == 4) {
        return 2;
    }
//--???????????????????????????????????????
    // jal also runs into BEQ
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

