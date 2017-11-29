#define LMAXTOKEN 255

#include<stdio.h>
#include<stdlib.h>
#include<string.h>

typedef struct node{
    char content[LMAXTOKEN];
    struct node *child, *sibling;
}Node;

typedef struct{
    char name[LMAXTOKEN];
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

/*
int strcmp(char n1[], char n2[]){
    int i=0;
    while((n1[i] != '\0') && (n2[i] != '\0')){
        if(n1[i] != n2[i])
            return 0;
        ++i;
    }
    if((n1[i] == '\0') && (n2[i] == '\0'))
        return 1;
    else
        return 0;
}*/


T_STACK*** createTable(){
  int m,n;
  m = 17;
  n = 28;

  T_STACK*** grid = (T_STACK***) malloc(m*sizeof(T_STACK**));

  for(int i=0;i<m;i++){
    grid[i] = (T_STACK**) malloc(n*sizeof(T_STACK*));
  }

  /*

  Token t;
  //Regra 1
  sprintf(t.name, "%s", "transitions");
  sprintf(t.type, "%s", "0");
  T_STACK* s = newStack(t,NULL);

  sprintf(t.name, "%s", "names");
  sprintf(t.type, "%s", "0");
  push(&s,t);

  sprintf(t.name, "%s", "istates");
  sprintf(t.type, "%s", "0");
  push(&s,t);

  sprintf(t.name, "%s", "states");
  sprintf(t.type, "%s", "0");
  push(&s,t);

  sprintf(t.name, "%s", "id");
  sprintf(t.type, "%s", "id");
  push(&s,t);

  sprintf(t.name, "%s", "CA");
  sprintf(t.type, "%s", "CA");
  push(&s,t);

  grid[0][0] = s;

  //Regra 2
  //libera(s);
  sprintf(t.name, "%s", "set");
  sprintf(t.type, "%s", "0");
  s = newStack(t);

  sprintf(t.name, "%s", "STATES");
  sprintf(t.type, "%s", "STATES");
  push(&s,t);

  grid[1][1] = s;

  //Regra 3
  //libera(s);
  sprintf(t.name, "%s", "set");
  sprintf(t.type, "%s", "0");
  s = newStack(t);

  sprintf(t.name, "%s", "STATES");
  sprintf(t.type, "%s", "STATES");
  push(&s,t);

  sprintf(t.name, "%s", "INITIAL");
  sprintf(t.type, "%s", "INITIAL");
  push(&s,t);

  grid[2][2] = s;

  //Regra 4
  //libera(s);
  sprintf(t.name, "%s", "set");
  sprintf(t.type, "%s", "0");
  s = newStack(t);

  sprintf(t.name, "%s", "NAMES");
  sprintf(t.type, "%s", "NAMES");
  push(&s,t);

  grid[3][3] = s;

  //Regra 5
  //libera(s);
  sprintf(t.name, "%s", "t_set");
  sprintf(t.type, "%s", "0");
  s = newStack(t);

  sprintf(t.name, "%s", "TRANSITIONS");
  sprintf(t.type, "%s", "TRANSITIONS");
  push(&s,t);

  grid[4][4] = s;

  //Regra 6
  //libera(s);
  sprintf(t.name, "%s", "t_set");
  sprintf(t.type, "%s", "0");
  s = newStack(t);

  sprintf(t.name, "%s", "]");
  sprintf(t.type, "%s", "]");
  push(&s,t);

  sprintf(t.name, "%s", "data_constraint");
  sprintf(t.type, "%s", "0");
  push(&s,t);

  sprintf(t.name, "%s", "[");
  sprintf(t.type, "%s", "[");
  push(&s,t);

  sprintf(t.name, "%s", ")");
  sprintf(t.type, "%s", ")");
  push(&s,t);

  sprintf(t.name, "%s", "set");
  sprintf(t.type, "%s", "0");
  push(&s,t);

  sprintf(t.name, "%s", "(");
  sprintf(t.type, "%s", "(");
  push(&s,t);

  sprintf(t.name, "%s", "id");
  sprintf(t.type, "%s", "id");
  push(&s,t);

  sprintf(t.name, "%s", ":");
  sprintf(t.type, "%s", ":");
  push(&s,t);

  sprintf(t.name, "%s", "id");
  sprintf(t.type, "%s", "id");
  push(&s,t);

  grid[5][5] = s;

  //Regra 7
  //libera(s);
  // colocar simbolo para representar epsilon
  grid[5][27] = s;

  */
}

int main(int argc, char *argv[]) {

    //T_STACK* grid[17][28];
    T_STACK*** grid = createTable();

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
