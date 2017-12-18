
#define LMAXTOKEN 255

//#define THEIGHT 18
//#define TWIDTH 29
#define TDEPTH 20

#define ENDCHAR "@"

#include<stdio.h>
#include<stdlib.h>
#include<string.h>

char NONTERMINALS[LMAXTOKEN][LMAXTOKEN];
char TERMINALS[LMAXTOKEN][LMAXTOKEN];

typedef struct node {
    char content[LMAXTOKEN];
    struct node *child, *sibling;
} Node;

typedef struct{
    char value[LMAXTOKEN];
    char type[LMAXTOKEN];
}Token;

Token* newToken(char value[LMAXTOKEN], char type[LMAXTOKEN]){
    Token* t = (Token*) malloc(sizeof(Token));
    strcpy(t->value,value);
    strcpy(t->type,type);
    return t;
}

typedef struct TokenList{
    Token* token;
    struct TokenList* next;
}T_LIST;

T_LIST* newList(Token* t){
    T_LIST* listNode = (T_LIST*) malloc(sizeof(T_LIST));
    listNode->token = t;
    listNode->next = NULL;
    return listNode;
}

void append(T_LIST* list, Token* t){
    if(!list){
        list->token = t;
        list->next = NULL;
    }
    else{
        T_LIST* p = list;
        while(p->next){
            p = p->next;
        }
        p->next = newList(t);
    }
}

int tkncmp(Token* t1, Token* t2){
    if(!strcmp(t1->value,t2->value) && !strcmp(t1->type,t2->type)){
        return 1;
    }
}

void delete(T_LIST* list, Token* t){
    T_LIST* p1 = list;
    T_LIST* p2;
    while(p1->next){
        p2 = p1;
        p1 = p1->next;
        if(tkncmp(t,p1->token)){
            p2->next = p1->next;
            free(p1);
        }
    }
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
    printf("%s pushed to stack\n", t->value);
}

Token* pop(T_STACK** root)
{
    if (isEmpty(*root)){
        return NULL;
        //Token t;
        //sprintf(t.value, "%s", "");
        //sprintf(t.type, "%s", "-1");
    }

    T_STACK* temp = *root;
    *root = (*root)->next;
    Token* popped = temp->token;
    free(temp);

    //temp->next = NULL;
    return popped;
}



Token** createTable(char* orig){
    FILE *arq = fopen(orig,"r");
    if(arq == NULL) {
        printf("error while opening file %s\n", orig);
        return NULL;
    }


    char temp[LMAXTOKEN];
    char var[10] = "VAR";
    int quant_T = 0;
    int quant_NT = 0;
    int i;

    for(i=0;i<LMAXTOKEN;i++){
        sprintf(TERMINALS[i],"%s","");
    }

    fscanf(arq,"%s\n",&temp);
    i = 0;
    while(strcmp(temp,var)){
        //printf("%s\n",temp);
        sprintf(TERMINALS[i],"%s",temp);
        fscanf(arq,"%s\n",&temp);
        i++;
    }
    quant_T = i;

    /*
    i = 0;
    while(strlen(TERMINALS[i])!=0){
        printf("%s\n",TERMINALS[i]);
        i++;
    }
    */

    for(i=0;i<LMAXTOKEN;i++){
        sprintf(NONTERMINALS[i],"%s","");
    }

    sprintf(var,"%s","RULES");
    fscanf(arq,"%s\n",&temp);
    i = 0;
    while(strcmp(temp,var)){
        //printf("%s\n",temp);
        sprintf(NONTERMINALS[i],"%s",temp);
        fscanf(arq,"%s\n",&temp);
        i++;
    }
    quant_NT = i;
    /*
    i = 0;
    while(strlen(NONTERMINALS[i])!=0){
        printf("%s\n",NONTERMINALS[i]);
        i++;
    }
    */


    Token* t = newToken("CA","T");
    T_LIST* l = newList(t);  //Deixo a criacao da lista dessa forma?
    t = newToken("states","NT");
    append(l,t);
    t = newToken("istates","NT");
    append(l,t);

    T_LIST* list = l;
    while(list){
        printf("value: %s,type: %s\n",list->token->value, list->token->type);
        list = list->next;
    }
    



    Token* tableFirstFollow[quant_T+quant_NT][2];


    return NULL;

}

int main(int argc, char *argv[]) {

    //T_STACK* grid[18][28];
    //T_STACK*** grid = createTable();
    //fillTable();

    Token** grid = createTable("rules.txt");

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
