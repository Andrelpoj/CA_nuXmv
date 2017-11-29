#define KEYWORDS {"INITIAL","STATES","NAMES","TRANSITIONS","CA"}
#define NKEYWORDS 5
#define LKEYWORDS 12
#define OPERATORS {'+','-','*','/','%','=','&','|','!','>','<','(',')','[',']',',',':'}
#define NOPERATORS 17
#define OPERATORS2 {"->",">=","<=","<->","!="}
#define NOPERATORS2 5
#define LOPERATORS2 4
#define LMAXTOKEN 255
 
#include<stdio.h>
#include<stdlib.h>
#include<string.h>
#include<ctype.h>
  
int isKeyword(char buffer[]) {
    char keywords[NKEYWORDS][LKEYWORDS] = KEYWORDS;
    int i;
    for(i = 0; i < NKEYWORDS; i++)
        if(strcmp(keywords[i], buffer) == 0) return 1;
    return 0;
}
 
int isOperator(char buffer) {
    char operators[NOPERATORS] = OPERATORS;
    int i;
    for(i = 0; i < NOPERATORS; i++)
        if(operators[i] == buffer) return 1;
    return 0;
}
 
int isOperator2(char buffer[]) {
    char operators2[NOPERATORS2][LOPERATORS2] = OPERATORS2;
    int i;
    for(i = 0; i < NOPERATORS2; i++)
        if(strcmp(operators2[i], buffer) == 0) return 1;
    return 0;
}
 
int write(char buffer[], FILE *out, int isNumber, int isID, int isOp) {
    if(isOp && isOperator2(buffer)) fprintf(out, "%s %s\n", buffer, buffer);
    else if(isOp && isOperator(buffer[0])) fprintf(out, "%c %c\n", buffer[0], buffer[0]);
    else if(isID && isKeyword(buffer)) fprintf(out, "%s %s\n", buffer, buffer);
    else if(isNumber) fprintf(out, "num %s\n", buffer);
    else if(isID) fprintf(out, "id %s\n", buffer);
    else return 0;
    return 1;
}
  
int main(int argc, char *argv[]) {
    char ch, buffer[LMAXTOKEN];
    FILE *ca, *out = stdout;
    int i, j = 0;
    int isNumber = 0;
    int isID = 0;
    int isOp = 0;
    unsigned int line =0;
     
    if(argc == 1) {
        printf("usage: %s input_file [output_file]\n", argv[0]);
        return 0;
    }
    ca = fopen(argv[1], "r");
    if(ca == NULL) {
        printf("error while opening file %s\n", argv[1]);
        return 2;
    }
    if(argc > 2) {
        out = fopen(argv[2], "w");
        if(out == NULL) {
            printf("error while opening file %s\n", argv[2]);
            return 2;
        }
    }
     
    while((ch = fgetc(ca)) != EOF) {
           line++;
           if(isspace(ch) || isblank(ch)) {
               if(j > 0) {
                   buffer[j] = '\0';
                   if(!write(buffer, out, isNumber, isID, isOp)) {
                       printf("error in line %u: invalid token %s\n", line, buffer);
                       return 2;
                   }
                   j = isNumber = isID = isOp = 0;
               }
       }
       else if((isNumber || j == 0) && isdigit(ch)) {
               buffer[j++] = ch;
               isNumber = 1;
           }
           else if((isID || j == 0) && isalnum(ch)) {
               buffer[j++] = ch;
               isID = 1;
           }
           else if(isOp || isOperator(ch)) {
               if(isNumber || isID || isalnum(ch)) {
                   buffer[j] = '\0';
                   if(!write(buffer, out, isNumber, isID, isOp)) {
                       printf("error in line %u: invalid token %s\n", line, buffer);
                       return 2;
                   }
                   j = 0;
               }
               isNumber = isdigit(ch);
               isID = isalpha(ch);
               isOp = !(isNumber || isID);
               buffer[j++] = ch;
           }
           else {
               buffer[j++] = ch;
               buffer[j] = '\0';
               printf("error in line %u: invalid token %s\n", line, buffer);
               return 2;
           }
           if(j == LMAXTOKEN) {
               printf("error in line %u: %s exceeded max token size of %d characters\n", line, buffer, LMAXTOKEN);
               return 2;
           } 
    }
    fclose(ca);
    if(argc > 2) fclose(out);
    return 0;
}
