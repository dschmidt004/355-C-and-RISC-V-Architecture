#include "Parser.h"
#include <sys/types.h>

void loadInstructions(Instruction_Memory *i_mem, const char *trace) {

    printf("Loading trace file: %s\n", trace);

    FILE *fd = fopen(trace, "r");
    if (fd == NULL) {
    
        perror("Cannot open trace file. \n");
        exit(EXIT_FAILURE);
    }
    // Iterate all the assembly instructions
    char *line = NULL;
    size_t len = 0;
    ssize_t read;

    Addr PC = 0; // program counter points to the zeroth location initially.
    int IMEM_index = 0;
    while ((read = getline(&line, &len, fd)) != -1) {
    
        // Assign program counter
        i_mem->instructions[IMEM_index].addr = PC;
        
        // Extract operation ~> raw_instr == MNEMONIC
        char *raw_instr = strtok(line, " ");
        
        // R-type
        if (strcmp(raw_instr, "add") == 0 ||
            strcmp(raw_instr, "sub") == 0 ||
            strcmp(raw_instr, "sll") == 0 ||
            strcmp(raw_instr, "srl") == 0 ||
            strcmp(raw_instr, "xor") == 0 ||
            strcmp(raw_instr, "or")  == 0)
        {
            parseRType(raw_instr, &(i_mem->instructions[IMEM_index]));
            i_mem->last = &(i_mem->instructions[IMEM_index]);
 	      }
        // I-type
	      else if (strcmp(raw_instr, "ld")   == 0 ||
                 strcmp(raw_instr, "lw")   == 0 ||
                 strcmp(raw_instr, "addi") == 0 ||
                 strcmp(raw_instr, "slli") == 0 ||
                 strcmp(raw_instr, "jalr") == 0)
        {
            parseIType(raw_instr, &(i_mem->instructions[IMEM_index]));
            i_mem->last = &(i_mem->instructions[IMEM_index]);
        }
        /*
	      // SB-type
	      else if (strcmp(raw_instr, "bne")  == 0)
        {
            parseSBType(raw_instr, &(i_mem->instructions[IMEM_index]));
            i_mem->last = &(i_mem->instructions[IMEM_index]);
        }
        // SB-type
        else if (strcmp(raw_instr, "beq")  == 0)
        {
            parseSBType(raw_instr, &(i_mem->instructions[IMEM_index]));
            i_mem->last = &(i_mem->instructions[IMEM_index]);
        }
        // UJ-type
        else if (strcmp(raw_instr, "jal")  == 0)
        {
            parseUJType(raw_instr, &(i_mem->instructions[IMEM_index]));
            i_mem->last = &(i_mem->instructions[IMEM_index]);
        }
        // I-type (sw)
	      else if (strcmp(raw_instr, "sw")  == 0)
        {
            parseSType(raw_instr, &(i_mem->instructions[IMEM_index]));
            i_mem->last = &(i_mem->instructions[IMEM_index]);
        }  
        */      
	      IMEM_index++;
        PC += 4;
    }
    
    fclose(fd);
}

void parseRType(char *opr, Instruction *instr) {

    instr->instruction = 0;
    unsigned opcode = 0;
    unsigned funct3 = 0;
    unsigned funct7 = 0;

    if (strcmp(opr, "add") == 0) {
    
        opcode = 51;  // 0b0110011
        funct3 = 0;   // 0b000
        funct7 = 0;   // 0b0000000
    }
    else if (strcmp(opr, "sub") == 0) {
    
        opcode = 51;   // 0b0110011
        funct3 = 0;    // 0b000
        funct7 = 32;   // 0b0100000
    }    
    else if (strcmp(opr, "sll") == 0) {
    
        opcode = 51;   // 0b0110011
        funct3 = 1;    // 0b001
        funct7 = 0;    // 0b0000000
    }

    // parse rd, rs1, rs2 fields (R-type)
    fprintf(stdout, "\nR-type instr.: %s\n", opr);
        
    char *reg = strtok(NULL, ", ");
    unsigned rd = regIndex(reg);  //-------------------assign rd - CHECK
    //
    fprintf(stdout, "\ndest. reg: %s\n", reg);
    fprintf(stdout, "rd: %d\n", rd);

    reg = strtok(NULL, ", ");
    unsigned rs_1 = regIndex(reg);  //----------------assign rs1 - CHECK
    //
    fprintf(stdout, "source reg #1: %s\n", reg);
    fprintf(stdout, "rs1: %d\n", rs_1);


    reg = strtok(NULL, ", ");

    /* NOTE: the trace files MUST have either \n\r  or \n at the
     *       end of each line ~> i.e. make sure return is hit after 
     *       the last inst. s.t. there's an empty line at the end */
    int size = strlen(reg);
    // fprintf(stdout, "\nstrlen(reg) = %d", size);
    
    // debugging to figure out where to put "\0" to get rs2
    // fprintf(stdout, "\nASCII char at reg[size-0]: %d", reg[size-0]);
    // fprintf(stdout, "\nASCII char at reg[size-1]: %d", reg[size-1]);
    // fprintf(stdout, "\nASCII char at reg[size-2]: %d", reg[size-2]);
    // fprintf(stdout, "\nASCII char at reg[size-3]: %d\n", reg[size-3]);

    // general case fix for setting null char location:
    // str either ends with ...\r\n\0 OR ...\n\0 ~> \r == ASCII decimal value 13
    if (reg[size-2] == 13) { reg[size-2] = '\0'; }
    else { reg[size-1] = '\0'; }
    unsigned rs_2 = regIndex(reg);  //----------------assign rs2 - CHECK
    //
    fprintf(stdout, "source reg #2: %s\n", reg);
    fprintf(stdout, "rs2: %d\n", rs_2);
    fprintf(stdout, "-------------------------\n");
    
    // Contruct instruction
    instr->instruction |= opcode;
    instr->instruction |= (rd << 7);
    instr->instruction |= (funct3 << (7 + 5));
    instr->instruction |= (rs_1 << (7 + 5 + 3));
    instr->instruction |= (rs_2 << (7 + 5 + 3 + 5));
    instr->instruction |= (funct7 << (7 + 5 + 3 + 5 + 5));
}

void parseIType(char *opr, Instruction *instr)
{

    instr->instruction = 0;
    unsigned opcode = 0;
    unsigned funct3 = 0;
    
    if (strcmp(opr, "ld") == 0) {
    
        opcode = 3;
        funct3 = 3;	   
    }
    else if (strcmp(opr, "addi") == 0) {
    
        opcode = 19;
        funct3 = 0;
    }
    else if (strcmp(opr, "slli") == 0) {
    
        opcode = 19;
        funct3 = 1;
    }
    else if (strcmp(opr, "jalr") == 0) {

        opcode = 103;
        funct3 = 0;
    }
    else if (strcmp(opr, "lw") == 0) {

        opcode = 3;
        funct3 = 2;
    }

    char *reg = strtok(NULL, ", ");
    unsigned rd = regIndex(reg);
    // ** rd assignment is CORRECT **

    unsigned rs_1;
    unsigned immed = 0;
    
//--TODO ~> strtok doing the right thing?! ~> ld is FIXED
    
    if (strcmp(opr, "ld") == 0) {
    
        fprintf(stdout, "\nI-type instr.: %s\n", opr);
        fprintf(stdout, "\ndest. reg: %s", reg);
                               
        // this line gets <<<imm(xreg)>>>
        reg = strtok(NULL, ", ");
        fprintf(stdout, "\nsource for ld: %s", reg);
        
        // this **CORRECTLY** gets imm ~> offset == (str)imm
        char *offset = strtok(reg, "(x");
        fprintf(stdout, "imm: %s\n", offset);
        
        reg = strtok(NULL, "(");
        // fprintf(stdout, "\nnew reg: %s\n", reg);
                
        int size = strlen(reg); //Total size of string
        // fprintf(stdout, "\nstrlen = %d\n\n", size);
        
        // debugging to figure out where to put "\0" to get imm
        // fprintf(stdout, "\nstrlen(reg) = %d", size);
        // fprintf(stdout, "\nASCII char at reg[size-0]: %d", reg[size-0]);
        // fprintf(stdout, "\nASCII char at reg[size-1]: %d", reg[size-1]);
        // fprintf(stdout, "\nASCII char at reg[size-2]: %d", reg[size-2]);
        // fprintf(stdout, "\nASCII char at reg[size-3]: %d\n", reg[size-3]);
        
        // general case fix for setting null char location:
        // str either ends with ..)\r\n\0 *OR* ...)\n\0 
        // ~> \r == ASCII decimal value 13
        if (reg[size-2] == 13) { reg[size-3] = '\0'; }
        else { reg[size-2] = '\0'; }        
        fprintf(stdout, "source reg: %s", reg);
        fprintf(stdout, "\nrs1: %d\n", regIndex(reg));
        fprintf(stdout, "-------------------------\n");
        
        // **THESE ARE NOW CORRECT**
	      rs_1 = regIndex(reg);
	      immed = atoi(offset);
    }
    else if (strcmp(opr, "lw") == 0) {

        reg = strtok(NULL, ", ");
        char *offset = strtok(reg, "(x");
        reg = strtok(NULL, "(");
        int size = strlen(reg);
        reg[size-3] = '\0';
	      // printf("\n\n reg = %s ",reg);
        rs_1 = regIndex(reg);

        immed = atoi(offset);
    }
    else if (strcmp(opr, "jalr") == 0) {
    
        reg = strtok(NULL, ", ");           // reg = 0(x1)
        char *offset = strtok(reg, "(x");
        reg = strtok(NULL, ")");            // reg = x1
        int size = strlen(reg);
        reg[size] = '\0';

        rs_1 = regIndex(reg);
        immed = atoi(offset);
        fprintf(stdout, "\n this is jalr immed %d \n",immed);
        fprintf(stdout, "\n this is jalr rs1 %d \n",rs_1);
        fprintf(stdout, "\n this is jalr rd %d \n",rd);
    }
    else if (strcmp(opr, "slli") == 0 || strcmp(opr, "addi") == 0) {
        
        reg = strtok(NULL, ", ");
        // unsigned rs_1 = regIndex(reg);
        //printf("\n\n%s\n\n",reg);
        rs_1 = regIndex(reg);

        reg  = strtok(NULL, ", ");
        // debug
        // printf("\n\n%s\n\n",reg);
	      int x = atoi(reg);		
	      immed = immed + x;
    }
    
    instr->instruction |= opcode;
    instr->instruction |= (rd << 7);
    instr->instruction |= (funct3 << (7 + 5));
    instr->instruction |= (rs_1 << (7 + 5 + 3));
    instr->instruction |= (immed << (7 + 5 + 3 + 5 ));
}

/**/

void parseSBType(char *opr, Instruction *instr)
{
    // in SB-type rs_1 is compared to rs_2 
    // & jumps to relative address wich is 
    // PC addr. + (immed << 1)       

    instr->instruction = 0;
    unsigned opcode = 0;
    unsigned funct3 = 0;
    
    if (strcmp(opr, "bne") == 0) {
        opcode = 99;
        funct3 = 1;
	  } 
    else if (strcmp(opr, "beq") == 0) {
        opcode = 99;
        funct3 = 0;
	  } 
    
    // parse rs_1 out of instr
    char *reg = strtok(NULL, ", ");
    unsigned rs_1 = regIndex(reg);
    // fprintf(stdout, "\n\nreg (string) = %s\n\n", reg);
    // fprintf(stdout, "\n\nrs_2 (unsigned int) = %d\n\n", rs_1);
    
    // parse rs_2 out of instr
    reg = strtok(NULL, ", ");
    unsigned rs_2 = regIndex(reg);
    // fprintf(stdout, "\n\nreg (string) = %s\n\n", reg);
    // fprintf(stdout, "\n\nrs_1 (unsigned int) = %d\n\n", rs_2); 
    
    // parse immed out of instr
    reg = strtok(NULL, ", ");
    signed int immed = atoi(reg);
    // fprintf(stdout, "\n\nreg (string) = %s\n\n", reg);
    // fprintf(stdout, "\n\nimmed (signed int) = %d\n\n", immed);
    
    // parse immed to get the 4 required fields
    unsigned imm11, imm4_1, imm10_5, imm12;
    
    // bit masking operation to seperate fields
    imm11 = immed & 2048;
    imm11 = (imm11 >> 11);

    imm4_1 = immed & 30;
    imm4_1 = (imm4_1 >> 1);

    imm10_5 = immed & 2016;
    imm10_5 = (imm10_5 >> 5);

    imm12 = immed & 4096;
    imm12 = (imm12 >> 12);

    instr->instruction |= opcode;
    instr->instruction |= (imm11 << 7);
    instr->instruction |= (imm4_1 << (7 + 1));
    instr->instruction |= (funct3 << (7 + 1 + 4));
    instr->instruction |= (rs_1 << (7 + 1 + 4 + 3));
    instr->instruction |= (rs_2 << (7 + 1 + 4 + 3 + 5));
    instr->instruction |= (imm10_5 << (7 + 1 + 4 + 3 + 5 + 5));
    instr->instruction |= (imm12 << (7 + 1 + 4 + 3 + 5 + 5 + 6));
}

void parseUJType(char *opr, Instruction *instr) 
{
    instr->instruction = 0;
    unsigned opcode = 0;

    if (strcmp(opr, "jal") == 0) { opcode = 111; }

    char *reg = strtok(NULL, ", ");
    // debug
    // printf("\n\n%s\n\n",reg);
    unsigned rd = regIndex(reg);

    // parse immed out of instr
    reg = strtok(NULL, ", ");
    signed int immed = atoi(reg);
    // fprintf(stdout, "\n\nreg (string) = %s\n\n", reg);

    // parse immed to get the 4 required fields
    unsigned imm20, imm10_1, imm11, imm19_12; 

    // bit masking operation to seperate fields
    // not sure how this works
    
    imm10_1 = immed & 1023;
    imm10_1 = (imm10_1 >> 1);    
 
    imm11 = immed & 1024;
    imm11 = (imm11 >> 11);

    imm19_12 = immed & 522240;
    imm19_12 = (imm11 >> 11);
    
    imm20 = immed & 524288;
    imm20 = (imm20 >> 19);
    
    // Contruct instruction
    instr->instruction |= opcode;
    instr->instruction |= (rd << 7);
    instr->instruction |= (imm19_12 << 7 + 5); 
    instr->instruction |= (imm11 << 7 + 5 + 8); 
    instr->instruction |= (imm10_1 << 7 + 5 + 8 + 1); 
    instr->instruction |= (imm20 << 7 + 5 + 8 + 1 + 10); 
}

void parseSType(char *opr, Instruction *instr) {
     
    instr->instruction = 0;
    unsigned opcode = 0;
    unsigned funct3 = 0;
    unsigned immed;
    char *reg = NULL; 
    reg = strtok(NULL, ", ");
    unsigned src = regIndex(reg);
    
    unsigned rs_1; 
    if (strcmp(opr, "sw") == 0) {
        opcode = 35;
        funct3 = 2;
    }
    if (strcmp(opr, "sw") == 0) {
        immed = 0;
        //unsigned rs_1=10;
        reg = strtok(NULL, ", ");
        char *offset = strtok(reg, "(x");
        reg = strtok(NULL, "(");
        int size = strlen(reg); //Total size of string
        // printf("\nstrlen = %d\n\n",size);
        reg[size-3] = '\0';
        // unsigned rs_1 = regIndex(reg);
        // printf("\n\n%s\n%s\n\n",reg, offset);
	      rs_1 = regIndex(reg);

        immed = atoi(offset);
        printf("\n\n reg = %s \n\n",reg);
    }
    unsigned immed4_0 = immed & 31;
    unsigned immed11_5 = immed & 4080;
    immed11_5 = (immed11_5 >> 4);    

    
    instr->instruction |= opcode;
    instr->instruction |= (immed4_0 << 7);
    instr->instruction |= (funct3 << 7 + 5);
    instr->instruction |= (rs_1 << 7 + 5 + 3);
    instr->instruction |= (src << 7 + 5 + 3 + 5);
    instr->instruction |= (immed11_5 << 7 + 5 + 3 + 5 + 5);
}

int regIndex(char *reg) {

    unsigned i = 0;
    for (i; i < NUM_OF_REGS; i++) {
    
        if (strcmp(REGISTER_NAME[i], reg) == 0) { break; }
    }
    return i;
} 
