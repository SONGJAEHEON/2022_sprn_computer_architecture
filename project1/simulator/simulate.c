/* LC-2K Instruction-level simulator */

#include <stdlib.h>
#include <stdio.h>
#include <string.h>

#define NUMMEMORY 65536 /* maximum number of words in memory */
#define NUMREGS 8 /* number of machine registers */
#define MAXLINELENGTH 1000

int instructionCnt = 0;

typedef struct stateStruct {
    int pc;
    int mem[NUMMEMORY];
    int reg[NUMREGS];
    int numMemory;
} stateType;

void printState(stateType *);
int runInstructions(stateType*);
int convertNum(int);

int main(int argc, char *argv[]){
    char line[MAXLINELENGTH];
    stateType state;
    stateType* statePtr;
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
    statePtr = &state;
    for(int i = 0; i < NUMREGS; i++){
        statePtr->reg[i] = 0;
    }
    /* read in the entire machine-code file into memory */
    for (state.numMemory = 0; fgets(line, MAXLINELENGTH, filePtr) != NULL; state.numMemory++) {
        if (sscanf(line, "%d", state.mem+state.numMemory) != 1) {
            printf("error in reading address %d\n", state.numMemory);
            exit(1);
        }
        printf("memory[%d]=%d\n", state.numMemory, state.mem[state.numMemory]);
    }
    int rtn = 1;
    while(rtn == 1){
        printState(statePtr);
        rtn = runInstructions(statePtr);
    }
    if(rtn == 0){
        printf("machine halted\n");
        printf("total of %d instructions executed\n", instructionCnt);
        printf("final state of machine:\n");
        printState(statePtr);
    }else if(rtn == -1){
        printf("error with the instruction not defined!\n");
        exit(-1);
    }
    return(0);
}

int runInstructions(stateType* statePtr){
    int opcodeMask = 7 << 22;
    int regAMask = 7 << 19;
    int regBMask = 7 << 16;
    int regDMask = 7;
    int offsetMask = 1;
    int halt = 0;
    for(int i = 0; i < 15; i++){
        offsetMask = (offsetMask << 1) + 1;
    }
    int instruction = statePtr->mem[statePtr->pc];
    int opcode = (instruction & opcodeMask) >> 22;
    int regA = 0, regB = 0, regD = 0, offset = 0;
    switch(opcode){
        case 0: // R-add
            regA = (instruction & regAMask) >> 19;
            regB = (instruction & regBMask) >> 16;
            regD = (instruction & regDMask);
            statePtr->reg[regD] = statePtr->reg[regA] + statePtr->reg[regB];
            break;
        case 1: // R-nor
            regA = (instruction & regAMask) >> 19;
            regB = (instruction & regBMask) >> 16;
            regD = (instruction & regDMask);
            statePtr->reg[regD] = ~(statePtr->reg[regA] | statePtr->reg[regB]);
            break;
        case 2: // I-lw
            regA = (instruction & regAMask) >> 19;
            regB = (instruction & regBMask) >> 16;
            offset = convertNum(instruction & offsetMask);
            statePtr->reg[regB] = statePtr->mem[statePtr->reg[regA]+offset];
            break;
        case 3: // I-sw
            regA = (instruction & regAMask) >> 19;
            regB = (instruction & regBMask) >> 16;
            offset = convertNum(instruction & offsetMask);
            statePtr->mem[statePtr->reg[regA]+offset] = statePtr->reg[regB];
            break;
        case 4: // I-beq
            regA = (instruction & regAMask) >> 19;
            regB = (instruction & regBMask) >> 16;
            if(statePtr->reg[regA] == statePtr->reg[regB]){
                offset = convertNum(instruction & offsetMask);
                statePtr->pc = statePtr->pc + offset; // 디폴트 pc 증가
                // statePtr->pc = statePtr->pc + 1 + offset;
            }
            break;
        case 5: // J-jalr
            regA = (instruction & regAMask) >> 19;
            regB = (instruction & regBMask) >> 16;
            offset = convertNum(instruction & offsetMask);
            statePtr->reg[regB] = statePtr->pc + 1;
            statePtr->pc = statePtr->reg[regA] - 1;
            break;
        case 6: // O-halt
            halt = 1;
            break;
        case 7: // O-noop
            break;
        default:
            // not defined instruction
            return -1;
    }
    ++statePtr->pc;
    ++instructionCnt;
    if(halt)
        return 0;

    return 1;
}

void printState(stateType *statePtr){
    int i;
    printf("\n@@@\nstate:\n");
    printf("\tpc %d\n", statePtr->pc);
    printf("\tmemory:\n");
    for (i=0; i<statePtr->numMemory; i++) {
        printf("\t\tmem[ %d ] %d\n", i, statePtr->mem[i]);
    }
    printf("\tregisters:\n");
    for (i=0; i<NUMREGS; i++) {
        printf("\t\treg[ %d ] %d\n", i, statePtr->reg[i]);
    }
    printf("end state\n");
}

int convertNum(int num){
    // convert a 16-bit number into a 32-bit Linux integer
    if(num & (1<<15)){
       num -= (1<<16); 
    }
    return num;
}