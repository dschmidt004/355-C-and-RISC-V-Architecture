#include "MEM.h"

MEM *initMEM(ID *id, EX *ex, Core *core) {

    MEM *mem = (MEM *)malloc(sizeof(MEM));

    // Load-type
    mem->memRead = id->signals->MemRead;
    
    if (mem->memRead) { mem->memReadValue = core->mem[ex->ALU_result]; }
    
    // Store type
    mem->memWrite = id->signals->MemWrite;
    
    
    if(id->signals->Branch && ex->zero) { mem->ANDgate = 1; }
    else  { mem->ANDgate = 0; }
    
    return mem;
}
// this was part of proj3, store & old pseudo mem operations - not needed for proj4

//------------------------------------------------------------------------------- 
//--------------BEGIN COMMENTED OUT BLOCK NOT NEEDED IN PROJECT 4---------------- 
//------------------------------------------------------------------------------- 
    /*
    if(id->opcode == 35) {
//------FIX ME SW ACTION GOES HERE   
    }
    if (id->opcode == 3 || id->opcode == 19 || id->opcode == 51) {

	      values[rd] = *ALU_result;
        if(id->opcode == 3) {
	    	
            values[rd] = mem[*ALU_result/8];
	          out[*ALU_result/8]=5;
	      }
    }
//--------------------
        
    if (id->opcode == 99) {
        fprintf(stdout, "\nValue asserted on ALU zero line: %d\n\n", *zero); 
    } 
    else
        fprintf(stdout, "\nResultant value stored in rd, register x%d: %d\n\n",
                rd, values[rd]);
    */
//-------------------------------------------------------------------------------
//--------------END COMMENTED OUT BLOCK NOT NEEDED IN PROJECT 4------------------ 
//-------------------------------------------------------------------------------