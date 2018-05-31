#include "syntax.c"

//Non-terminals used to structure a data_constraint
#define DC_IGNORE {"F\'","E\'","T\'"}
#define N_DC_IGNORE 3
#define L_DC_IGNORE 2

Node* search(Node* tree,char* type){
    //depth search that returns first ocurrence with "content = type" in tree
    if(!tree) return NULL;

    Node *p1, *p2;

    if(strcmp(tree->content,type)==0){
        return tree;
    }
    else{
        p1 = tree->child;
        while(p1){
            p2 = search(p1,type);
            if(p2){
                return p2;
            }
            p1 = p1->sibling;
        }
        return NULL;
    }
}

N_LIST* find_ids(Node* tree){
    N_LIST *list = NULL;
    if(!tree) return NULL;
    if(strcmp(tree->content,"id")==0){
        list = (N_LIST*) malloc(sizeof(N_LIST));
        list->node = tree->child;
        list->next = NULL;
        return list;
    }
    else{
        Node *p = tree->child;
        N_LIST *aux = NULL, *l = NULL;
        while(p){
            aux = find_ids(p);
            if(!list){
                list = aux;
            }
            else{
                l = list;
                while(l->next){
                    l = l->next;
                }
                l->next = aux;
            }
            p = p ->sibling;
        }
        return list;
    }
}

N_LIST* find(char* type){
    //used to find states, initial states and names id's

    Node *p;
    N_LIST *list;

    p = search(syntaxTree,type);
    list = find_ids(p);
    return list;
}

N_LIST* set_difference(N_LIST* t1,N_LIST* t2){
    N_LIST *list = NULL, *aux = NULL, *temp=NULL;
    int flag;

    while(t1){
        flag = 0;
        aux = t2;
        while( aux && !flag ){
            if(strcmp(t1->node->content,aux->node->content)==0){
                flag = 1;
            }
            aux = aux->next;
        }

        if(!flag){
            temp = (N_LIST*) malloc(sizeof(N_LIST));
            temp->node = t1->node;
            temp->next = NULL;
            if(!list){
                list = temp;
            }
            else{
                aux = list;
                while(aux->next){
                    aux = aux->next;
                }
                aux->next = temp;
            }
        }

        t1 = t1->next;
    }

    return list;
}

char* getsDataConstraint(Node *t){
    char* result = "";
    if(!t) return result;

    // printf("%s\n",t->content);
    //
    // Node *teste = t->child;
    // while(teste){
    //     printf("%s\n",teste);
    //     teste = teste->sibling;
    // }
    //
    // return result;

    Node *aux;
    int flag, i;
    if(!(t->child)){
        char ignore[N_DC_IGNORE][L_DC_IGNORE] = DC_IGNORE;
        flag = i = 0;
        while((!flag)&&(i<N_DC_IGNORE)){
            if(strcmp(t->content,ignore[i])==0) flag = 1;
            i++;
        }

        if(!flag){
            return t->content;
        }
        else{
            return result;
        }
    }
    else{
        aux = t->child;
        while(aux){
            strcat(result,getsDataConstraint(aux));
            aux = aux->sibling;
        }
        return result;
    }
}

int main(){
    generateSyntaxTree("rules.txt","fifo_resultado.txt");

    FILE *dest;
    N_LIST *states, *istates, *names, *temp;
    Node *tempTree, *tempTree2;
    states = find("states");
    istates = find("istates");
    names = find("names");

    //VAR
    dest = fopen("fifo.smv","w");
    char *line = "MODULE main\nVAR\n";
    fprintf(dest,"%s",line);

    fprintf(dest,"    cs : {");
    temp = states;
    if(!temp){
        printf("Error: There's no states' id's in the syntaxTree\n");
        return 1;
    }
    fprintf(dest,"%s",temp->node->content);
    temp = temp->next;
    while(temp){
        fprintf(dest,",%s",temp->node->content);
        temp = temp->next;
    }
    fprintf(dest,"};\n");

    temp = names;
    while(temp){
        fprintf(dest,"    %s : {",temp->node->content);
        fprintf(dest,"NULL,0,1};\n"); //data domain of the port
        temp = temp->next;
    }

    //ASSIGN
    fprintf(dest,"ASSIGN\n");

    //init
    fprintf(dest,"    init(cs) := {");
    temp = istates;
    fprintf(dest,"%s",temp->node->content);
    temp = temp->next;
    while(temp){
        fprintf(dest,",%s",temp->node->content);
        temp = temp->next;
    }
    fprintf(dest,"};\n");

    temp = names;
    while(temp){
        fprintf(dest,"    init(%s) := {",temp->node->content);
        fprintf(dest,"NULL,0,1};\n");
        temp = temp->next;
    }

    //next
    fprintf(dest,"\tnext(cs) := case\n");
    //t_set -> id : id ( set ) [ data_constraint ]  t_set
    tempTree = search(syntaxTree,"t_set");
    char* secondId;
    while(tempTree->child){
        tempTree = tempTree->child;
        //gets the first id
        fprintf(dest,"\t\t(cs = %s) &",tempTree->child->content);

        //gets the second id
        tempTree = tempTree->sibling->sibling;
        secondId = tempTree->child->content;

        //gets set
        tempTree = tempTree->sibling->sibling;
        //gets names - set
        temp = set_difference(names,find_ids(tempTree));
        while(temp){
            fprintf(dest," (%s = NULL) &",temp->node->content);
            temp = temp->next;
        }

        //error in data_constraint
        break;
        //gets data_constraint
        tempTree = tempTree->sibling->sibling->sibling;
        char* dc = getsDataConstraint(tempTree);
        printf("dc\n");
        fprintf(dest," (%s)",dc);
        tempTree = tempTree->sibling->sibling;

        fprintf(dest,": %s;\n",secondId);
    }


    fclose(dest);

}
