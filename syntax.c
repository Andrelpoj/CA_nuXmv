#define LMAXTOKEN 255
#define ENDSYMBOL "$"
#define EPSILON epsilon

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

//Global definitions
char NONTERMINALS[LMAXTOKEN][LMAXTOKEN];
char TERMINALS[LMAXTOKEN][LMAXTOKEN];
R_LIST* RULES = NULL;
//R_LIST* parsingTable[LMAXTOKEN][LMAXTOKEN];
int parsingTable[LMAXTOKEN][LMAXTOKEN];
T_LIST* firstTable[LMAXTOKEN];
T_LIST* followTable[LMAXTOKEN];
Token* endSymbol;

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
    return 0;
}

int insert(T_LIST** list, Token* token){
	//returns 1 if the token was inserted
	//returns 0 if nothing changed
	
    T_LIST* temp = *list;
    int exists=0;

    while(temp && !exists){
        if((tkncmp(temp->token,token))) exists = 1;
        temp = temp->next;
    }
    
    if(!exists){
		append(list,token);
    	return 1;
    }
    else{
    	return 0;
    }
}

void delete(T_LIST** list, Token* t){
    if(list == NULL) return;
    if(*list == NULL) return;
    if(t == NULL) return;

    T_LIST* p1 = *list;
    T_LIST* p2;

    //printf("type: %s",p1->token->type);

    if(tkncmp(t,p1->token)){
        //printf("deletando: %s\n",p1->token->value);
        *list = p1->next;
        free(p1);
    }
    else{
        while(p1){
            p2 = p1;
            p1 = p1->next;

            if(!p1) break;
            if(tkncmp(t,p1->token)){
                //printf("deletando: %s\n",p1->token->value);
                p2->next = p1->next;
                free(p1);
                break;
            }
        }
    }
}

R_LIST* newRule(T_LIST* rule){
	if(!rule) return NULL;
	
	R_LIST* list = (R_LIST*) malloc(sizeof(R_LIST));
    list->rule = rule;
    list->next = NULL;
    return list;
}

void append_rule(R_LIST** list, T_LIST* rule){
    if(!rule) return;
    
    if(! *list){
        *list = newRule(rule);
    }
    else{
        R_LIST* r = *list;
        while(r->next){
            r = r->next;
        }
        r->next = newRule(rule);
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

int findNonTerminalIndex(char type[LMAXTOKEN]){
    int i=0;
    while(strlen(NONTERMINALS[i])!=0){
        if(strcmp(NONTERMINALS[i], type) == 0){
            return i;
        }
        i++;
    }
    return -1;
}

int findTerminalIndex(char type[LMAXTOKEN]){
    int i=0;
    while(strlen(TERMINALS[i])!=0){
        if(strcmp(TERMINALS[i], type) == 0){
            return i;
        }
        i++;
    }
    return -1;
}

void nextNT(int index,int* rightSideNT, int size){
    //procura regras da forma:
    //index : vetor[0] vetor[1] ... vetor[n] X Y
    //adiciona X ao first de index
	
    Token* epsilon = newToken("epsilon","epsilon");
	
	/*
    printf("Index: %s,",NONTERMINALS[index]);
    int j;
    for(j=0;j<size;j++){
        printf(" %s |",NONTERMINALS[ rightSideNT[j] ]);
    }
    printf("Size: %d\n",size);
	*/

    R_LIST* r = RULES;
    while(r){
        T_LIST* list = r->rule;
        if (strcmp(NONTERMINALS[index],list->token->type) == 0){
            //list = list->next->next; //pula o ':'
            list = list->next;

            int flag = 0;
            int i = 0;
            for(i=0;i<size;i++){
                if(strcmp(NONTERMINALS[ rightSideNT[i] ],list->token->type) == 0){
                    list = list->next;
                    if(!list) {
						i++;
						break;
					}
                }
                else{
                    flag = 1;
                    break;
                }
            }

            //Case:
            //A : BC
            //B : b | epsilon
            //C : c | epsilon
            //so we must add epsilon to First(A)
            if((flag == 0)&&(!list)&&(i==size)){
                insert(&firstTable[index],newToken("epsilon","epsilon"));
			}

            //other cases
            if((flag == 0)&&(list)){
                //insert(&firstTable[index],newToken(list->token->value,list->token->type));
                
				if(isNonTerminal(list->token->type)){
                    int nonTerminalIndex = findNonTerminalIndex(list->token->type);
                    if(nonTerminalIndex == -1) {
                        printf("NT nÃ£o encontrado");
                        return;
                    }
                    
                    //Adds the list of terminals before calling the recursion
                    T_LIST* ntLine = firstTable[nonTerminalIndex];
		            while(ntLine){
		                if(!(tkncmp(ntLine->token,epsilon))){
		                    insert(&firstTable[index],newToken(ntLine->token->value,ntLine->token->type));
		                    //append(&firstTable[index],newToken(ntLine->token->value,ntLine->token->type));
		                }
		                ntLine = ntLine->next;
		            }

                    rightSideNT[size] = nonTerminalIndex;
                    nextNT(index,rightSideNT,size+1);
                }
                else{
                    if(isTerminal(list->token->type)){
                        insert(&firstTable[index],newToken(list->token->value,list->token->type));
                        //append(&(firstTable[index]),newToken(list->token->value,list->token->value));
                     }
                }
            }

        }
        r = r->next;
    }
}

void replaceNT(int index){
	
	T_LIST* list = firstTable[index];
    int nonTerminalIndex;

    Token* epsilon = newToken("epsilon","epsilon");

    while(list){
        if(isNonTerminal(list->token->type)){
            nonTerminalIndex = findNonTerminalIndex(list->token->type);
            if(nonTerminalIndex == -1) {
                printf("NT nÃ£o encontrado");
                return;
            }
            replaceNT(nonTerminalIndex);

            delete(&firstTable[index],list->token);
            T_LIST* ntLine = firstTable[nonTerminalIndex];
            while(ntLine){
                if(tkncmp(ntLine->token,epsilon)){
                    int rightSideNT[LMAXTOKEN];
                    rightSideNT[0] = nonTerminalIndex;
                    int size = 1;
                    nextNT(index,rightSideNT,size);
                }
                else{
                    insert(&firstTable[index],newToken(ntLine->token->value,ntLine->token->type));
					//append(&firstTable[index],newToken(ntLine->token->value,ntLine->token->type));
                }
                ntLine = ntLine->next;
            }
        }
        list = list->next;
    }
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
                //list = list->next->next; //pula o ':'
				list = list->next;

                insert(&firstTable[i],newToken(list->token->value,list->token->type));
                //append(&(firstTable[i]),newToken(list->token->value,list->token->value));
            }
            r = r->next;
        }
    }

    //Replaces all NT for T
    for(i=0;i<quant_NT;i++){
        replaceNT(i);
    }

    /*
    for(i=0;i<quant_NT;i++){
        T_LIST* l = firstTable[i];
        printf("%s: ",NONTERMINALS[i]);
        while(l){
            printf("%s ",l->token->type);
            l = l->next;
        }
        printf("\n");
    }
    */
}

void fillFollowTable(int quant_NT){
	int i = 0;
    for(i=0;i<LMAXTOKEN;i++){
        followTable[i] = NULL;
    }
	
	R_LIST* r = NULL;
	T_LIST* list = NULL;
	T_LIST* symbol = NULL;
	T_LIST* temp = NULL;
	int orig, dest, index;
	int changed = 1;
	int ins = 0;
	
	Token* epsilon = newToken("epsilon","epsilon");
	int hasEpsilon = 0;
	
	//inserts the endSymbol in Follow of the first symbol(left side) of the first rule
	r = RULES;
	list = r->rule;
	index = findNonTerminalIndex(list->token->type);
	insert(&followTable[index],newToken(endSymbol->value,endSymbol->type));			
	
	
	while(changed){
		r = RULES;
		changed = 0;
		while(r){
			list = r->rule;
			
			//saves the index of the left side of the rule symbol
			index = findNonTerminalIndex(list->token->type);
			list = list->next;
			
			while(list){
				if(isNonTerminal(list->token->type)){
					
					//tests if the symbol is not the last one
					if(list->next){
						symbol = list->next;
						
						do{
							dest = findNonTerminalIndex(list->token->type);
							orig = findNonTerminalIndex(symbol->token->type);
							
							if(orig!=-1){
								temp = firstTable[orig];
								hasEpsilon = 0;
								while(temp){
									if(tkncmp(temp->token,epsilon)){
										hasEpsilon = 1;
									}
									else{
										ins = insert(&followTable[dest],newToken(temp->token->value,temp->token->type));
										changed = changed || ins;					
									}
									temp = temp->next;
								}
								
								if((!symbol->next)&&(hasEpsilon)){
									temp = followTable[index];
									while(temp){
										ins = insert(&followTable[dest],newToken(temp->token->value,temp->token->type));	
										changed = changed || ins;
										
										temp = temp->next;				
									}	
								}
								
							}
							else{
								//if the symbol is not a non terminal gets the index from terminals
								orig = findTerminalIndex(symbol->token->type);
								
								if(orig==-1){
									printf("Erro, símbolo não permitido usado na regra");
									return;
								}
								
								ins = insert(&followTable[dest],newToken(symbol->token->value,symbol->token->type));
								changed = changed || ins;
							}
							
							symbol = symbol->next;
						}while((symbol)&&(hasEpsilon));
						
					}
					
					//Case:
					// A -> B
					if(!list->next){
						dest = findNonTerminalIndex(list->token->type);
						
						temp = followTable[index];
						while(temp){
							ins = insert(&followTable[dest],newToken(temp->token->value,temp->token->type));	
							changed = changed || ins;
							
							temp = temp->next;				
						}
					}		
				}
				list = list->next;
			}
			r = r->next;
		}
	}
}


void fillParsingTable(int quant_T){
    T_LIST *first, *follow, *temp;
    Token* epsilon = newToken("epsilon","epsilon");
    R_LIST* r = NULL;
    T_LIST* list = NULL;
    int i=0, j=0, flag=0, hasEpsilon = 0, cont = 0;
    
    for(i=0;i<LMAXTOKEN;i++){
        for(j=0;j<LMAXTOKEN;j++){
		parsingTable[i][j] = 0;	
		}
    }
    
	r = RULES;
    while(r){
    	cont++;
        list = r->rule;
        i = findNonTerminalIndex(list->token->type);
		list = list->next;
		
        if(isNonTerminal(list->token->type)){
        	do{
		    	//CASE: A -> B
		    	//      B -> epsilon	
		    	if(!list){
		    		follow = followTable[i];
				
					while(follow){
						if(tkncmp(follow->token,endSymbol)){
							//j = findTerminalIndex(epsilon);
							j = quant_T - 1;
						}
						else{
							j = findTerminalIndex(follow->token->type);	
						}
						
						//append_rule(&(parsingTable[i][j]), r->rule);
						parsingTable[i][j] = cont;
						
						follow = follow->next;
					}
				}
				else{
					hasEpsilon = 0;
					first = firstTable[ findNonTerminalIndex(list->token->type) ];
					while(first){
			    		if(tkncmp(first->token,epsilon)){
			    			hasEpsilon = 1;
			    			list = list->next;
						}
						else{
							j = findTerminalIndex(first->token->type);
							
							//append_rule(&(parsingTable[i][j]), r->rule);
							parsingTable[i][j] = cont;
						}
						first = first->next;
					}	
				}
			}while(hasEpsilon);
		}
		else{
			if(tkncmp(list->token,epsilon)){
				printf("%s \n",NONTERMINALS[i]);
				follow = followTable[i];
				
				while(follow){
					if(tkncmp(follow->token,endSymbol)){
						//j = findTerminalIndex(epsilon);
						j = quant_T - 1;
					}
					else{
						j = findTerminalIndex(follow->token->type);	
					}
					//append_rule(&(parsingTable[i][j]), r->rule);
					parsingTable[i][j] = cont;	
					
					follow = follow->next;
				}
			}
			else{
				j = findTerminalIndex(list->token->type);
				
				//append_rule(&(parsingTable[i][j]), r->rule);
				parsingTable[i][j] = cont;		
			}
		}
		r = r->next;
    }
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
    int i,j;

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

    //Adds epsilon to the Terminals
    sprintf(TERMINALS[i],"%s","epsilon");
    i++;
    quant_T = i;
    
    i = 0;
    while(strlen(TERMINALS[i])!=0){
        printf("%s\n",TERMINALS[i]);
        i++;
    }
    

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

    while ( fgets (temp , LMAXTOKEN , arq) != NULL ) {
//      printf("%d",strlen(temp));
//      printf("%s\n",temp);
        char *tkn = strtok(temp," ");
        T_LIST* rule = NULL;

		int mark=0;
		char marker[2]=":";

        while (tkn != NULL){
        	//eliminates the marker
            if((strcmp(marker,tkn)==0)&&(mark==0)){
            	mark = 1;
			}
            else{
				//printf("The token is:  %s\n", tkn);
	            Token* new = findType(tkn);
	            append(&rule,new);	
			}
            
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
    
    fillFirstTable(quant_NT);

	printf("FIRST TABLE\n*****************\n");
    for(i=0;i<quant_NT;i++){
        T_LIST* list = firstTable[i];
        printf("%s: ",NONTERMINALS[i]);
        while(list){
            printf("%s ",list->token->type);
            list = list->next;
        }
        printf("\n");
    }
    printf("\n*****************\n");

	fillFollowTable(quant_NT);
	
	printf("\n\n\n");
	printf("FOLLOW TABLE\n*****************\n");
    for(i=0;i<quant_NT;i++){
        T_LIST* list = followTable[i];
        printf("%s: ",NONTERMINALS[i]);
        while(list){
            printf("%s ",list->token->type);
            list = list->next;
        }
        printf("\n");
    }
    printf("\n*****************\n");

    
	fillParsingTable(quant_T);
	
	printf("\n\n\n");
	printf("PARSING TABLE\n*****************\n");
	for(i=0;i<quant_NT;i++){
        printf("%s: ",NONTERMINALS[i]);
		for(j=0;j<quant_T;j++){
			printf("| %d",parsingTable[i][j]);
		}
		printf("\n");
    }
    printf("\n*****************\n");


    fclose(arq);
    return NULL;

}

int main(int argc, char *argv[]) {
	endSymbol = newToken(ENDSYMBOL,ENDSYMBOL);
	
    Token** grid = createTable("rules.txt");


    // T_LIST* table[3];
    // table[0] = NULL;
    // table[1] = NULL;
    // table[2] = NULL;
    //
    //
    // Token* t = newToken("CA","CA");
    // append(&table[0],t);
    // t = newToken("STATES","STATES");
    // append(&table[0],t);
    // t = newToken("INITIAL","INITIAL");
    // append(&table[0],t);
    //
    // T_LIST* temp = table[0];
    // while(temp){
    //     printf("value: %s type: %s\n",temp->token->value,temp->token->type);
    //     temp = temp->next;
    // }
    //
    // printf("\n\n\n");
    //
    // delete(&table[0],t);
    // delete(&table[0],newToken("CA","CA"));
    // delete(&table[0],newToken("STATES","STATES"));
    //
    // temp = table[0];
    // while(temp){
    //     printf("value: %s type: %s\n",temp->token->value,temp->token->type);
    //     temp = temp->next;
    // }





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
