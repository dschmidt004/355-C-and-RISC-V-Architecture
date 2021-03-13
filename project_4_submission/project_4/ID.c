#include "ID.h"


ID *initID(IFe *fetch, Core *core) {

    ID *id = (ID *)malloc(sizeof(ID));
    id->PC = fetch->PC;
    
    // get opcode to facilitate instr. decode
    id->opcode = fetch->instr & 127;
    
    // CONTROL UNIT
    id->signals = (ControlSignals *)malloc(sizeof(ControlSignals));
    ControlUnit(id->opcode, id->signals);
    
    /* BRANCH ON OPCODE TO:
     * ASSIGN FIELDS BASED ON INSTR TYPE
     * READ DATA FROM REGISTERS 
     * CALL IMMEDIATE GENERATOR */
    
    // below: bit masking & shifting operations to pull base-10 values
    // out of the 32-bit instr.
     
    // R-type
    if (id->opcode == 51) {
        
        id->rd = fetch->instr & 3968;
        id->rd = (id->rd >> 7);
        
        id->funct3 = fetch->instr & 28672;
        id->funct3 = (id->funct3 >> 12);
        
        id->rs1 = fetch->instr & 1015808; 
        id->rs1 = (id->rs1 >> 15);
        id->ALU_in1 = core->reg[id->rs1];
        
        id->rs2 = fetch->instr & 32505856; 
        id->rs2 = (id->rs2 >> 20);
        id->ALU_in2 = core->reg[id->rs2];
        
        id->funct7 = fetch->instr & 4261412864;
        id->funct7 = (id->funct7 >> 25);     
    }
    // I-type
    else if (id->opcode == 19 || id->opcode == 3 || id->opcode == 103) {
        
        id->rd = fetch->instr & 3968;
        id->rd = (id->rd >> 7);
        
        id->funct3 = fetch->instr & 28672;
        id->funct3 = (id->funct3 >> 12);
        
        id->rs1 = fetch->instr & 1015808;
        id->rs1 = (id->rs1 >> 15);
        id->ALU_in1 = core->reg[id->rs1];
         
        // I-type imm: [11:0] in instr. bits [31:20]
        id->imm = ImmeGen(fetch->instr);
        // id->ALU_in2 = id->imm;
        // ^^this is done by aluMUX in EX stage
    }
//------------------------------------------------------------------------------- 
//--------------BEGIN COMMENTED OUT BLOCK NOT NEEDED IN PROJECT 4---------------- 
//-------------------------------------------------------------------------------
    /*
    // JAL
    else if (id->opcode == 111) {
        //src_reg_2 not used
        src_reg_2 = 0;
        immed = instruction & 4294963200;
        immed = (immed >> 12);
    }
    // SB-type instr
    else if (id->opcode == 99) {
   
        src_reg_2 = instruction & 32505856;
        src_reg_2 = (src_reg_2 >> 20);
        immed = instruction & 4290772992;
        immed = (immed >> 20); 
//------pretty sure we need to implement immediate correctly!!!   
    }

//--FIX ME?  have we implemented the rd immed for SB and S type codes?
    if (id->opcode == 35) {

        src_reg_2 = instruction & 32505856;
        src_reg_2 = (src_reg_2 >> 20);
    }
    */
//-------------------------------------------------------------------------------
//--------------END COMMENTED OUT BLOCK NOT NEEDED IN PROJECT 4------------------ 
//-------------------------------------------------------------------------------   
        
    return id;
}

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
    else if (input == 3 || input == 19) {
    
        signals->ALUSrc = 1;
        signals->MemtoReg = 1; // WB stage
        signals->RegWrite = 1; // WB stage
        signals->MemRead = 1;  // MEM stage
        signals->MemWrite = 0; // MEM stage
        signals->Branch = 0;
        signals->ALUOp = 0;
    }   
    // SB-type
    else if (input == 99) {
    
        signals->ALUSrc = 0;
        signals->MemtoReg = 0;
        signals->RegWrite = 0;
        signals->MemRead = 0;
        signals->MemWrite = 0;
        signals->Branch = 1;
        signals->ALUOp = 1;
    }
}

Signal ImmeGen(Signal instr) {

    unsigned opcode = instr & 127;
    
    if (opcode == 51) {
        unsigned immed = instr & 4261412864;
        immed = (immed >> 25);
	      return immed;
	  } 
    else if ( opcode == 3 || opcode == 19) {
    
        unsigned immed = instr & 4293918720;
        immed = (immed >> 20);  
        return immed;
    }

    else if (opcode == 99) {
    
        signed immed = instr & 4293918720;
        immed = (immed >> 20);
        return immed;
    }
}
