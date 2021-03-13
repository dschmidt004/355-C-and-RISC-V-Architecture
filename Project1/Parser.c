#include "Parser.h"
#include <sys/types.h>
// FIXME, implement this function.
// Here shows an example on how to translate "add x10, x10, x25"
void loadInstructions(Instruction_Memory *i_mem, const char *trace)
{
    printf("Loading trace file: %s\n", trace);

    FILE *fd = fopen(trace, "r");
    if (fd == NULL)
    {
        perror("Cannot open trace file. \n");
        exit(EXIT_FAILURE);
    }

    // Iterate all the assembly instructions
    char *line = NULL;
    size_t len = 0;
    ssize_t read;

    Addr PC = 0; // program counter points to the zeroth location initially.
    int IMEM_index = 0;
    while ((read = getline(&line, &len, fd)) != -1)
    {
        // Assign program counter
        i_mem->instructions[IMEM_index].addr = PC;
        
        // Extract operation ~> raw_instr == MNEMONIC
        char *raw_instr = strtok(line, " ");
        //printf("\n\n%s\n\n",raw_instr);
        
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

	      if (strcmp(raw_instr, "ld")    == 0 ||
            strcmp(raw_instr, "addi")  == 0 ||
            strcmp(raw_instr, "slli")  == 0)

        {
            parseIType(raw_instr, &(i_mem->instructions[IMEM_index]));
            i_mem->last = &(i_mem->instructions[IMEM_index]);
        }
	
	      if (strcmp(raw_instr, "bne")  == 0)

        {
            parseBType(raw_instr, &(i_mem->instructions[IMEM_index]));
            i_mem->last = &(i_mem->instructions[IMEM_index]);
        }

	
        IMEM_index++;
        PC += 4;
    }

    fclose(fd);
}

void parseRType(char *opr, Instruction *instr)
{
    instr->instruction = 0;
    unsigned opcode = 0;
    unsigned funct3 = 0;
    unsigned funct7 = 0;

    if (strcmp(opr, "add") == 0)
    {
        opcode = 51;
        funct3 = 0;
        funct7 = 0;
    }

    char *reg = strtok(NULL, ", ");
    // debug
    // printf("\n\n%s\n\n",reg);
    unsigned rd = regIndex(reg);

    reg = strtok(NULL, ", ");
    // debug
    // printf("\n\n%s\n\n",reg);
    unsigned rs_1 = regIndex(reg);

    reg = strtok(NULL, ", ");
    // debug
    // printf("\n\n%s\n\n",reg); ~> same output as next printf
    reg[strlen(reg)-1] = '\0'; // add NULL char to make it a string ???
    // debug
    // printf("\n\n%s\n\n",reg);
    unsigned rs_2 = regIndex(reg);

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
    unsigned funct7 = 0;

    if (strcmp(opr, "ld") == 0)
    {
        opcode = 3;
        funct3 = 3;
        funct7 = 0;
     	   
    }
    if (strcmp(opr, "addi") == 0)
    {
        opcode = 19;
        funct3 = 0;
        funct7 = 0;
        
    }
    if (strcmp(opr, "slli") == 0)
    {
        opcode = 19;
        funct3 = 1;
        funct7 = 0;
        
    }


    char *reg = NULL; 
    reg = strtok(NULL, ", ");
    unsigned rd = regIndex(reg);


    // reg = strtok(NULL, ", ");
    // unsigned rs_1 = regIndex(reg);
    unsigned rs_1;
    
    if (strcmp(opr, "ld") == 0)
    {
        // unsigned rs_1=10;
        rs_1=10;
        reg = strtok(NULL, ", ");
        char *offset = strtok(reg, "(x");
        reg = strtok(NULL, "(");
        int size = strlen(reg); // total size of string
        // printf("\nstrlen = %d\n\n",size); // string has an extra char
        reg[size-2] = '\0';
        // unsigned rs_1 = regIndex(reg);
        // printf("\n\n%s\n%s\n\n",reg, offset);
        rs_1 = regIndex(reg);
        funct7 = atoi(offset);

    }
 
    if (strcmp(opr, "slli") == 0 || strcmp(opr, "addi") == 0)
    {
        
        reg = strtok(NULL, ", ");
        // unsigned rs_1 = regIndex(reg);
        //printf("\n\n%s\n\n",reg);
        rs_1 = regIndex(reg);

        reg  = strtok(NULL, ", ");
        // debug
        // printf("\n\n%s\n\n",reg);
	      int x = atoi(reg);		
	      funct7= funct7 + x;
    }
    
    instr->instruction |= opcode;
    instr->instruction |= (rd << 7);
    instr->instruction |= (funct3 << (7 + 5));
    instr->instruction |= (rs_1 << (7 + 5 + 3));
    instr->instruction |= (funct7 << (7 + 5 + 3 + 5 ));
}



void parseBType(char *opr, Instruction *instr)
{
    instr->instruction = 0;
    unsigned opcode = 0;
    unsigned funct3 = 0;
    unsigned funct7 = 0;
    unsigned im=25;

    if (strcmp(opr, "bne") == 0)
    {
        opcode = 99;
        funct3 = 1;
        funct7 = 4092;
	}

    char *reg = strtok(NULL, ", ");
    unsigned rd = regIndex(reg);

    reg = strtok(NULL, ", ");
    unsigned rs_1 = regIndex(reg);

    reg = strtok(NULL, ", ");
    int y = atoi(reg);
    //printf("%d \n",y);
    //reg[strlen(reg)-1] = '\0';
    //unsigned rs_2 = regIndex(reg);
    unsigned rs_2 = y;   
    instr->instruction |= opcode;
    instr->instruction |= (im << 7);
    instr->instruction |= (funct3 << (7 + 5));
    instr->instruction |= (rd << (7 + 5 + 3));
    instr->instruction |= (rs_2 << (7 + 5 + 3+5));
}
int regIndex(char *reg)
{
    unsigned i = 0;
    for (i; i < NUM_OF_REGS; i++)
    {
        if (strcmp(REGISTER_NAME[i], reg) == 0)
        {
            break;
        }
    }

    return i;
} 
