/* LC-2K Instruction-level simulator */

#include <stdlib.h>
#include <stdio.h>
#include <string.h>

#define NUMMEMORY 65536 /* maximum number of data words in memory */
#define NUMREGS 8 /* number of machine registers */
#define MAXLINELENGTH 1000

#define ADD 0
#define NOR 1
#define LW 2
#define SW 3
#define BEQ 4
#define JALR 5 /* JALR will not implemented for this project */
#define HALT 6
#define NOOP 7

#define NOOPINSTRUCTION 0x1c00000

typedef struct IFIDStruct {
    int instr;
    int pcPlus1;
} IFIDType;

typedef struct IDEXStruct {
    int instr;
    int pcPlus1;
    int readRegA;
    int readRegB;
    int offset;
} IDEXType;

typedef struct EXMEMStruct {
    int instr;
    int branchTarget;
    int aluResult;
    int readRegB;
} EXMEMType;

typedef struct MEMWBStruct {
    int instr;
    int writeData;
} MEMWBType;

typedef struct WBENDStruct {
    int instr;
    int writeData;
} WBENDType;

typedef struct stateStruct {
    int pc;
    int instrMem[NUMMEMORY];
    int dataMem[NUMMEMORY];
    int reg[NUMREGS];
    int numMemory;

    IFIDType IFID;
    IDEXType IDEX;
    EXMEMType EXMEM;
    MEMWBType MEMWB;
    WBENDType WBEND;

    int cycles; /* number of cycles run so far */
} stateType;

void printState(stateType*);
int field0(int);
int field1(int);
int field2(int);
int opcode(int);
void printInstruction(int);
int convertNum(int);

int main(int argc, char* argv[]){
    char line[MAXLINELENGTH];
    stateType state;
    stateType newState;
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
        if(state.numMemory >= NUMMEMORY){
            printf("error: too many instructions\n");
            exit(1);
        }
        if (sscanf(line, "%d", state.instrMem+state.numMemory) != 1) {
            printf("error in reading address %d\n", state.numMemory);
            exit(1);
        }
        // state.dataMemstate.numMemory = state.instrMem+state.numMemory;
        state.dataMem[state.numMemory] = state.instrMem[state.numMemory];
        printf("memory[%d]=%d\n", state.numMemory, state.instrMem[state.numMemory]);
    }

    printf("%d memory words\n", state.numMemory);
    printf("\tinstruction memory:\n");
    for(int i = 0; i < statePtr->numMemory; i++){
        printf("\t\tinstrMem[%d] ",i);
        printInstruction(statePtr->instrMem[i]);
    }

    state.pc = 0;
    state.cycles = 0;
    for(int i = 0; i < NUMREGS; i++){
        state.reg[i] = 0;
    }
    state.IFID.instr = NOOPINSTRUCTION;
    state.IDEX.instr = NOOPINSTRUCTION;
    state.EXMEM.instr = NOOPINSTRUCTION;
    state.MEMWB.instr = NOOPINSTRUCTION;
    state.WBEND.instr = NOOPINSTRUCTION;


    int opCode = -1;
    int dest = 0;
    int branchFlag = 0;

    while (1) {
        printState(&state);
        /* check for halt */
        if (opcode(state.MEMWB.instr) == HALT) {
        printf("machine halted\n");
        printf("total of %d cycles executed\n", state.cycles);
        exit(0);
        }
        newState = state;
        newState.cycles++;
        
        IFIDType IFoutput;
        IFIDType IDinput;
        IDEXType IDoutput;
        IDEXType EXinput;
        EXMEMType EXoutput;
        EXMEMType MEMinput;
        MEMWBType MEMoutput;
        MEMWBType WBinput;
        WBENDType WBoutput;

        /* --------------------- IF stage --------------------- */
        IFoutput.instr = state.instrMem[state.pc];
        IFoutput.pcPlus1 = state.pc+1;

        newState.pc = IFoutput.pcPlus1;
        newState.IFID = IFoutput;
        /* --------------------- ID stage --------------------- */
        IDinput = state.IFID;
        IDoutput.instr = IDinput.instr;
        IDoutput.pcPlus1 = IDinput.pcPlus1;
        IDoutput.readRegA = state.reg[field0(IDinput.instr)];
        IDoutput.readRegB = state.reg[field1(IDinput.instr)];
        IDoutput.offset = field2(IDinput.instr);
        IDoutput.offset = convertNum(IDoutput.offset);

        int regA = field0(IDoutput.instr);
        int regB = field1(IDoutput.instr);

        newState.IDEX = IDoutput;
        /* --------------------- EX stage --------------------- */
        EXinput = state.IDEX;

        if(branchFlag){
            EXoutput.aluResult = 0;
            branchFlag = 0;
        }

        EXoutput.instr = EXinput.instr;
        int EXRegA = EXinput.readRegA;
        int EXRegB = EXinput.readRegB;
        int EXOffset = EXinput.offset;
        EXoutput.readRegB = EXRegB;
        EXoutput.branchTarget = EXinput.pcPlus1 + EXOffset;

        opCode = opcode(state.IDEX.instr);
        switch(opCode){
            case ADD: // 0 for ADD. R-type
                EXoutput.aluResult = EXRegA + EXRegB;
                break;
            case NOR: // 1 for ADD. R-type
                EXoutput.aluResult = ~(EXRegA | EXRegB);
                break;
            case LW: // 2 for LW. I-type
            case SW: // 3 for LW. I-type
                EXoutput.aluResult = EXRegA + EXOffset;
                break;
            case BEQ: // 4 for BEQ. I-type
                EXoutput.aluResult = EXRegA - EXRegB;
                break;
            case JALR: // 5 for JALR. J-type
            case HALT: // 6 for HALT. O-type
            case NOOP: // 7 for NOOP. O-type
                break;
            default:
                // not defined instruction
                exit(1);
                break;
        }
        newState.EXMEM = EXoutput;

        /* --------------------- MEM stage --------------------- */
        MEMinput = state.EXMEM;
        MEMoutput.instr = MEMinput.instr;
        opCode = opcode(MEMinput.instr);

        if(opCode == ADD || opCode == NOR){ // 0 for ADD, 1 for NOR. R-type
            MEMoutput.writeData = MEMinput.aluResult;
        }else if(opCode == LW){ // 2 for LW. I-type
            MEMoutput.writeData = state.dataMem[MEMinput.aluResult];
        }else if(opCode == SW){ // 3 for SW. I-type
            newState.dataMem[MEMinput.aluResult] = MEMinput.readRegB;
        }
        newState.MEMWB = MEMoutput;
        if(opCode == BEQ && state.EXMEM.aluResult == 0){ // 4 for BEQ. I-type
            newState.IFID.instr = NOOPINSTRUCTION;
            newState.IFID.pcPlus1 = 0;
            newState.IDEX.instr = NOOPINSTRUCTION;
            newState.IDEX.pcPlus1 = newState.IDEX.readRegA = newState.IDEX.readRegB = newState.IDEX.offset = 0;
            newState.EXMEM.instr = NOOPINSTRUCTION;
            newState.EXMEM.branchTarget = newState.EXMEM.aluResult = newState.EXMEM.readRegB = 0;
            newState.pc = state.EXMEM.branchTarget;
            branchFlag = 1;
        }
        /* --------------------- WB stage --------------------- */
        WBinput = state.MEMWB;
        WBoutput.instr = WBinput.instr;
        WBoutput.writeData = WBinput.writeData;
        opCode = opcode(WBinput.instr);
        if(opCode == ADD || opCode == NOR){ // 0 for ADD, 1 for NOR. R-type
            dest = field2(WBinput.instr);
            newState.reg[dest] = WBinput.writeData;
        }else if(opCode == LW){ // 2 for LW. I-type
            dest = field1(WBinput.instr);
            newState.reg[dest] = WBinput.writeData;
        }
        newState.WBEND = WBoutput;

        state = newState; /* this is the last statement before end of the loop.
        It marks the end of the cycle and updates the
        current state with the values calculated in this
        cycle */
    }
    return 0;
}

void printState(stateType *statePtr){
    int i;
    printf("\n@@@\nstate before cycle %d starts\n", statePtr->cycles);
    printf("\tpc %d\n", statePtr->pc);
    printf("\tdata memory:\n");
    for (i=0; i<statePtr->numMemory; i++) {
        printf("\t\tdataMem[ %d ] %d\n", i, statePtr->dataMem[i]);
    }
    printf("\tregisters:\n");
    for (i=0; i<NUMREGS; i++) {
        printf("\t\treg[ %d ] %d\n", i, statePtr->reg[i]);
    }
    printf("\tIFID:\n");
    printf("\t\tinstruction ");
    printInstruction(statePtr->IFID.instr);
    printf("\t\tpcPlus1 %d\n", statePtr->IFID.pcPlus1);
    printf("\tIDEX:\n");
    printf("\t\tinstruction ");
    printInstruction(statePtr->IDEX.instr);
    printf("\t\tpcPlus1 %d\n", statePtr->IDEX.pcPlus1);
    printf("\t\treadRegA %d\n", statePtr->IDEX.readRegA);
    printf("\t\treadRegB %d\n", statePtr->IDEX.readRegB);
    printf("\t\toffset %d\n", statePtr->IDEX.offset);
    printf("\tEXMEM:\n");
    printf("\t\tinstruction ");
    printInstruction(statePtr->EXMEM.instr);
    printf("\t\tbranchTarget %d\n", statePtr->EXMEM.branchTarget);
    printf("\t\taluResult %d\n", statePtr->EXMEM.aluResult);
    printf("\t\treadRegB %d\n", statePtr->EXMEM.readRegB);
    printf("\tMEMWB:\n");
    printf("\t\tinstruction ");
    printInstruction(statePtr->MEMWB.instr);
    printf("\t\twriteData %d\n", statePtr->MEMWB.writeData);
    printf("\tWBEND:\n");
    printf("\t\tinstruction ");
    printInstruction(statePtr->WBEND.instr);
    printf("\t\twriteData %d\n", statePtr->WBEND.writeData);
}

int field0(int instruction){
    return( (instruction>>19) & 0x7);
}

int field1(int instruction){
    return( (instruction>>16) & 0x7);
}

int field2(int instruction){
    return(instruction & 0xFFFF);
}

int opcode(int instruction){
    return(instruction>>22);
}

void printInstruction(int instr){
    char opcodeString[10];

    if (opcode(instr) == ADD) {
        strcpy(opcodeString, "add");
    } else if (opcode(instr) == NOR) {
        strcpy(opcodeString, "nor");
    } else if (opcode(instr) == LW) {
        strcpy(opcodeString, "lw");
    } else if (opcode(instr) == SW) {
        strcpy(opcodeString, "sw");
    } else if (opcode(instr) == BEQ) {
        strcpy(opcodeString, "beq");
    } else if (opcode(instr) == JALR) {
        strcpy(opcodeString, "jalr");
    } else if (opcode(instr) == HALT) {
        strcpy(opcodeString, "halt");
    } else if (opcode(instr) == NOOP) {
        strcpy(opcodeString, "noop");
    } else {
        strcpy(opcodeString, "data");
    }

    printf("%s %d %d %d\n", opcodeString, field0(instr), field1(instr), field2(instr));
}

int convertNum(int num){
    // convert a 16-bit number into a 32-bit Linux integer
    if(num & (1<<15)){
       num -= (1<<16); 
    }
    return num;
}