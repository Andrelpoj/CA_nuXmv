#include "syntax.c"

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
    char* aux;
    while(tempTree->child){
        tempTree = tempTree->child;
        fprintf(dest,"\t\t(cs = %s) &",tempTree->content);

        //gets the second id
        tempTree = tempTree->sibling->sibling;
        aux = tempTree->content;

        //gets set
        tempTree = tempTree->sibling->sibling;
        // tempTree2 = tempTree->child;
        // while(tempTree2){
        //     fprintf(dest,"\t\t(%s = %s) &",tempTree->content);
        // }

        //gets data_constraint
        tempTree = tempTree->sibling->sibling->sibling;
        fprintf(dest,"\t\t(cs = %s) &",tempTree->content);
    }


    fclose(dest);

}
