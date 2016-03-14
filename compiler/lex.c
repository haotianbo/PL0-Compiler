/*Clayton Walker
 COP 3402
 Due: 6/23/13
 Lexical Analyzer/Scanner
 */

/**Uses Structs!**/

#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <ctype.h>
#include "data.h"

//For Reserved Words
const char* reservedWords[]={"const", "var", "procedure", "call", "begin", "end", "if", "then", "else", "while", "do", "read", "write", "odd"};

//For Reserved Special Symbols
const char specialSymbols[]={'+', '-', '*', '/', '(', ')', '=', ',' , '.', '<', '>',  ';' , ':'};

//Lists for variables, numbers, and Lexeme
/*int lexemeList[5000];
int numberList[5000];
char variableNamesList[500][12];*/

tokenStruct lexList[5000];
int lexListIndex = 0;


//void lex(char* filename){
void lex(void){

    int count;
    for(count=0;count<5000;count++){
        lexList[count].tokenID=0;
    }
    
    //Declaring file pointers
    FILE* ifp;
    FILE* ofp;
    
    char filename[20];
    printf("What is the name of your test file?\n");
    scanf("%s", filename);

    ifp = fopen(filename, "r");
    ofp = fopen("lexoutput.txt", "w");

    //Variable Declarations
    int i,j=0,k=0;
    //for holding erros
    int errorHolder;
    //Variable to hold each character read in
    int c;
    //For comments
    int comments=0;
    //Looks ahead at next character read in
    int lookAhead=0;

    //Indexes that help prints Lexemes out in Lexeme List, keeps track of index for Lexemes/numberList, and variableNamesList
    /*int lexemeListIndex=0;
    int numberListIndex=0;
    int variableNamesListIndex=0;*/

    ifp=fopen(filename,"r");
    c=fgetc(ifp);
    //Ignores spaces, tabs, and newlines aka whitespace
    while(c!=EOF){
        if(c==' '||c=='\t'||c=='\r' ||c=='\n'){
            c=fgetc(ifp);
            lookAhead=0;
            continue;
        }
        //reads if the next character is part of the alphabet
        if(isalpha(c)){
            char characterString[12];
            memset(characterString, 0, sizeof characterString);
            
            int index=0;
            characterString[index]=c;

            index++;
            lookAhead=1;
            //Prints Error 3 if the variable name is too long
            while(isalpha(c=fgetc(ifp))||isdigit(c)){
                if(index>10){
                    //printf("Error 3: Name too long.\n");

                    //Error Checking
                    while (isalpha(c=fgetc(ifp))||isdigit(c)) {

                    }
                    errorHolder=1;

                    //lookAhead=0;
                    break;
                }
                characterString[index]=c;
                index++;
            }

            //If there was an error, continue without accepting token
            if(errorHolder==1) {
                errorHolder=0;
                continue;
            }

            //Compares the variable name to see if it is one of the reserved words
            int reservedSwitch=-1;

            for(i=0; i<14;i++){
                if(strcmp(characterString, reservedWords[i])==0){
                    reservedSwitch=i;
                }
            }
            //If it is a reserved word, print out the correct tokentype
            switch(reservedSwitch){

                //Case for const
                case 0:
                    lexList[lexListIndex].tokenID = constsym;
                    break;
                //Case for var
                case 1:
                    lexList[lexListIndex].tokenID = varsym;
                    break;
                //Case for procedure
                case 2:
                    lexList[lexListIndex].tokenID = procsym;
                    break;
                //Case for call
                case 3:
                    lexList[lexListIndex].tokenID = callsym;
                    break;
                //Case for begin
                case 4:
                    lexList[lexListIndex].tokenID = beginsym;
                    break;
                //Case for end
                case 5:
                    lexList[lexListIndex].tokenID = endsym;
                    break;
                //Case for if
                case 6:
                    lexList[lexListIndex].tokenID = ifsym;
                    break;
                //Case for then
                case 7:
                    lexList[lexListIndex].tokenID = thensym;
                    break;
                //Case for else
                case 8:
                    lexList[lexListIndex].tokenID = elsesym;
                    break;
                //Case for while
                case 9:
                    lexList[lexListIndex].tokenID = whilesym;
                    break;
                //Case for do
                case 10:
                    lexList[lexListIndex].tokenID = dosym;
                    break;
                //Case for read
                case 11:
                    lexList[lexListIndex].tokenID = readsym;
                    break;
                //Case for write
                case 12:
                    lexList[lexListIndex].tokenID = writesym;
                    break;
                //Case for odd
                case 13:
                    lexList[lexListIndex].tokenID = oddsym;
                    break;

                default:
                    lexList[lexListIndex].tokenID = identsym;
                    strcpy(lexList[lexListIndex].name,characterString);
                    break;
            }
            lexListIndex++;
        }
        //reads if the next character is part of the 0-9 digits
        else if(isdigit(c)){
            int number=c-'0';
            int d;
            int place=1;

            lookAhead=1;

            //Prints Error 2 if the number is too long
            while(isdigit(c=fgetc(ifp))){
                if(place>4){
                    //printf("Error 2: Number too long.\n");

                    //Error checking
                    while (isdigit(c=fgetc(ifp))) {

                    }
                    errorHolder=1;
                    //lookAhead=0;
                    break;
                }
                d=c-'0';
                number=10*number+d;
                place++;
            }

            //Prints Error 1 if the variable starts with a digit, and not a number
            if(isalpha(c)){
            //printf("Error 1: Variable does not start with letter.\n");
                while(isalpha(c=fgetc(ifp))||isdigit(c)){

                }
                continue;
             }

            //If there was an error, continue without accepting token
            if(errorHolder==1) {
                errorHolder=0;
                continue;
            }

            lexList[lexListIndex].tokenID=numbersym;
            lexList[lexListIndex].numberValue=number;
            lexListIndex++;
        }

        //reads if the next character is part of the special symbols
        else {
            lookAhead=0;
            int spec=-1;
            for(i=0;i<13;i++){
                if(c==specialSymbols[i]){
                    spec=i;
                }
            }
            //If it is a special symbol, print out the correct tokentype
            switch(spec){
                //Case for +
                case 0:
                    lexList[lexListIndex].tokenID = plussym;
                    lexListIndex++;
                    break;
                //Case for -
                case 1:
                    lexList[lexListIndex].tokenID = minussym;
                    lexListIndex++;
                    break;
                //Case for *
                case 2:
                    lexList[lexListIndex].tokenID = multsym;
                    lexListIndex++;
                    break;

                //Case for comments
                case 3:
                    c=fgetc(ifp);
                    lookAhead=1;
                    if(c=='*'){
                        comments=1;
                        lookAhead=0;
                        c=fgetc(ifp);
                        while(comments==1){
                            if(c=='*'){
                                c=fgetc(ifp);
                                if(c=='/'){
                                    comments=0;
                                }
                            }
                            else{
                                c=fgetc(ifp);
                            }
                        }
                    }
                    else{
                        lexList[lexListIndex].tokenID = slashsym;
                        lexListIndex++;
                    }
                    break;
                //Case for (
                case 4:
                    lexList[lexListIndex].tokenID = lparentsym;
                    lexListIndex++;
                    break;
                //Case for )
                case 5:
                    lexList[lexListIndex].tokenID = rparentsym;
                    lexListIndex++;
                    break;
                //Case for =
                case 6:
                    lexList[lexListIndex].tokenID = eqsym;
                    lexListIndex++;
                    break;
                //Case for ,
                case 7:
                    lexList[lexListIndex].tokenID = commasym;
                    lexListIndex++;
                    break;
                //Case for .
                case 8:
                    lexList[lexListIndex].tokenID = periodsym;
                    lexListIndex++;
                    break;
                //Case for <>
                case 9:
                    c=fgetc(ifp);
                    lookAhead=1;
                    if(c=='>'){
                        lexList[lexListIndex].tokenID = neqsym;
                        lookAhead=0;
                    }
                    //Case for <=
                    else if(c=='='){
                        lexList[lexListIndex].tokenID = leqsym;
                        lookAhead=0;
                    }
                    //Case for <
                    else{
                        lexList[lexListIndex].tokenID = lessym;
                    }
                    lexListIndex++;
                    break;
                //Case for >=
                case 10:
                    c=fgetc(ifp);
                    lookAhead=1;
                    if(c=='='){
                        lexList[lexListIndex].tokenID = geqsym;
                        lookAhead=0;
                    }
                    //Case for >
                    else{
                        lexList[lexListIndex].tokenID = gtrsym;
                    }
                    lexListIndex++;
                    break;
                //Case for ;
                case 11:
                    lexList[lexListIndex].tokenID = semicolonsym;
                    lexListIndex++;
                    break;
                //Case for :=
                case 12:
                    c=fgetc(ifp);
                    if(c == '='){
                        lexList[lexListIndex].tokenID = becomessym;
                        lexListIndex++;
                    }
                    //Prints Error 4 for invalid symbols
                    else{
                        //printf("Error 4: Invalid symbols.\n");
                    }
                    break;
                    //Prints Error 4 for invalid symbols
                default:
                    //printf("Error 4: Invalid symbols.\n");
                    break;
            }
        }
        //if we aren't looking at next character, read in next as a part of a new variable/number/symbol
        if(lookAhead==0){
            c=fgetc(ifp);
        }

    }
    //Prints out the Lexeme List, separated by "|"
    
    /**new for struct, instead of array**/
    //Takes care of variable names, always represented by "variableName | 2"
    fprintf(ofp,"%d", lexList[0].tokenID);
    if(lexList[0].tokenID==2){
        fprintf(ofp,"|%s", lexList[0].name);
    }
    //Takes care of numbers, always represented by "number | 3"
    else if(lexList[0].tokenID==3){
        fprintf(ofp,"|%d",lexList[0].numberValue);
    }
    //prints out variable names in Lexeme List
    for(i=1;i<lexListIndex;i++){
        fprintf(ofp,"|%d", lexList[i].tokenID);
        if(lexList[i].tokenID==2){
            fprintf(ofp,"|%s", lexList[i].name);
        }
        else if(lexList[i].tokenID==3){
            fprintf(ofp,"|%d",lexList[i].numberValue);
        }
    }
    fclose(ifp);
    fclose(ofp);
}
