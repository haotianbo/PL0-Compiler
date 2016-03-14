/*Clayton Walker
 COP 3402
 Due: 6/9/13
 Virtual Machine
 */

#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include "data.h"

const char *opcodes[] = {"ILLEGAL", "lit", "opr", "lod", "sto", "cal", "inc", "jmp", "jpc", "sio", "sio"};

void printStackFrame(int* stack, int SP, int BP, FILE* ofp);
void executeCycle(instruction* irStruct, int* stack, int* sp, int* bp, int* pc);
void OPR (int *sp, int* bp, int *pc, int *stack, instruction* irStruct);
int base(int* bp, instruction* irStruct, int* stack);

FILE* ofp2;
FILE* ofp3;

void vm(void) {

    //Declaring file pointers
    FILE* ifp;
    FILE* ofp;
    
    ifp = fopen("parserout.txt", "r");
    ofp = fopen("vmout.txt", "w");
    ofp2 = fopen("vmout2.txt", "w");
    ofp3 = fopen("vmout3.txt", "w");

    //Declraing index and stack
    int i=0;
    int stack[MAX_STACK_HEIGHT] = {0};

    //Assigning values from homework specifications
    int SP = 0;
    int BP = 1;
    int PC = 0;
    int IR = 0;

    //arrayStruct is a filled array of instruction structs, containing the op, l, and m.
    //irStruct is a single reference of the instruction struct
    instruction arrayStruct[CODE_SIZE];
    instruction *irStruct;

    //Scans in the instructions line by line until end of file.
    //Also prints out first part of program to screen:
    //The Line, OP (by name not number), L, and M.

        fprintf(ofp, "Line\tOP\tL\tM\n");
        fprintf(ofp2, "Line\tOP\tL\tM\n");
         while (fscanf(ifp, "%d %d %d", &arrayStruct[i].op, &arrayStruct[i].l, &arrayStruct[i].m) != EOF) {
             fprintf(ofp, "%d\t%s\t%d\t%d\n", i, opcodes[arrayStruct[i].op], arrayStruct[i].l, arrayStruct[i].m);
             fprintf(ofp2, "%d\t%s\t%d\t%d\n", i, opcodes[arrayStruct[i].op], arrayStruct[i].l, arrayStruct[i].m);
             i++;
         }

    //Prints second part of program to screen, starting with
    //the pc, bp, sp, and stack headers.

        fprintf(ofp, "\n\n");
        fprintf(ofp, "\t\t\t\tpc \tbp \tsp \tstack\n");
        fprintf(ofp, "Initial values\t\t\t%d  \t%d \t%d \n", PC, BP, SP);
    
        //fprintf(ofp3, "\n\n");
        fprintf(ofp3, "\t\t\t\tpc \tbp \tsp \tstack\n");
        fprintf(ofp3, "Initial values\t\t\t%d  \t%d \t%d \n", PC, BP, SP);

    //Prints out the second part of the program. Starts and keeps going until BP = 0, representing the program stopping.
    while (BP!=0) {

        //Prints out the PC, OP, L, and M. This is also the "Fetch Cycle"
        irStruct=&arrayStruct[PC];
            fprintf(ofp, "%d\t%s \t%d \t%d",PC, opcodes[irStruct->op], irStruct->l, irStruct->m);
            fprintf(ofp3, "%d\t%s \t%d \t%d",PC, opcodes[irStruct->op], irStruct->l, irStruct->m);
        PC++;

        //Sends to giant switch function, mimicking the execute cycle
        executeCycle(irStruct, stack, &SP, &BP, &PC);
            fprintf(ofp, "\t%d  \t%d \t%d \t", PC, BP, SP);

            fprintf(ofp3, "\t%d  \t%d \t%d \t", PC, BP, SP);
        //Calls function that prints out each stack frame and its values, separated by |
        printStackFrame(stack, SP, BP, ofp);
            fprintf(ofp, "\n");

            fprintf(ofp3, "\n");
    }

    fclose(ifp);
    fclose(ofp);
    fclose(ofp2);
    fclose(ofp3);
}

//Recursive function that prints each stack frame using | in between
void printStackFrame(int* stack, int SP, int BP, FILE* ofp) {

    int i=0;

    //Base Case #1: if BP is 0, the program has finished. No stack frames are left to print out
    if (BP==0) {
        return;
    }
    //Base Case #2: if BP is 1, then it is in the main stack frame, and we print out the stack from BP to SP, with BP pointing to the bottom of the main stack frame, and SP pointing to the top of the stack
    else if (BP==1) {

        for(i=1;i<=SP;i++){
                fprintf(ofp, "%d ",stack[i]);
            
                fprintf(ofp3, "%d ",stack[i]);
        }
        return;
    }
    //Recursive Case: Prints out each new stack frame, separating them with |
    else {
        printStackFrame(stack, BP-1, stack[BP+2], ofp);

        //Covers one case, where CAL instruction is just called, meaning a new Stack Frame is created, but SP is still less than BP
        if (SP<BP) {
                fprintf(ofp, "| ");
    
                fprintf(ofp3, "| ");
            
            for (i=0;i<4;i++) {
                    fprintf(ofp, "%d ", stack[BP+i]);

                    fprintf(ofp3, "%d ", stack[BP+i]);
            }
        }
        //For SP being greater than BP, aka most cases
        else {
            fprintf(ofp, "| ");
            
            fprintf(ofp3, "| ");
            for (i=BP;i<=SP;i++) {
                    fprintf(ofp, "%d ", stack[i]);
                
                    fprintf(ofp3, "%d ", stack[i]);
            }
        }
        return;
    }
}

//Execute Cycle Function: mimics execute, and contains code
//for each instruction in ISA, implemented in a switch.
//OPR is another function because it was better looking.
void executeCycle(instruction* irStruct, int* stack, int* sp, int* bp, int* pc) {

    switch(irStruct->op) {
        case 1: //LIT
            *sp=*sp+1;
            stack[*sp]=irStruct->m;
            break;
        case 2: //OPR function
            OPR(sp, bp, pc, stack, irStruct);
            break;
        case 3: //LOD
            *sp=*sp+1;
            stack[*sp]=stack[base(bp, irStruct, stack)+irStruct->m];
            break;
        case 4: //STO
            stack[base(bp, irStruct, stack)+irStruct->m]=stack[*sp];
            *sp=*sp-1;
            break;
        case 5: //CAL
            stack[*sp+1]=0; //space to return value
            stack[*sp+2]=base(bp, irStruct, stack); //static link (SL)
            stack[*sp+3]=*bp; //dynamic link (DL)
            stack[*sp+4]=*pc; //return address (RA)
            *bp=*sp+1;
            *pc=irStruct->m;
            break;
        case 6: //INC
            *sp=*sp+irStruct->m;
            break;
        case 7: //JMP
            *pc=irStruct->m;
            break;
        case 8: //JPC
            if (stack[*sp]==0) {
                *pc=irStruct->m;
            }
            *sp=*sp-1;
            break;
        case 9: //SIO1
            printf("%d\n", stack[*sp]);
            *sp=*sp-1;
            break;
        case 10: //SIO2
            *sp=*sp+1;
            scanf("%d", &stack[*sp]);
            break;
        default:
            printf("Illegal OPR!\n");
    }
}

//OPR function: also a switch, determined by the OP's M, passed through irStruct.
void OPR (int *sp, int* bp, int *pc, int *stack, instruction* irStruct) {

    switch (irStruct->m) {

        case 0: //RET
            *sp=*bp-1;
            *pc=stack[*sp+4];
            *bp=stack[*sp+3];
            break;
        case 1: //NEG
            stack[*sp]=-stack[*sp];
            break;
        case 2: //ADD
            *sp=*sp-1;
            stack[*sp]=stack[*sp]+stack[*sp+1];
            break;
        case 3: //SUB
            *sp=*sp-1;
            stack[*sp]=stack[*sp]-stack[*sp+1];
            break;
        case 4: //MUL
            *sp=*sp-1;
            stack[*sp]=stack[*sp]*stack[*sp+1];
            break;
        case 5: //DIV
            *sp=*sp-1;
            stack[*sp]=stack[*sp]/stack[*sp+1];
            break;
        case 6: //ODD
            stack[*sp]=stack[*sp]%2;
            break;
        case 7: //MOD
            *sp=*sp-1;
            stack[*sp]=stack[*sp]%stack[*sp+1];
            break;
        case 8: //EQL
            *sp=*sp-1;
            stack[*sp]=stack[*sp]==stack[*sp+1];
            break;
        case 9: //NEQ
            *sp=*sp-1;
            stack[*sp]=stack[*sp]!=stack[*sp+1];
            break;
        case 10: //LSS
            *sp=*sp-1;
            stack[*sp]=stack[*sp]<stack[*sp+1];
            break;
        case 11: //LEQ
            *sp=*sp-1;
            stack[*sp]=stack[*sp]<=stack[*sp+1];
            break;
        case 12: //GTR
            *sp=*sp-1;
            stack[*sp]=stack[*sp]>stack[*sp+1];
            break;
        case 13: //GEQ
            *sp=*sp-1;
            stack[*sp]=stack[*sp]>=stack[*sp+1];
            break;
    }
}

//Base function: function used to find base L levels down
//l stans for L in the instruction format, passed through the irStruct
int base(int* bp, instruction* irStruct, int* stack) {
    int l = irStruct->l;
    int b1; //find base L levels down
    b1 = *bp;
    while (l>0) {
        b1=stack[b1+1];
        l--;
    }
    return b1;
}
