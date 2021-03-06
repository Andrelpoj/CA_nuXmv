#include "syntax.c"

//Non-terminals used to structure a data_constraint
#define DC_IGNORE {"F\'","E\'","T\'"}
#define N_DC_IGNORE 3
#define L_DC_IGNORE 3

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

void writesDataConstraint(Node *t,FILE *dest){
    //ajeitar a questao dos parenteses
    if(!t) return;
    int i,flag;
    Node *child = NULL;
    if(!t->child){
        char ignore[N_DC_IGNORE][L_DC_IGNORE] = DC_IGNORE;
        flag = i = 0;
        while((!flag)&&(i<N_DC_IGNORE)){
            if(strcmp(t->content,ignore[i])==0) flag = 1;
            //printf("flag: %d, ignore: %s\n",flag,ignore[i]);
            i++;
        }
        if(!flag){
            fprintf(dest," %s",t->content);
        }
    }
    else{
        child = t->child;
        while(child){
            writesDataConstraint(child,dest);
            child = child->sibling;
        }
    }
}

int main(int argc, char *argv[]){
    FILE *dest = stdout;
    if(argc == 1) {
        printf("usage: %s rules_file token_file [output_file]\n", argv[0]);
        return 0;
    }
    if(argc > 3) {
        dest = fopen(argv[3], "w");
        if(dest == NULL) {
            printf("error while opening file %s\n", argv[2]);
            return 2;
        }
    }

    generateSyntaxTree(argv[1],argv[2]);

    N_LIST *states, *istates, *names, *temp;
    Node *tempTree, *tempTree2;
    states = find("states");
    istates = find("istates");
    names = find("names");

    //VAR
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

    fprintf(dest,"    tr: boolean;\n");

    temp = names;
    while(temp){
        fprintf(dest,"    %s : {",temp->node->content);
        fprintf(dest,"NULL,0,1};\n"); //data domain of the port
        temp = temp->next;
    }

    //DEFINE
    fprintf(dest,"DEFINE transitions := ");
    tempTree = search(syntaxTree,"t_set");
    while(tempTree->child){
        tempTree = tempTree->child;
        //gets the first id
        fprintf(dest,"((cs = %s) &",tempTree->child->content);
        //jumps over the second id
        tempTree = tempTree->sibling->sibling;
        //gets set
        tempTree = tempTree->sibling->sibling;
        //gets names - set
        temp = set_difference(names,find_ids(tempTree));
        while(temp){
            fprintf(dest,"(%s = NULL) &",temp->node->content);
            temp = temp->next;
        }
        //gets data_constraint
        tempTree = tempTree->sibling->sibling->sibling;
        fprintf(dest,"%s"," (");
        writesDataConstraint(tempTree,dest);
        fprintf(dest,"%s"," ))");

        tempTree = tempTree->sibling->sibling;
        if(tempTree->child)
            fprintf(dest,"%s"," |\n    ");
        else
            fprintf(dest,"%s",";\n");
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

    fprintf(dest,"    init(tr) := FALSE;\n");

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

        //gets data_constraint
        tempTree = tempTree->sibling->sibling->sibling;
        fprintf(dest,"%s"," (");
        writesDataConstraint(tempTree,dest);
        fprintf(dest,"%s"," ) ");

        tempTree = tempTree->sibling->sibling;
        fprintf(dest,": %s;\n",secondId);
    }
    //remains in the same state if there's no transition available
    fprintf(dest,"        TRUE : cs;\n");
    fprintf(dest,"    esac;\n");

    fprintf(dest,"    next(tr) := case \n");
    fprintf(dest,"        transitions : TRUE;\n");
    fprintf(dest,"        TRUE : FALSE;\n");
    fprintf(dest,"    esac;\n");

    //variation of values in ports
    temp = names;
    while(temp){
        fprintf(dest,"    next(%s) := {",temp->node->content);
        fprintf(dest,"NULL,0,1};\n");
        temp = temp->next;
    }

    fclose(dest);

}
