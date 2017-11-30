#define LMAXTOKEN 255

#define THEIGHT 18 
#define TWIDTH 29
#define TDEPTH 10
#define Token[THEIGHT][TWIDTH][TDEPTH] grid;

#define ENDCHAR "@"

#include<stdio.h>
#include<stdlib.h>
#include<string.h>

typedef struct node {
    char content[LMAXTOKEN];
    struct node *child, *sibling;
} Node;

typedef struct{
    char value[LMAXTOKEN];
    char type[LMAXTOKEN];
}Token;
//typedef struct Token TOKEN;

Token* newToken(char name[LMAXTOKEN], char type[LMAXTOKEN]){
    Token* t = (Token*) malloc(sizeof(Token));
    strcpy(t->name,name);
    strcpy(t->type,type);
    return t;
}

typedef struct TokenStack{
    Token* token;
    Node* node;
    struct TokenStack *next;
}T_STACK;

T_STACK* newStack(Token* t, Node* n)
{
    T_STACK* stackNode = (T_STACK*) malloc(sizeof(T_STACK));
    stackNode->token = t;
    stackNode->node = n;
    stackNode->next = NULL;
    return stackNode;
}

int isEmpty(T_STACK *root)
{
    return !root;
}

void push(T_STACK** root, Token* t, Node* n)
{
    T_STACK* stackNode = newStack(t,n);
    stackNode->next = *root;
    *root = stackNode;
    printf("%s pushed to stack\n", t->name);
}

Token* pop(T_STACK** root)
{
    if (isEmpty(*root)){
        return NULL;
        //Token t;
        //sprintf(t.name, "%s", "");
        //sprintf(t.type, "%s", "-1");
    }

    T_STACK* temp = *root;
    *root = (*root)->next;
    Token* popped = temp->token;
    free(temp);

    //temp->next = NULL;
    return popped;
}

void fillTable(){
    Token t;

    //token de erro
    sprintf(t.value,"%s","");
    sprintf(t.type,"%s","#");

    int i, j, k;
    for(i=0;i<THEIGHT;i++){
        for(j=0;j<TWIDTH;j++){
            for(k=0;k<TDEPTH;k++){
                grid[i][j][k] = t;
            }
        }    
    }
    
    //Regra 1
    sprintf(t.value,"%s","transitions");
    sprintf(t.type,"%s","0");
    grid[0][0][0] = t;

    sprintf(t.value,"%s","names");
    sprintf(t.type,"%s","0");
    grid[0][0][1] = t;

    sprintf(t.value,"%s","istates");
    sprintf(t.type,"%s","0");
    grid[0][0][2] = t;
    
    sprintf(t.value,"%s","states");
    sprintf(t.type,"%s","0");
    grid[0][0][3] = t;

    sprintf(t.value,"%s","id");
    sprintf(t.type,"%s","id");
    grid[0][0][4] = t;

    sprintf(t.value,"%s","CA");
    sprintf(t.type,"%s","CA");
    grid[0][0][5] = t;

    sprintf(t.value,"%s","end");
    sprintf(t.type,"%s",ENDCHAR);
    grid[0][0][6] = t;


    //Regra 2
    sprintf(t.name, "%s", "set");
    sprintf(t.type, "%s", "0");
    grid[1][1][0] = t;

    sprintf(t.name, "%s", "STATES");
    sprintf(t.type, "%s", "STATES");
    grid[1][1][1] = t;

    sprintf(t.value,"%s","end");
    sprintf(t.type,"%s",ENDCHAR);
    grid[1][1][2] = t;

    //Regra 3
    sprintf(t.name, "%s", "set");
    sprintf(t.type, "%s", "0");
    grid[2][2][0] = t;

    sprintf(t.name, "%s", "STATES");
    sprintf(t.type, "%s", "STATES");
    grid[2][2][1] = t;

    sprintf(t.name, "%s", "INITIAL");
    sprintf(t.type, "%s", "INITIAL");
    grid[2][2][2] = t;

    sprintf(t.value,"%s","end");
    sprintf(t.type,"%s",ENDCHAR);
    grid[2][2][3] = t;

    //Regra 4
    sprintf(t.name, "%s", "set");
    sprintf(t.type, "%s", "0");
    grid[3][3][0] = t;

    sprintf(t.name, "%s", "NAMES");
    sprintf(t.type, "%s", "NAMES");
    grid[3][3][1] = t;

    sprintf(t.value,"%s","end");
    sprintf(t.type,"%s",ENDCHAR);
    grid[3][3][2] = t;

    //Regra 5
    sprintf(t.name, "%s", "t_set");
    sprintf(t.type, "%s", "0");
    grid[4][4][0] = t;

    sprintf(t.name, "%s", "TRANSITIONS");
    sprintf(t.type, "%s", "TRANSITIONS");
    grid[4][4][1] = t;

    sprintf(t.value,"%s","end");
    sprintf(t.type,"%s",ENDCHAR);
    grid[4][4][2] = t;

    //Regra 6
    sprintf(t.name, "%s", "t_set");
    sprintf(t.type, "%s", "0");
    grid[5][5][0] = t;

    sprintf(t.name, "%s", "]");
    sprintf(t.type, "%s", "]");
    grid[5][5][1] = t;

    sprintf(t.name, "%s", "data_constraint");
    sprintf(t.type, "%s", "0");
    grid[5][5][2] = t;

    sprintf(t.name, "%s", "[");
    sprintf(t.type, "%s", "[");
    grid[5][5][3] = t;

    sprintf(t.name, "%s", ")");
    sprintf(t.type, "%s", ")");
    grid[5][5][4] = t;

    sprintf(t.name, "%s", "set");
    sprintf(t.type, "%s", "0");
    grid[5][5][5] = t;

    sprintf(t.name, "%s", "(");
    sprintf(t.type, "%s", "(");
    grid[5][5][6] = t;

    sprintf(t.name, "%s", "id");
    sprintf(t.type, "%s", "id");
    grid[5][5][7] = t;

    sprintf(t.name, "%s", ":");
    sprintf(t.type, "%s", ":");
    grid[5][5][8] = t;

    sprintf(t.name, "%s", "id");
    sprintf(t.type, "%s", "id");
    grid[5][5][9] = t;
    //a regra acima n tem endchar pq ela usa todo o espaço reservado para regras

    //Regra 7
    // colocar simbolo para representar epsilon
    sprintf(t.name, "%s", "epsilon");
    sprintf(t.type, "%s", "");
    grid[5][28][0] = t;

    //Regra 8
    

       
}

int main(int argc, char *argv[]) {

    //T_STACK* grid[18][28];
    //T_STACK*** grid = createTable();
    fillTable();

    /*FILE *ca, *out = stdout;

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

    fclose(ca);
    if(argc > 2) fclose(out);
    return 0;
    */
}
