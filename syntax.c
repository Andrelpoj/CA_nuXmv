#define LMAXTOKEN 255

//#define THEIGHT 18
//#define TWIDTH 29
#define TDEPTH 20

#include<stdio.h>
#include<stdlib.h>
#include<string.h>

typedef struct node {
    char content[LMAXTOKEN];
    struct node *child, *sibling;
}Node;

typedef struct{
    char value[LMAXTOKEN];
    char type[LMAXTOKEN];
}Token;

typedef struct TokenList{
    Token* token;
    struct TokenList* next;
}T_LIST;

typedef struct TokenStack{
    Token* token;
    Node* node;
    struct TokenStack *next;
}T_STACK;

typedef struct RulesList{
    T_LIST* rule;
    struct RulesList* next;
}R_LIST;

//Global definitions
char NONTERMINALS[LMAXTOKEN][LMAXTOKEN];
char TERMINALS[LMAXTOKEN][LMAXTOKEN];
R_LIST* RULES = NULL;
T_LIST* firstTable[LMAXTOKEN];
T_LIST* followTable[LMAXTOKEN];

Token* newToken(char value[LMAXTOKEN], char type[LMAXTOKEN]){
    Token* t = (Token*) malloc(sizeof(Token));
    strcpy(t->value,value);
    strcpy(t->type,type);
    return t;
}

T_LIST* newList(Token* t){
    if (!t) return NULL;

    T_LIST* listNode = (T_LIST*) malloc(sizeof(T_LIST));
    listNode->token = t;
    listNode->next = NULL;
    return listNode;
}

void append(T_LIST** list, Token* t){
    if(!t) return;

    if(! *list){
        *list = newList(t);
    }
    else{
        T_LIST* p = *list;
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
    //return 0;
}

void delete(T_LIST** list, Token* t){
    T_LIST* p1 = *list;
    T_LIST* p2;

    //printf("type: %s",p1->token->type);
    if(tkncmp(t,p1->token)){
        *list = p1->next;
        free(p1);
    }
    else{
        while(p1){
            p2 = p1;
            p1 = p1->next;
            if(tkncmp(t,p1->token)){
                p2->next = p1->next;
                free(p1);
                break;
            }
        }
    }
}

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

Token* findType(char value[LMAXTOKEN]){
    //char terminals[LMAXTOKEN][LMAXTOKEN] = TERMINALS;
    // e se for id ou num?
    int i=0;
    while(strlen(TERMINALS[i])!=0){
        if(strcmp(TERMINALS[i], value) == 0){
            return newToken(value,TERMINALS[i]);
        }
        i++;
    }
    i=0;
    while(strlen(NONTERMINALS[i])!=0){
        if(strcmp(NONTERMINALS[i], value) == 0){
            return newToken(value,NONTERMINALS[i]);
        }
        i++;
    }

}

int isTerminal(char type[LMAXTOKEN]){
    int i=0;
    while(strlen(TERMINALS[i])!=0){
        if(strcmp(TERMINALS[i], type) == 0){
            return 1;
        }
        i++;
    }
    return 0;
}

int isNonTerminal(char type[LMAXTOKEN]){
    int i=0;
    while(strlen(NONTERMINALS[i])!=0){
        if(strcmp(NONTERMINALS[i], type) == 0){
            return 1;
        }
        i++;
    }
    return 0;
}

void fillFirstTable(int quant_NT){
    int i =0;
    for(i=0;i<LMAXTOKEN;i++){
        firstTable[i] = NULL;
    }

    //Fill the FirstTable
    for(i=0;i<quant_NT;i++){
        R_LIST* r = RULES;
        while(r){
            T_LIST* list = r->rule;
            if (strcmp(NONTERMINALS[i],list->token->type) == 0){
                list = list->next->next; //pula o ':'

                //append(&(firstTable[i]),list->token);
                append(&(firstTable[i]),newToken(list->token->value,list->token->type));
            }
            r = r->next;
        }
    }

    //Replaces all NT for T
    for(i=0;i<quant_NT;i++){
        T_LIST* l = firstTable[i];
        //delete(&firstTable[i],newToken("rop","rop"));
        printf("%s: ",NONTERMINALS[i]);
        while(l){
            printf("%s ",l->token->type);
            l = l->next;
        }
        printf("\n");
    }
    //return firstTable;
}

/*
T_LIST* first(char type[LMAXTOKEN]){
    T_LIST* list = NULL;
    if(isTerminal(type)){
        append(&list,newToken(type,type));
        return list;
    }
    else{
        R_LIST* r = RULES;
        while(r){
            T_LIST* temp = r->rule;
            if (strcmp(type,temp->token->type) == 0){
                temp = temp->next->next; //pula o ':'

                temp = first(temp->token->type);
                while(temp){
                    //append(&list,temp->token);
                    append(&list,newToken(temp->token->value,temp->token->type));
                    temp = temp->next;
                }
            }
            r = r->next;
        }
        return list;
    }
}*/

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

    /*
    Token* t = newToken("CA","T");
    //T_LIST* l = newList(t);  //Deixo a criacao da lista dessa forma?
    T_LIST* l = NULL;
    t = newToken("states","NT");
    append(&l,t);
    t = newToken("istates","NT");
    append(&l,t);

    T_LIST* list = l;
    while(list){
        printf("value: %s,type: %s\n",list->token->value, list->token->type);
        list = list->next;
    }
    */

    while ( fgets (temp , LMAXTOKEN , arq) != NULL ) {
//      printf("%d",strlen(temp));
//      printf("%s\n",temp);


        char *tkn = strtok(temp," ");
        T_LIST* rule = NULL;

        while (tkn != NULL){
            //printf("The token is:  %s\n", tkn);
            Token* new = findType(tkn);
            append(&rule,new);
            tkn = strtok(NULL," \n\t\r");
        }

        T_LIST* list = rule;
        while(list){
            //printf("value: %s,type: %s\n",list->token->value, list->token->type);
            list = list->next;
        }

        /*
        T_LIST* list = rule;
        while(list){
            printf("type: %s", list->token->type);
            list = list->next;
        }
        printf("\n");
        */

        if(!RULES){
            RULES = (R_LIST*) malloc(sizeof(R_LIST));
            RULES->rule = rule;
            RULES->next = NULL;
        }
        else{
            R_LIST* r = RULES;
            while(r->next){
                r = r->next;
            }
            r->next = (R_LIST*) malloc(sizeof(R_LIST));
            r->next->rule = rule;
            r->next->next = NULL;
        }
    }

    /*
    R_LIST* r = RULES;
    while(r){
        T_LIST* list = r->rule;
        while(list){
            printf("%s ", list->token->type);
            list = list->next;
        }
        printf("\n");

        r = r->next;
    }
    */

    /*
    T_LIST* firstTable[quant_NT];
    T_LIST* followTable[quant_NT];
    for(i=0;i<quant_NT;i++){
        firstTable[i] = NULL;
        followTable[i] = NULL;
    }
    */

    fillFirstTable(quant_NT);


    //firstTable = createFirstTable(quant_NT);


    /*
    for(i=0;i<quant_NT;i++){
        T_LIST* list = firstTable[i];
        printf("%s\n",NONTERMINALS[i]);
        while(list){
            printf("%s\n",list->token->type);
            list = list->next;
        }
        printf("\n");
    }
    */

    fclose(arq);
    return NULL;

}

int main(int argc, char *argv[]) {
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
