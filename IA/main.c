#include <stdio.h>
#include <stdlib.h>

#include <stdint.h>

typedef struct NODE NODE;
typedef struct BRANCH BRANCH;
typedef struct GRAPH GRAPH;

struct BRANCH{
    int32_t pds;
    NODE * p_node; //Un noeud
};

struct NODE{
    size_t id;       //Indice du layer
    int32_t val0;

    size_t nb_branch;
    BRANCH * tab_branch;//La liste des branches
};

struct GRAPH{
    size_t nblayer;
    NODE ** tab_node;//le tableau de noeuds

};

void init_graph(void){
    //Placement des noeuds

    //Calcul des branches
}

int32_t node_value(NODE * node){//Fonction récursive
    if(node->id == 0){//Couche 0 -> On retourne sa valeur
        return node->val0;
    }else{
        int32_t value = 0;
        size_t ibranch;
        for(ibranch = 0; ibranch < node->nb_branch;ibranch ++){
            value += node->tab_branch[ibranch].pds * node_value(node->tab_branch[ibranch].p_node);
        }
        return value;
    }
}

int main(void)
{
    const size_t L= 3;              //Nombre de layer
    const size_t NL[3]  = {2,3,1};  //Nombre de node / layer

    GRAPH p;
    p.nblayer = L;

    //initialisation des noeuds
    p.tab_node = calloc(p.nblayer,sizeof(NODE *));
    size_t layer;
    for(layer = 0;layer < p.nblayer;layer++){
        p.tab_node[layer] = calloc(NL[layer],sizeof(NODE));
    }
    for(layer = 0;layer < p.nblayer;layer++){
        size_t i_node;
        for(i_node = 0;i_node < NL[layer];i_node++){
            p.tab_node[layer][i_node].id = layer;
            p.tab_node[layer][i_node].val0 = 1;                     //BOF BOF
        }
    }

    //Initialisation des branches
    for(layer = 1;layer < p.nblayer;layer++){//V couche
        size_t i_node;
        for(i_node = 0;i_node < NL[layer];i_node++){//V node
            //Allocation des branches
            p.tab_node[layer][i_node].nb_branch = NL[layer-1];
            p.tab_node[layer][i_node].tab_branch = calloc(p.tab_node[layer][i_node].nb_branch,sizeof(BRANCH));
            //Initialisation
            size_t i_branch;
            for(i_branch = 0; i_branch < p.tab_node[layer][i_node].nb_branch;i_branch++){
                p.tab_node[layer][i_node].tab_branch[i_branch].p_node = &p.tab_node[layer-1][i_branch];
                p.tab_node[layer][i_node].tab_branch[i_branch].pds = 1; // LE POID DE LA BANCHE
            }
        }
    }

    //printf("test %d\n",p.tab_node[1][2].nb_branch);

    p.tab_node[0][0].val0 = 10;
    p.tab_node[0][1].val0 = 0;

    printf("Z = %d",node_value(&p.tab_node[p.nblayer-1][0]));


    printf("Hello world!\n");
    return 0;
}
