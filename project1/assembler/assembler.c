/* Assembler code fragment for LC-2K */

#include <stdlib.h>
#include <stdio.h>
#include <string.h>

#define MAXLINELENGTH 1000

int readAndParse(FILE *, char *, char *, char *, char *, char *); // 라벨, opcode, regA, regB, offset
int isNumber(char *);
int reg[8];
int adrs[50];

typedef struct{
    char lab[7]; // 라벨 이름.
    int address; // 라벨 주소. (프로그램 카운트)
} LabelAddressMap;

int findLabel(char *label, LabelAddressMap *labelMap, int labelCount);

int main(int argc, char *argv[]) 
{
    char *inFileString, *outFileString;
    FILE *inFilePtr, *outFilePtr;
    char label[MAXLINELENGTH], opcode[MAXLINELENGTH], arg0[MAXLINELENGTH], 
         arg1[MAXLINELENGTH], arg2[MAXLINELENGTH]; // 배열을 하나씩 만듦.

    if (argc != 3) {
        printf("error: usage: %s <assembly-code-file> <machine-code-file>\n",
                argv[0]);
        exit(1);
    }

    inFileString = argv[1];
    outFileString = argv[2];

    inFilePtr = fopen(inFileString, "r"); // 파일 읽기. as file.
    if (inFilePtr == NULL) {
        printf("error in opening %s\n", inFileString);
        exit(1);
    }
    outFilePtr = fopen(outFileString, "w"); // 파일 쓰기. mc file.
    if (outFilePtr == NULL) {
        printf("error in opening %s\n", outFileString);
        exit(1);
    }

    /* here is an example for how to use readAndParse to read a line from
       inFilePtr */
    //if (!readAndParse(inFilePtr, label, opcode, arg0, arg1, arg2)) {
    /* reached end of file */
    //}

    /* TODO: Phase-1 label calculation */
    int address = 0; // 프로그램 카운트.
    LabelAddressMap labelMap[100]; // 라벨 100개 저장 가능.
    int labelCount = 0;
    while(readAndParse(inFilePtr, label, opcode, arg0, arg1, arg2)) // 파일의 끝까지 반복.
    {
        if(label[0] != '\0'){
            if(findLabel(label, labelMap, labelCount) == -1){ // 라벨이 등록되어있지 않은 경우. 등록.
                strcpy(labelMap[labelCount].lab, label);
                labelMap[labelCount].address = address;
                labelCount++;
            }
            else{
                printf("Duplicated definition of labels.\n"); // 중복라벨.
                exit(1);
            }
        }
        if(opcode[0] == '.'){
            if(isNumber(arg0)){
                adrs[address] = atoi(arg0);
            }
            else{
                int idx = findLabel(arg0, labelMap, labelCount);
                adrs[address] = labelMap[idx].address;
            }
        }
        address++;
    }
    for (int i = 0; i<labelCount; i++){
        printf("%s: %d\n", labelMap[i].lab, labelMap[i].address);
    }

    /* this is how to rewind the file ptr so that you start reading from the
       beginning of the file */
    rewind(inFilePtr);

    /* TODO: Phase-2 generate machine codes to outfile */
    int pc = 0;
    while(readAndParse(inFilePtr, label, opcode, arg0, arg1, arg2)){
        //printf("line%d: %s %s %s %s\n", pc, opcode, arg0, arg1, arg2);
        if((strcmp(opcode,"noop"))&&(strcmp(opcode,"halt"))&&(opcode[0]!='.')) { // O-type instructions아닐 때 arg0 정수 검사.
            if((!isNumber(arg0))||(!isNumber(arg1))){ // reg0, reg1 int검사.
                printf("here: Non-integer register arguments.\n");
                exit(1);
            }

            if((!strcmp(opcode,"add"))||(!strcmp(opcode,"nor"))){ // R-type instruction
                if(!isNumber(arg2)){
                    printf("Non-integer register arguments.\n");
                    exit(1);
                }
            }
            else if((!strcmp(opcode, "lw"))||(!strcmp(opcode, "sw"))||(!strcmp(opcode, "beq"))){
                if(!isNumber(arg2)){
                    if(findLabel(arg2, labelMap, labelCount) == -1){
                        printf("Non-integer register arguments.\n");
                        exit(1);
                    }
                }
            }
        }
        if(atoi(arg2)>65535){ // 16bit에 안맞는 offset
            printf("offsetFields that does not fit in 16 bits.\n");
            exit(1);
        }
        if((opcode[0] != '.')&&(opcode != "halt")&&(opcode != "noop")){
            if((atoi(arg0)>7)|(atoi(arg1)>7)){ // 초과 레지스터 오류.
                printf("Registers outside the range [0,7].\n");
                exit(1);
            }
        }
        int line[32];
        int a;
        if (!strcmp(opcode, "add")){
            a = ((atoi(arg0) & 7)<<19)|((atoi(arg1) & 7)<<16)|(atoi(arg2) & 7);

        }
        else if (!strcmp(opcode, "nor")){
            a = (1<<22)|((atoi(arg0) & 7)<<19)|((atoi(arg1) & 7)<<16)|(atoi(arg2) & 7);
        }
        else if (!strcmp(opcode, "lw")){
            if(atoi(arg0) == 0){ // arg0 이 0일 경우, symbolic address.
                int idx = findLabel(arg2, labelMap, labelCount);
                if(idx == -1){
                    printf("Use of undefined labels.\n");
                    exit(1);
                }
                a = (labelMap[idx].address & 65535)|((atoi(arg0)&7)<<19)|((atoi(arg1)&7)<<16)|(1<<23);
            }
            else{ // 아닌 경우는 numeric address.
                a = ((atoi(arg0)&7)<<19)|((atoi(arg1)&7)<<16)|(1<<23)|(atoi(arg2)&7);
            }
        }
        else if (!strcmp(opcode, "sw")){
            if(atoi(arg0) == 0){ // arg0 이 0일 경우, symbolic address.
                int idx = findLabel(arg2, labelMap, labelCount);
                if(idx == -1){
                    printf("Use of undefined labels.\n");
                    exit(1);
                }
                a = (labelMap[idx].address & 65535)|((atoi(arg0) & 7)<<19)|((atoi(arg1) & 7)<<16)|(3<<22);
            }
            else{ // 아닌 경우는 numeric address.
                adrs[reg[atoi(arg0)]+atoi(arg2)] = reg[atoi(arg1)];
            }
        }
        else if (!strcmp(opcode, "beq")){
            if(isNumber(arg2)){
                a = (1<<24)|((atoi(arg0) & 7)<<19)|((atoi(arg1) & 7)<<16)|(atoi(arg2) & 7);
            }
            else{
                int idx = findLabel(arg2, labelMap, labelCount);
                int offset = labelMap[idx].address - pc - 1;
                if(idx == -1){
                    printf("Use of undefined labels.\n");
                    exit(1);
                }
                a = (offset & 65535)|((atoi(arg0)&7)<<19)|((atoi(arg1)&7)<<16)|(1<<24);
            }
        }
        else if(!strcmp(opcode, "jalr")){
            a = (5<<22)|(atoi(arg0)<<19)|(atoi(arg1)<<16);
        }
        else if(!strcmp(opcode, "halt")){
            a = (6<<22);
        }
        else if(!strcmp(opcode, "noop")){
            a = (7<<22);
        }
        else if(!strcmp(opcode, ".fill")){
            a = adrs[pc];
        }

        else{ // 이상한 opcode.
            printf("Unrecognized opcodes.\n");
            exit(1);
        }
        fprintf(outFilePtr, "(address %d): %d (hex 0x%x)\n", pc, a, a);
        pc++;
    }
    /* after doing a readAndParse, you may want to do the following to test the
       opcode */
    if (!strcmp(opcode, "add")) {
        /* do whatever you need to do for opcode "add" */
    }

    if (inFilePtr) {
        fclose(inFilePtr);
    }
    if (outFilePtr) {
        fclose(outFilePtr);
    }
    return(0);
}

/*
 * Read and parse a line of the assembly-language file.  Fields are returned
 * in label, opcode, arg0, arg1, arg2 (these strings must have memory already
 * allocated to them).
 *
 * Return values:
 *     0 if reached end of file
 *     1 if all went well
 *
 * exit(1) if line is too long.
 */
int readAndParse(FILE *inFilePtr, char *label, char *opcode, char *arg0,
        char *arg1, char *arg2)
{
    char line[MAXLINELENGTH];
    char *ptr = line;

    /* delete prior values */
    label[0] = opcode[0] = arg0[0] = arg1[0] = arg2[0] = '\0';

    /* read the line from the assembly-language file */
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
    if (sscanf(ptr, "%[^\t\n\r ]", label)) {
        /* successfully read label; advance pointer over the label */
        ptr += strlen(label);
    }

    /*
     * Parse the rest of the line.  Would be nice to have real regular
     * expressions, but scanf will suffice.
     */
    sscanf(ptr, "%*[\t\n\r ]%[^\t\n\r ]%*[\t\n\r ]%[^\t\n\r ]%*[\t\n\r ]%"
            "[^\t\n\r ]%*[\t\n\r ]%[^\t\n\r ]", opcode, arg0, arg1, arg2);
    return(1);
}

int isNumber(char *string)
{
    /* return 1 if string is a number */
    int i;
    return( (sscanf(string, "%d", &i)) == 1);
}

int findLabel(char *label, LabelAddressMap *labelMap, int labelCount){
    for(int i = 0; i<labelCount; i++){
        if(strcmp(label, labelMap[i].lab) == 0) {
            return i; // 이미 있으면 그 idx 리턴.
        }
    }
    return -1; // 만약 새로운 라벨이면 -1 리턴.
}
