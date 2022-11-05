/* Assembler code fragment for LC-2K */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define MAXLINELENGTH 1000
static char fill[] = ".fill";
static char blank[] = "";
int lineCnt = 0;
int offset = 0;

typedef struct LABEL{
    struct LABEL* nxt;
    int line;
    char label0[MAXLINELENGTH];
    char label1[MAXLINELENGTH];
    char label2[MAXLINELENGTH];
} LABEL;

LABEL* labelInit = NULL;
LABEL* labelFinal = NULL;

int readAndParse(FILE *, char *, char *, char *, char *, char *);
int isNumber(char *);
int rewindOffset(FILE* ptr);
int writeOutput(FILE *, FILE *);
int getLine(LABEL*, char*);
int labelParse(LABEL*, char*);
void offsetRangeChk(int);
void duplicatedLabelChk(LABEL*, char*);

int main(int argc, char *argv[]){
    char *inFileString, *outFileString;
    FILE *inFilePtr, *outFilePtr;
    char label[MAXLINELENGTH], opcode[MAXLINELENGTH], arg0[MAXLINELENGTH], arg1[MAXLINELENGTH], arg2[MAXLINELENGTH];
    if (argc != 3) {
        printf("error: usage: %s <assembly-code-file> <machine-code-file>\n", argv[0]);
        exit(1);
    }
    inFileString = argv[1];
    outFileString = argv[2];
    inFilePtr = fopen(inFileString, "r");
    if (inFilePtr == NULL) {
        printf("error in opening %s\n", inFileString);
        exit(1);
    }
    outFilePtr = fopen(outFileString, "w");
    if (outFilePtr == NULL) {
        printf("error in opening %s\n", outFileString);
        exit(1);
    }
    int j = 0;
    while(readAndParse(inFilePtr, label, opcode, arg0, arg1, arg2) ) {
        j++;
    }
    rewindOffset(inFilePtr);
    while(writeOutput(inFilePtr, outFilePtr)){
    }
    exit(0);
}/*
* Read and parse a line of the assembly-language file. Fields are returned
* in label, opcode, arg0, arg1, arg2 (these strings must have memory already
* allocated to them). *
* Return values:
* 0 if reached end of file
* 1 if all went well *
* exit(1) if line is too long.
*/
int readAndParse(FILE *inFilePtr, char *label, char *opcode, char *arg0, char *arg1, char *arg2){
    char line[MAXLINELENGTH];
    char *ptr = line;
    /* delete prior values */
    label[0] = opcode[0] = arg0[0] = arg1[0] = arg2[0] = '\0';
    if(fseek(inFilePtr, offset, SEEK_SET) != 0){
        // fseek failed!
        printf("error: fseek offset error!\n");
        exit(1);
    }
    /* read the line from the assembl y-language file */
    if (fgets(line, MAXLINELENGTH, inFilePtr) == NULL) {
        /* reached end of file */
        return(0);
    }
    /* check for line too long (by looking for a \n) */
    if (strchr(line, '\n') == NULL) {
        /* line too long */
        printf("error: line too long\n");
        exit(1);
    }
    /* is there a label? */
    ptr = line;
    if (sscanf(ptr, "%[^\t\n\r ]", label)) { // read till \t, \n, \r is read.
        duplicatedLabelChk(labelInit, label);
        /* successfully read label; advance pointer over the label */
        ptr += strlen(label);
        if(strlen(label)){
            label[7]='\0';
            // 라벨 초기화
            LABEL* lbl = (LABEL*)malloc(sizeof(LABEL));
            lbl->nxt = NULL;
            lbl->line = lineCnt;
            strncpy(lbl->label0, blank, sizeof(blank));
            strncpy(lbl->label1, blank, sizeof(blank));
            strncpy(lbl->label2, blank, sizeof(blank));

            strncpy(lbl->label0, label, strlen(label));
            if(labelFinal == NULL){
                labelInit = lbl;
                labelFinal = lbl;
            }
            else{
                labelFinal->nxt = lbl;
                labelFinal = lbl;
            }
        }
        if(labelFinal != NULL){
            /*
            * Parse the rest of the line. Would be nice to have real regular
            * expressions, but scanf will suffice.
            */
            sscanf(ptr, "%*[\t\n\r ]%[^\t\n\r ]%*[\t\n\r ]%[^\t\n\r ]%*[\t\n\r ]%[^\t\n\r ]%*[\t\n\r ]%[^\t\n\r ]", opcode, arg0, arg1, arg2);
            if(strcmp(fill, opcode) == 0){
                strncpy(labelFinal->label1, arg0, strlen(arg0));
            }else{
                int length = snprintf(NULL, 0, "%d", lineCnt); // to find out the length
                char* str = malloc(length+1);
                snprintf(str, length+1, "%d", lineCnt);
                strncpy(labelFinal->label1, str, strlen(str));
                free(str);
            }
        }
    }
    offset = ftell(inFilePtr);
    if(offset == -1){
        // ftell failed!
        printf("error: ftell offset error!\n");
        exit(1);
    }
    lineCnt++;
    return(1);
}

int isNumber(char *string){
    /* return 1 if string is a number */
    int i;
    return((sscanf(string, "%d", &i)) == 1);
}

int rewindOffset(FILE* ptr){
    offset = 0;
    lineCnt = 0;
}

int writeOutput(FILE* inFilePtr, FILE* outFilePtr){
    char line[MAXLINELENGTH];
    char *ptr = line;
    char label[MAXLINELENGTH], opcode[MAXLINELENGTH], arg0[MAXLINELENGTH], arg1[MAXLINELENGTH], arg2[MAXLINELENGTH];
    /* delete prior values */
    label[0] = opcode[0] = arg0[0] = arg1[0] = arg2[0] = '\0';
    if(fseek(inFilePtr, offset, SEEK_SET) != 0){
        // fseek failed!
        printf("error: fseek offset error!\n");
        exit(1);
    }
    /* read the line from the assembl y-language file */
    if (fgets(line, MAXLINELENGTH, inFilePtr) == NULL) {
        /* reached end of file */
        return(0);
    }
    /* check for line too long (by looking for a \n) */
    if (strchr(line, '\n') == NULL) {
        /* line too long */
        printf("error: line too long\n");
        exit(1);
    }
    if(strlen(line) == 0){
        return 0;
    }
    /* is there a label? */
    ptr = line;
    if (sscanf(ptr, "%[^\t\n\r ]", label)) { // read till \t, \n, \r is read.
        /* successfully read label; advance pointer over the label */
        ptr += strlen(label);
    }
    /*
    * Parse the rest of the line. Would be nice to have real regular
    * expressions, but scanf will suffice.
    */
    sscanf(ptr, "%*[\t\n\r ]%[^\t\n\r ]%*[\t\n\r ]%[^\t\n\r ]%*[\t\n\r ]%[^\t\n\r ]%*[\t\n\r ]%[^\t\n\r ]", opcode, arg0, arg1, arg2);
    int op2int = atoi(opcode);
    int bitA = 0, bitB = 0, bitC = 0, bitD = 0, bitE = 0, ml = 0;
    if(strcmp(opcode, "add") == 0){
        bitA = 0; //0b0;
        if(isNumber(arg0))
            bitB = atoi(arg0);
        else
            bitB = labelParse(labelInit, arg0);
        if(isNumber(arg1))
            bitC = atoi(arg1);
        else
            bitC = labelParse(labelInit, arg1);
        if(isNumber(arg2))
            bitD = atoi(arg2);
        else
            bitD = labelParse(labelInit, arg2);
        ml = bitA << 22 | bitB << 19 | bitC << 16 | bitD;
    }else if(strcmp(opcode, "nor") == 0){
        bitA = 1; //0b1;
        if(isNumber(arg0))
            bitB = atoi(arg0);
        else
            bitB = labelParse(labelInit, arg0);
        if(isNumber(arg1))
            bitC = atoi(arg1);
        else
            bitC = labelParse(labelInit, arg1);
        if(isNumber(arg2))
            bitD = atoi(arg2);
        else
            bitD = labelParse(labelInit, arg2);
        ml = bitA << 22 | bitB << 19 | bitC << 16 | bitD;
    }else if(strcmp(opcode, "lw") == 0){
        bitA = 2; //0b10;
        if(isNumber(arg0))
            bitB = atoi(arg0);
        else
            bitB = labelParse(labelInit, arg0);
        if(isNumber(arg1))
            bitC = atoi(arg1);
        else
            bitC = labelParse(labelInit, arg1);
        if(isNumber(arg2))
            bitD = atoi(arg2);
        else
            bitD = getLine(labelInit, arg2);
        ml = bitA << 22 | bitB << 19 | bitC << 16 | bitD;
    }else if(strcmp(opcode, "sw") == 0){
        bitA = 3; //0b11;
        if(isNumber(arg0))
            bitB = atoi(arg0);
        else
            bitB = labelParse(labelInit, arg0);
        if(isNumber(arg1))
            bitC = atoi(arg1);
        else
            bitC = labelParse(labelInit, arg1);
        if(isNumber(arg2))
            bitD = atoi(arg2);
        else
            bitD = getLine(labelInit, arg2);
        ml = bitA << 22 | bitB << 19 | bitC << 16 | bitD;
    }else if(strcmp(opcode, "beq") == 0){
        bitA = 4; //0b100;
        if(isNumber(arg0))
            bitB = atoi(arg0);
        else
            bitB = labelParse(labelInit, arg0);
        if(isNumber(arg1))
            bitC = atoi(arg1);
        else
            bitC = labelParse(labelInit, arg1);
        if(isNumber(arg2))
            bitD = atoi(arg2);
        else
            bitD = (getLine(labelInit, arg2) - lineCnt -1) & 65535; // 16bit mask
        ml = bitA << 22 | bitB << 19 | bitC << 16 | bitD;
    }else if(strcmp(opcode, "jalr") == 0){
        bitA = 5; //0b101;
        if(isNumber(arg0))
            bitB = atoi(arg0);
        else
            bitB = labelParse(labelInit, arg0);
        if(isNumber(arg1))
            bitC = atoi(arg1);
        else
            bitC = labelParse(labelInit, arg1);
        bitD = 0;
        ml = bitA << 22 | bitB << 19 | bitC << 16 | bitD;
    }else if(strcmp(opcode, "halt") == 0){
        bitA = 6; //0b110;
        ml = bitA << 22;
    }else if(strcmp(opcode, "noop") == 0){
        bitA = 7; //0b111;
        ml = bitA << 22;
    }else if(strcmp(opcode, fill) == 0){
        if(isNumber(arg0)){
            ml = atoi(arg0);
        }
        else{
            ml = labelParse(labelInit, arg0);
        }
    }else{
        printf("error: use of unrecognized opcode! (%s)\n", opcode);
        exit(1);
    }
    int length = snprintf(NULL, 0, "%d", ml); // to find out the length
    char* str = malloc(length+1);
    snprintf(str, length+1, "%d", ml);
    if(fprintf(outFilePtr, "%s", str) < 0){
        printf("error: output file printing error!\n");
        exit(1);
    }
    if(fprintf(outFilePtr, "\n") < 0){
        printf("error: output file printing error!\n");
        exit(1);
    }
    free(str);
    
    offset = ftell(inFilePtr);
    if(offset == -1){
        // ftell failed!
        printf("error: ftell offset error!\n");
        exit(1);
    }
    lineCnt++;
    return(1);
}

int getLine(LABEL* ptr, char* str){
    if(ptr != NULL){
        if(strncmp(str, ptr->label0, sizeof(str)>sizeof(ptr->label0)?sizeof(str):sizeof(ptr->label0)) == 0){
            if(!isNumber(ptr->label1)){
                return getLine(labelInit, ptr->label1);
            }
            return ptr->line;
        }else{
            if(ptr->nxt != NULL){
                return getLine(ptr->nxt, str);
            }else{
                // printf("error: undefined label!\n");
                printf("error: undefined label! (%s)\n",str);
                exit(1);
            }
        }
    }else{
        printf("error: NULL pointer is passed to the getLine function!\n");
        exit(1);
    }
}

int labelParse(LABEL* ptr, char* str){
    if(ptr != NULL){
        if(strncmp(str, ptr->label0, sizeof(str)>sizeof(ptr->label0)?sizeof(str):sizeof(ptr->label0)) == 0){
            if(!isNumber(ptr->label1)){
                return labelParse(labelInit, ptr->label1);
            }
            return atoi(ptr->label1);
        }else{
            if(ptr->nxt != NULL){
                return labelParse(ptr->nxt, str);
            }else{
                // printf("error: undefined label!\n");
                printf("error: undefined label! (%s)\n",str);
                exit(1);
            }
        }
    }else{
        printf("error: NULL pointer is passed to the getLine function!\n");
        exit(1);
    }
}

void offsetRangeChk(int offset){
    if(offset < -32768 || offset > 32767){
        printf("error: offsetFields don't fit in 16 bits!\n");
        exit(1);
    }
    return;
}

void duplicatedLabelChk(LABEL* ptr, char* str){
    if(ptr != NULL){
        if(strncmp(str, ptr->label0, sizeof(str)>sizeof(ptr->label0)?sizeof(str):sizeof(ptr->label0)) == 0){
            printf("error: dublicated label! (%s)\n", str);
            exit(1);
        }else{
            if(ptr->nxt != NULL)
                duplicatedLabelChk(ptr->nxt, str);
        }
    }
    return;
}