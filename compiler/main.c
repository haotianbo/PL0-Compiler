/*Clayton Walker
 COP 3402
 Due: 7/21/13
 Main/Compile
 */

#include <string.h>
#include <stdio.h>
#include <stdlib.h>
#include "data.h"

//these are flags that will say whether or not we want to print something or not
int lexoutput;
int codegenoutput;
int vmoutput;

int c;
FILE* ifp;

//Given enum for internal representation
const char *lexSym[] = { "", "nulsym", "identsym", "numbersym", "plussym", "minussym",
    "multsym",  "slashsym", "oddsym", "eqsym", "neqsym", "lessym", "leqsym",
    "gtrsym", "geqsym", "lparentsym", "rparentsym", "commasym", "semicolonsym",
    "periodsym", "becomessym", "beginsym", "endsym", "ifsym", "thensym",
    "whilesym", "dosym", "callsym", "constsym", "varsym", "procsym", "writesym",
    "readsym" , "elsesym"};

//functions
void usage(void);

//main, which when called, will call other .c files
int main(int argc, char *argv[]) {
    
    //temporary variable to be used to check -l, -a, and -v
    char** temp=argv;

    //we parse arguments, the argv contains the string passed by the program, ie "-l" "-a" "src_file" etc.
    //argc is the length of the array argv, argv[0] is the program name, so its length is always at least 1.
    
    //In our specific case, we always compile with argv[0] being the program name, and argv[1] being the input test file name. This means argc will be 2, or argv will always have length of 2.
    
    //for each command, meaning "-l, -a, -v" it will be in argv[2] which means argc will be 3. We check for that in a loop, and string compare it to set our flags.
        while (argc > 1) {
            if (strcmp(temp[1], "-l") == 0) {
                lexoutput = 1;
            }
            if (strcmp(temp[1], "-a") == 0) {
                codegenoutput = 1;
            }
            if (strcmp(temp[1], "-v") == 0) {
                vmoutput = 1;
            }
            argc--;
            temp++;
        }
    
    //if just ./compile, or just the test file is given, ie "./compile test.txt" then a usage function will be given.
    //when you are out here, if argc is > 1, then argv[1] will be the source file, unless you want to read from a pre-determined input file, then you would check while (argc > 1) and glob the last one
    
    //this sends the input file name to the compile function below
    //compile(argv[1]);
    compile();
}

//runs all .c files
//void compile(char* filename) {
void compile() {
    
    //run lex.c with the test case. Lex outputs a file with the lexeme list that parser reads from
    //lex(filename);
    lex();
    
    //run parser.c with the input file, that is the output file from lex. This then writes to another output file that the vm reads
    parse();
    
    //run vm.c with the input file, that is the output file from the parser. This then writes to a 3rd and final output file, which will have all the vm prints.
    vm();
    
    //do lex print stuff
    
    //this reads from "lexoutput.txt" and...
    if (lexoutput == 1) {
        //...first prints out the token (internal represenation) list, and then...
        printf("\n");
        ifp = fopen("lexoutput.txt", "r");
        while((c=fgetc(ifp)) !=EOF){
            printf("%c", c);
        }
        printf("\n\n");
        fclose(ifp);
        
        //...prints out the symbolic representation list!
        int i;
        ifp = fopen("lexoutput.txt", "r");
        //prints out variable names in Lexeme List
        for(i=0;i<lexListIndex;i++){
            printf("%s ", lexSym[lexList[i].tokenID]);
            if(lexList[i].tokenID==2){
                printf("%s ", lexList[i].name);
            }
            else if(lexList[i].tokenID==3){
                printf("%d ",lexList[i].numberValue);
            }
        }
        printf("\n\n");
        fclose(ifp);
    }
    
    //this will only be printed if we had no errors
    if (errorCount==0) {
        printf("No errors, program is syntactically correct\n\n");
    }
    
    //do parse print stuff
    //this reads from "vmout2.txt" and...
    if (codegenoutput == 1) {
        
        //...prints out the generated code
        ifp = fopen("vmout2.txt", "r");
        while((c=fgetc(ifp)) !=EOF){
            //putchar(c);
            printf("%c", c);
        }
        printf("\n");
        fclose(ifp);
    }
    
    //do vm print stuff
    //this reads from "vmout3.txt" and...
    if (vmoutput == 1) {
        
        //...prints out the program after it runs on the virtual machine
        ifp = fopen("vmout3.txt", "r");
        while((c=fgetc(ifp)) !=EOF){
            printf("%c", c);
        }
        printf("\n");
        fclose(ifp);
    }
}

//prints out how to use the program
void usage() {
    
    printf("\n-l prints out the lexeme list from lex.c\n");
    printf("-a prints out instructions from vm.c, which are passed to vm.c through the parser.c\n");
    printf("-v prints out stack frames from vm.c\n\n");
    return;
}