#include <stdio.h>
#include <stdlib.h>


#include <stdint.h>
#include <assert.h>
typedef double t_pds;
typedef double t_val;

typedef struct NODE NODE;
typedef struct BRANCH BRANCH;
typedef struct LAYER LAYER;
typedef struct GRAPH GRAPH;

struct BRANCH{
    t_pds pds;                  //Le poind de la branche
    NODE * p_node;              //Un pointeur vers le noeud d'ou elle provient
};

struct NODE{
    size_t id;                  //Indice du layer
    t_val val0;                 //Valeur du noeud -> A supprimer

    size_t nb_branch;
    BRANCH * tab_branch;        //Le tableau des pranche qui mene à ce neurone
};

struct LAYER{
    size_t nb_node;
    NODE * tab_node;            //Le tableau de neuronne dans le layer
};

struct GRAPH{
    size_t nb_layer;
    LAYER * tab_layer;          //le tableau de layer
};

GRAPH * init_graph(size_t NT,size_t T[]){ //{2,3,1}
    //Initialisation du graph
    GRAPH * p;
    p = (GRAPH*)calloc(1,sizeof(GRAPH)); assert(p);
    //Initialisation des layers
    p->nb_layer  = NT;
    printf("NB layer = %d\n",p->nb_layer);
    p->tab_layer = (LAYER*)calloc(p->nb_layer,sizeof(LAYER)); assert(p->tab_layer);
    size_t i_layer;
    for(i_layer = 0;i_layer < p->nb_layer;i_layer++){
        //Initialisation nodes
        p->tab_layer[i_layer].nb_node  = T[i_layer];
        p->tab_layer[i_layer].tab_node = calloc(p->tab_layer[i_layer].nb_node,sizeof(NODE));
        size_t i_node;
        for(i_node = 0;i_node < p->tab_layer[i_layer].nb_node ;i_node++){
            p->tab_layer[i_layer].tab_node[i_node].id = i_layer;
            p->tab_layer[i_layer].tab_node[i_node].val0 = 1.0f; //VALEUR EN 0 A DEL
            //Initialisation Branch (SSI i_layer > 0)
            if(i_layer != 0){
                p->tab_layer[i_layer].tab_node[i_node].nb_branch  = T[i_layer-1];
                p->tab_layer[i_layer].tab_node[i_node].tab_branch = calloc(T[i_layer-1],sizeof(BRANCH));
                size_t i_branch;
                for(i_branch = 0; i_branch < p->tab_layer[i_layer].tab_node[i_node].nb_branch ; i_branch++){
                    p->tab_layer[i_layer].tab_node[i_node].tab_branch[i_branch].p_node  = &p->tab_layer[i_layer-1].tab_node[i_branch];
                    p->tab_layer[i_layer].tab_node[i_node].tab_branch[i_branch].pds     = 1.0f;// LE POID DE LA BRANCHE
                }
            }
        }
    }
    return p;
}

t_val node_value(NODE * node){//Fonction récursive
    if(node->id == 0){//Couche 0 -> On retourne sa valeur
        return node->val0;
    }else{
        t_val value = 0;
        size_t ibranch;
        for(ibranch = 0; ibranch < node->nb_branch;ibranch ++){
            value += node->tab_branch[ibranch].pds * node_value(node->tab_branch[ibranch].p_node);
        }
        return value;
    }
}

int main(void)
{

    GRAPH * p = NULL;
    size_t init[] = {2,3,1};
    p = init_graph(3,init);



    //printf("test %d\n",p.tab_node[1][2].nb_branch);

    p->tab_layer[0].tab_node[0].val0 = 10;
    p->tab_layer[0].tab_node[1].val0 = 0;


   printf("Z = %f",node_value(&p->tab_layer[p->nb_layer-1].tab_node[0]));


    printf("Hello world!\n");
    return 0;
}
