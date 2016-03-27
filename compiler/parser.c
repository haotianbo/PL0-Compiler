/*Clayton Walker
 COP 3402
 Due: 7/21/13
 Parser
 */

#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <ctype.h>
#include "data.h"

//Recommended data structure for the symbol.
typedef struct {
	int kind; 		// const = 1, var = 2, proc = 3
	char name[12];	// name up to 11 chars
	int val; 		// number (ASCII value)
	int level; 		// L level
	int addr; 		// M address
} symbol;

/*For constants, you must store kind, name and value.
 For variables, you must store kind, name, L and M.
 For procedures, you must store kind, name, L and M.*/

//functions
void block(int lev, int tx, symbol* table, FILE* ifp, instruction* code);
void constdeclaration(int lev, int *ptx, int *pdx, FILE* ifp, symbol* table);
void vardeclaration(int lev, int *ptx, int *pdx, FILE* ifp, symbol* table);
void statement(int lev, int *ptx, FILE* ifp, instruction* code, symbol* table);
void condition(int lev, int *ptx, FILE* ifp, instruction* code, symbol* table);
void expression(int lev, int *ptx, FILE* ifp, instruction* code, symbol* table);
void term(int lev, int *ptx, FILE* ifp, instruction* code, symbol* table);
void factor(int lev, int *ptx, symbol* table, FILE* ifp, instruction* code);

void emit(int op, int l, int m, instruction* code);
void enter(int k, int *ptx, int *pdx, int lev, symbol* table);
void error(int errorCase);

int getNextToken(FILE* ifp);
int position(char *id, int *ptx, symbol* table, int lev);
void program(FILE* ifp, symbol* table, instruction* code);


//Globals Defined
int cx, token, num, kind, lexemeListIndex=0, errorCount=0, difference, previousDifference=0;
char id[12];

void parse(void) {

    //Declaring file pointers
    FILE* ifp;
    FILE* ofp;

    ifp = fopen("lexoutput.txt", "r");
    ofp = fopen("parserout.txt", "w");
    
    int i;
    int lev = 0; //Lexigraphical level
    int dx = 0; //data index
    int tx = 0; //table index

    //Declaring symbol_table array of symbol structs
    //symbol table needs to be 2D array of size [max][4] because max lex level is 3, so 0,1,2,3 is size[4]
    symbol table[MAX_SYMBOL_TABLE_SIZE] = {0};

    //array of instruction structs, called code, which holds the instructoins formed in parser from the lexList, which will be passed to the vm.c
    instruction code[CODE_SIZE];

    //start program
    program(ifp, table, code);

    //prints instructions to file
    for (i=0; i<cx;i++) {
        fprintf(ofp, "%d %d %d\n", code[i].op, code[i].l, code[i].m);
    }
    fclose(ifp);
    fclose(ofp);
}

void program(FILE* ifp, symbol* table, instruction* code) {

    token = getNextToken(ifp);
    block(0,0, table, ifp, code);

    if (token!=periodsym) {
        error(9); //Period expected.
    }
}

void block(int lev, int tx, symbol* table, FILE* ifp, instruction* code) {
    
    //errors for above max lexi level
    if(lev > MAX_LEXI_LEVELS) {
        error(26);
    }
    
    int dx, tx0, cx0;
    //this changes the amount in M for INC calls, so if dx=3; we will only create space for sp,bp,pc, which throws off vm.c, but if dx=4; meaning we will create space for sp,bp,pc,and retrun value, which makes vm.c work properly.
    dx=4;
    tx0=tx;
    table[tx].addr=cx;
    emit(7,0,0, code); // 7 is JMP for op, 0 is for L and 0 for M

    do {
        if (token==constsym) {
            token = getNextToken(ifp);
            do {
                constdeclaration(lev,&tx,&dx, ifp, table);
                while(token==commasym) {
                    token = getNextToken(ifp);
                    constdeclaration(lev,&tx,&dx, ifp, table);
                }
                if(token==semicolonsym) {
                    token = getNextToken(ifp);
                }
                else {
                    error(5); //Semicolon or comma missing.
                }
            } while (token==identsym);
        }
        if (token==varsym) {
            token = getNextToken(ifp);
            do {
                vardeclaration(lev,&tx,&dx, ifp, table);
                while (token==commasym) {
                    token = getNextToken(ifp);
                    vardeclaration(lev,&tx,&dx, ifp, table);
                }
                if(token==semicolonsym) {
                    token = getNextToken(ifp);
                }
                else {
                    error(5); //Semicolon or comma missing.
                }
            } while(token==identsym);
        }
        while(token==procsym) {
            token = getNextToken(ifp);

            //get identsym, then store it in symbol table, need to store, kind, level, name, dont care about addr.

            //store symbol in the symbol table to fix it later, you need to go back and fix address. you don't have enough info to generate code so you need to store it for later
            if(token==identsym) {
                enter(3, &tx, &dx, lev, table); //procedure
                token = getNextToken(ifp);
            }
            else {
                error(4); //const, var, procedure must be followed by identifier.
            }
            if(token==semicolonsym) {
                token = getNextToken(ifp);
            }
            else {
                error(5); //Semicolon or comma missing.
            }
            //lev++; call lev in block after incrementing
            block(lev+1, tx, table, ifp, code); //Go to a block one level higher
            if(token==semicolonsym) {
                token = getNextToken(ifp);
            }
            else {
                error(5); //Semicolon or comma missing.
            }
        }
    }while((token==constsym) || (token==varsym) || (token==procsym));

    //The tentative jump address is fixed up
    code[table[tx0].addr].m=cx;
    //the space for address for the above jmp is now occupied by the new cx
    table[tx0].addr=cx;
    //inc 0, dx is generated. At run time, the space of dx is secured
    cx0=cx;
    emit(6, 0, dx, code); // 6 is INC for op, 0 is for L, and dx is M
    statement(lev, &tx, ifp, code, table);
    emit(2, 0, 0, code); // 2 is OPR for op, 0 is RET for M inside OPR
}

void constdeclaration(int lev, int *ptx, int *pdx, FILE* ifp, symbol* table) {
    
    if (token==identsym) {
        token = getNextToken(ifp);
        if ((token==eqsym) || (token==becomessym)) {
            if (token==becomessym) {
                error(1); //Use = instead of :=
            }
            token = getNextToken(ifp);
            if (token==numbersym) {
                enter(1,ptx,pdx,lev, table);//const
                token = getNextToken(ifp);
            }
        }
    }
}

void vardeclaration(int lev, int *ptx, int *pdx, FILE* ifp, symbol* table) {
    
    if (token==identsym) {
        enter(2,ptx,pdx,lev, table);//var
        token = getNextToken(ifp);
    }
    else error(4); //const, var, procedure must be followed by identifier.
}

void statement(int lev, int *ptx, FILE* ifp, instruction* code, symbol* table) {
    
    int i, cx1, cx2;

    if (token==identsym){
        i=position(id,ptx, table, lev);
        if(i==0) {
            error(11); //Undeclared identifier.
        }
        else if (table[i].kind!=2) { //var
                error(12); //Assignment to constant or procedure is not allowed
                i=0;
        }
        token = getNextToken(ifp);
        if (token==becomessym) {
            token = getNextToken(ifp);
        }
        else {
            error(13); //Assignment operator expected.
        }
        expression(lev, ptx, ifp, code, table);
        if (i!=0) {
            emit(4, lev-table[i].level, table[i].addr, code); // 4 is STO for op, lev-table[i].level is for L, table[i].adr for M
        }
    }
    else if (token==callsym) {
        token = getNextToken(ifp);
        if (token!=identsym) {
            error(14); //call must be followed by an identifier
        }
        else {
            i=position(id, ptx, table, lev);
            if(i==0) {
                error(11); //Undeclared identifier.
            }
            else if (table[i].kind==3) {//proc
                emit(5,lev-table[i].level, table[i].addr, code); // 5 is CAL for op, lev-table[i].level is for L, table[i].adr for M
                //statement::= ["call" ident | ...]
            }
            else {
                    error(15); //Call of a constant or variable is meaningless
            }
            token = getNextToken(ifp);
        }
    }

    //if <condition> then <statement>
    else if (token == ifsym) {
        token = getNextToken(ifp);
        condition(lev, ptx, ifp, code, table);
        if(token == thensym) {
            token = getNextToken(ifp);
        }
        else {
            error(16);  // then expected
        }
        
        cx1 = cx;
        emit(8, 0, 0, code); // 8 is JPC for op, 0 is for L and 0 for M
        statement(lev, ptx, ifp, code, table);
    
        /**working on else**/
        if(token == elsesym) {
            token = getNextToken(ifp);
            
            code[cx1].m = cx+1; //jumps past if
            cx1 = cx;
            emit(7, 0, 0, code); // 7 is JMP for op, 0 is for L and cx1 for M
            //updates JPC M value
            statement(lev, ptx, ifp, code, table);
        }
        code[cx1].m = cx; //jumps past else (if theres an else statement) otherwise jumps past if
    }

    //begin <condition> end <statement>
    else if (token == beginsym) {
        token = getNextToken(ifp);
        statement(lev, ptx, ifp, code, table);
        
         /**changed**/
         while (token == semicolonsym) {
            token = getNextToken(ifp);
            statement(lev, ptx, ifp, code, table);
         }

        /**original**/
        /*while((token==semicolonsym)||(token==beginsym)||
         (token==ifsym)||(token==elsesym)||(token==whilesym)||
              (token==callsym)||(token==writesym)||(token==readsym)) {

            if (token==semicolonsym) {
                token = getNextToken(ifp);
            }
            else {
                error(10);  //Semicolon between statements missing.
            }
            statement(lev,ptx, ifp, code, table);
        }*/ //end original
        if (token == endsym) {
            token = getNextToken(ifp);
        }
        else {
            error(17);  //Semicolon or } expected.
        }
    }

    //while <condition> do <statement>
    else if (token == whilesym) {
        cx1 =cx;
        token = getNextToken(ifp);
        condition(lev,ptx, ifp, code, table);
        cx2 = cx;
        emit(8, 0, 0, code); // 8 is JPC for op, 0 is for L and 0 for M
        if(token == dosym) {
            token = getNextToken(ifp);
        }
        else {
            error(18);  // do expected
        }
        statement(lev, ptx, ifp, code, table);
        emit(7, 0, cx1, code); // 7 is JMP for op, 0 is for L and cx1 for M, jump to instruction cx1
        code[cx2].m = cx;
    }

    //write needs to write
    else if (token == writesym) {
        token = getNextToken(ifp);
        expression(lev, ptx, ifp, code, table);
        emit(9,0,1, code); // 9 is SIO1 for op, 0 is for L and 1 for M, write the top stack element to the screen
    }
    
    //read needs to read and STO
    else if (token == readsym) {
        token = getNextToken(ifp);
        emit(10,0,2, code); // 10 is SIO2 for op, 0 is for L and 1 for M, write the top stack element to the screen
        i=position(id,ptx, table, lev);
        if(i==0) {
            error(11); //Undeclared identifier.
        }
        else if (table[i].kind!=2) { //var
            error(12); //Assignment to constant or procedure is not allowed
            i=0;
        }
        if (i!=0) {
            emit(4, lev-table[i].level, table[i].addr, code); // 4 is STO for op, lev-table[i].level is for L, table[i].adr for M
        }
         token = getNextToken(ifp);
    }

}

void condition(int lev, int *ptx, FILE* ifp, instruction* code, symbol* table) {
    
    int relationSwitch;

 if (token == oddsym) {
     token = getNextToken(ifp);
        expression(lev,ptx, ifp, code, table);
     emit(2, 0, 6, code); // 2 is OPR for op, 6 is ODD for M inside OPR

    }
    else {
        expression(lev,ptx, ifp, code, table);
        if ((token!=eqsym) && (token!=neqsym) && (token!=lessym) && (token!=leqsym) && (token!=gtrsym) && (token!=geqsym)) {
            error(20); //Relational operator expected.
        }
        else { //for relational operators
            relationSwitch = token;
            token = getNextToken(ifp);
            expression(lev, ptx, ifp, code, table);
            switch(relationSwitch) {
                case 9:
                    emit(2,0,8, code); // 2 is OPR for op, 8 is EQL for M inside OPR
                    break;
                case 10:
                    emit(2,0,9, code); // 2 is OPR for op, 9 is NEQ for M inside OPR
                    break;
                case 11:
                    emit(2,0,10, code); // 2 is OPR for op, 10 is LSS for M inside OPR
                    break;
                case 12:
                    emit(2,0,11, code); // 2 is OPR for op, 11 is LEQ for M inside OPR
                    break;
                case 13:
                    emit(2,0,12, code); // 2 is OPR for op, 12 is GTR for M inside OPR
                    break;
                case 14:
                    emit(2,0,13, code); // 2 is OPR for op, 13 is GEQ for M inside OPR
                    break;
            }
        }
    }
}

void expression(int lev, int *ptx, FILE* ifp, instruction* code, symbol* table) {

    int addop;
    if (token == plussym || token == minussym) {
        addop = token;
        token = getNextToken(ifp);
        term(lev, ptx, ifp, code, table);
        if(addop == minussym) {
            emit(2, 0, 1, code); // 2 is OPR for op, 1 is NEG for M inside OPR
        }
    }
    else {
        term (lev, ptx, ifp, code, table);
    }
    while (token == plussym || token == minussym) {
        addop = token;
        token = getNextToken(ifp);
        term(lev, ptx, ifp, code, table);
        if (addop == plussym) {
            emit(2, 0, 2, code); // 2 is OPR for op, 2 is ADD for M inside OPR
        }
        else {
            emit(2, 0, 3, code); // 2 is OPR for op, 3 is SUB for M inside OPR
        }
    }
}

void term(int lev, int *ptx, FILE* ifp, instruction* code, symbol* table) {
    
    int mulop;
    factor(lev, ptx, table, ifp, code);
    while(token == multsym || token == slashsym) {
        mulop = token;
        token = getNextToken(ifp);
        factor(lev, ptx, table, ifp, code);
        if(mulop == multsym) {
            emit(2, 0, 4, code); // 2 is OPR for op, 4 is MUL for M inside OPR
        }
        else {
            emit(2, 0, 5, code); // 2 is OPR for op, 5 is DIV for M inside OPR
        }
    }
}

void factor(int lev, int *ptx, symbol* table, FILE* ifp, instruction* code) {
    
    int i, level, adr, val;

    while ((token==identsym)||(token==numbersym)||(token==lparentsym)){
        if (token==identsym) {
            i=position(id,ptx, table, lev);
            if (i==0) {
                error(11); // undeclared identifier
            }
            else {
                kind=table[i].kind;
                level=table[i].level;
                adr=table[i].addr;
                val=table[i].val;
                if (kind==1) {//const
                    emit(1,0,val, code); // 1 is LIT for op, val is for M inside LIT
                }
                else if (kind==2) {//var
                    emit(3,lev-level,adr, code); // 3 is LOD for op, lev-level is L inside LOD, adr is for M inside LOD
                }
                else {
                    error(21); // Expression must not contain a procedure identifier
                }
            }
            token = getNextToken(ifp);
        }
        /***this might need to be changed***/
        else if(token==numbersym) {
            if (num>2047) { //maximum address
                error(25);
                num=0;
            }
            emit(1,0,num, code); // 1 is LIT for op, num is for M inside LIT
            token = getNextToken(ifp);
        }
        else if(token==lparentsym) {
            token = getNextToken(ifp);
            expression(lev,ptx, ifp, code, table);
            if (token==rparentsym) {
                token = getNextToken(ifp);
            }
            else {
                error(22); // Right parenthesis missing.
            }
        }
    }
}

void emit(int op, int l, int m, instruction* code) {

    if (cx > CODE_SIZE)
        printf("Program too long! cx > CODE_SIZE\n");
    else {
        code[cx].op = op; 	//opcode
        code[cx].l = l;	// lexicographical level
        code[cx].m = m;	// modifier
        cx++;
    }
}

//This enters a symbol into the table
void enter(int k, int *ptx, int *pdx, int lev, symbol* table) {
    
    char *id1;
    int ii, len;
    (*ptx)++; //table index tx is increased by 1
    id1=id; //last identifier read
    len=strlen(id);
    for (ii=0;ii<=len;ii++) {
        table[*ptx].name[ii]=*id1; //id is recorded in .name
        id1++;
    }
    //updates kind
    table[*ptx].kind=k;
    //for constants: updates value
     if (k==1) {//const
        table[*ptx].val=num;
    }
    //for variables: updates L and M 
    else if (k==2) {//var
        table[*ptx].level=lev;
        table[*ptx].addr=*pdx;
        (*pdx)++;
    }
    //for procedures: updates L because M will change
    else {//procedure
        table[*ptx].level=lev;
    }
}

//prints out errors
void error(int errorCase) {

    errorCount++;
    
    switch (errorCase) {
        case 1:
            printf("Error 1: ");
            printf("Use = instead of :=.\n");
            break;
        case 2:
            printf("Error 2: ");
            printf("= must be followed by a number.\n");
            break;
        case 3:
            printf("Error 3: ");
            printf("Identifier must be followed by =.\n");
            break;
        case 4:
            printf("Error 4: ");
            printf("const, var, procedure must be followed by identifier.\n");
            break;
        case 5:
            printf("Error 5: ");
            printf("Semicolon or comma missing.\n");
            break;
        case 6:
            printf("Error 6: ");
            printf("Incorrect symbol after procedure declaration.\n");
            break;
        case 7:
            printf("Error 7: ");
            printf("Statement expected\n");
            break;
        case 8:
            printf("Error 8: ");
            printf("Incorrect symbol after statement part in block.\n");
            break;
        case 9:
            printf("Error 9: ");
            printf("Period expected.\n");
            break;
        case 10:
            printf("Error 10: ");
            printf("Semicolon between statements missing.\n");
            break;
        case 11:
            printf("Error 11: ");
            printf("Undeclared identifier.\n");
            break;
        case 12:
            printf("Error 12: ");
            printf("Assignment to constant or procedure is not allowed.\n");
            break;
        case 13:
            printf("Error 13: ");
            printf("Assignment operator expected.\n");
            break;
        case 14:
            printf("Error 14: ");
            printf("call must be followed by an identifier\n");
            break;
        case 15:
            printf("Error 15: ");
            printf("Call of a constant or variable is meaningless.\n");
            break;
        case 16:
            printf("Error 16: ");
            printf("then expected\n");
            break;
        case 17:
            printf("Error 17: ");
            printf("Semicolon or } expected\n");
            break;
        case 18:
            printf("Error 18: ");
            printf("do expected\n");
            break;
        case 19:
            printf("Error 19: ");
            printf("Incorrect symbol following statement.\n");
            break;
        case 20:
            printf("Error 20: ");
            printf("Relational operator expected.\n");
            break;
        case 21:
            printf("Error 21: ");
            printf("Expression must not contain a procedure identifier.\n");
            break;
        case 22:
            printf("Error 22: ");
            printf("Right parenthesis missing.\n");
            break;
        case 23:
            printf("Error 23: ");
            printf("The preceding factor cannot begin with this symbol.\n");
            break;
        case 24:
            printf("Error 24: ");
            printf("An expression cannot begin with this symbol.\n");
            break;
        case 25:
            printf("Error 25: ");
            printf("This number is too large.\n");
            break;
        case 26:
            printf("Error: 26 ");
            printf("Level is larger than the maximum allowed lexicographical levels!\n");
            break;
        default:
            break;
    }
    //stops program when error occurs
    exit(1);
}

int getNextToken(FILE* ifp) {
    
    token = lexList[lexemeListIndex].tokenID;
    
    //Takes care of variables, always represented by "2 | variable"
    if(token==2){
        strcpy(id, lexList[lexemeListIndex].name);
    }
    //Takes care of numbers, always represented by "3 | number"
    else if(token==3){
        num = lexList[lexemeListIndex].numberValue;
    }
    
    lexemeListIndex++;
    return token;
}

int position(char *id, int *ptx, symbol* table, int lev) {
    
    int s;
    s=*ptx;
    
    int currentS;
    int differenceCount = 0;

    while(s!=0) {
        if (strcmp(table[s].name,id) == 0) {
            /**Added in right now, trying to do procedures**/
            //if table[s].level isnt equal to the current level, and the kind is not equal to constant, then we don't have access to this variable in this stack frame
            
            //maybe table[s].level>lev ? because we can look at levels below, and current level, just not above.
            if(table[s].level<=lev) {
               
                if (differenceCount!=0) {
                    previousDifference = difference;
                }
                
                difference = lev-table[s].level;
                
                if(differenceCount==0) {
                    currentS=s;
                }
                
                if (difference<previousDifference) {
                    currentS=s;
                }
                differenceCount++;
            }
        }
        s--;
    }
    //minimum
    return currentS;
}
