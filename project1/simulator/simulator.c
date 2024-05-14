/* LC-2K Instruction-level simulator */

#include <stdlib.h>
#include <stdio.h>
#include <string.h>

#define NUMMEMORY 65536 /* maximum number of words in memory */
#define NUMREGS 8 /* number of machine registers */
#define MAXLINELENGTH 1000 
typedef struct stateStruct {
    int pc;
    int mem[NUMMEMORY];
    int reg[NUMREGS];
    int numMemory;
} stateType;

void printState(stateType *);
int convertNum(int num);

int main(int argc, char *argv[])
{
    char line[MAXLINELENGTH];
    stateType state;
    FILE *filePtr;

    if (argc != 2) {
        printf("error: usage: %s <machine-code file>\n", argv[0]);
        exit(1);
    }

    filePtr = fopen(argv[1], "r");
    if (filePtr == NULL) {
        printf("error: can't open file %s", argv[1]);
        perror("fopen");
        exit(1);
    }

    /* read in the entire machine-code file into memory */
    for (state.numMemory = 0; fgets(line, MAXLINELENGTH, filePtr) != NULL;
            state.numMemory++) {

        if (sscanf(line, "%d", state.mem+state.numMemory) != 1) {
            printf("error in reading address %d\n", state.numMemory);
            exit(1);
        }
        printf("memory[%d]=%d\n", state.numMemory, state.mem[state.numMemory]);
    }

		/* TODO: */
    state.pc = 0;
    int incount = 0;
    int opcode, reg0, reg1, reg2, offset;
    for(int i = 0; i < 8; i++){
        state.reg[i] = 0;
    }
    while(1){ // opcode가 halt가 아니면
        opcode = (state.mem[state.pc]>>22)&7;
        reg0 = (state.mem[state.pc]>>19)&7;
        reg1 = (state.mem[state.pc]>>16)&7;

        //printf("%d,%d,%d\n",opcode ,reg0,reg1);
        printState(&state);
        if (opcode == 0){ // add op
            reg2 = state.mem[state.pc]&7;
            state.reg[reg2] = state.reg[reg0] + state.reg[reg1];
        }
        else if (opcode == 1){ // nor op
            reg2 = state.mem[state.pc]&7;
            state.reg[reg2] = ~(state.reg[reg0]|state.reg[reg1]);
        }
        else if (opcode == 2){ // lw op
            offset = 65535&state.mem[state.pc];
            state.reg[reg1] = state.mem[state.reg[reg0] + offset];
            /*for(int i = 0; i < 8; i++){
                printf("[%d]%d\n",i, state.reg[i]);
            }
            printf("\n");*/
        }
        else if (opcode == 3){ // sw op
            offset = 65535&state.mem[state.pc];
            state.mem[state.reg[reg0] + offset] = state.reg[reg1];
        }
        else if (opcode == 4){ // beq op
            offset = 65535&state.mem[state.pc];
            if(state.reg[reg0] == state.reg[reg1]){
                state.pc = (state.pc + 1 + offset)&65535;
                incount++;
                continue;
            }
        }
        else if (opcode == 5){ // jalr op
            state.reg[reg1] = state.pc + 1;
            state.pc = state.reg[reg0];
            incount++;
            continue;
        }

        else if (opcode == 6){ // halt op
            state.pc++;
            incount++;
            printf("machine halted\n");
            printf("total of %d instructions executed\n", incount);
            printf("final state of machine:\n\n");
            break;
        }
        
        state.pc++;
        incount++;
        
    }
    printState(&state);
    return(0);
}

void printState(stateType *statePtr)
{
    int i;
    printf("\n@@@\nstate:\n");
    printf("\tpc %d\n", statePtr->pc);
    printf("\tmemory:\n");
    for (i = 0; i < statePtr->numMemory; i++) {
        printf("\t\tmem[ %d ] %d\n", i, statePtr->mem[i]);
    }
    printf("\tregisters:\n");
    for (i = 0; i < NUMREGS; i++) {
        printf("\t\treg[ %d ] %d\n", i, statePtr->reg[i]);
    }
    printf("end state\n");
}

int convertNum(int num)
{
	/* convert a 16-bit number into a 32-bit Linux integer */
	if (num & (1 << 15)) {
		num -= (1 << 16);
	}
	return (num);
}
