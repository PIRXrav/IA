#include <stdio.h>
#include <stdlib.h>


//=============================================================== NETWORK ================================================================

/* driver */
typedef double t_val;
typedef double t_pds;

/* fonction d'activation */
#include <math.h>
typedef enum e_activation e_activation;
enum e_activation{
    RAMPE       = 0,
    ECHELON     = 1,
    TANH        = 2,
};

t_val fonction_activation(e_activation type,t_val x){
    switch(type){
    case RAMPE:
        return x;
    break;
    case ECHELON:
        return (t_val)(x>=0);
    break;
    case TANH:
        return (2/(1+exp(-2*x))-1);
    break;
    default:
        exit(1);
    }
}

/* NW_network.c*/
#include <stdint.h>
#include <assert.h>
typedef struct BRANCH BRANCH;   // -------
typedef struct NODE NODE;       // ---O---
typedef struct LAYER LAYER;     // O^O^O^O
typedef struct GRAPH GRAPH;     // OOOOOOO
typedef struct NETWORK NETWORK;

struct BRANCH{
    t_pds pds;                  //Le poind de la branche
    NODE * p_node;              //Un pointeur vers le noeud d'ou elle provient
};

struct NODE{
    size_t id;                  //Indice du layer
   // t_val val0;                 //Valeur du noeud -> A supprimer

    size_t nb_branch;

    union {                        //En entrée d'un noeud, il y a soit :
        t_val * p_val;              //  -> une valeur
        BRANCH * tab_branch;        //  -> des branches
    };
    //Ajouter fonction d'activation phi

    e_activation f;
};

struct LAYER{
    size_t nb_node;
    NODE * tab_node;            //Le tableau de neuronne dans le layer
};

struct GRAPH{
    size_t nb_layer;
    LAYER * tab_layer;          //le tableau de layer
};

struct NETWORK{                 //Cette sructure facilite l'utilisateur
    GRAPH p;
    size_t nb_input;
    t_val * tab_input;          //Les valeurs en entrée du réseau
    size_t nb_output;
    t_val * tab_output;         //Les valeurs en sortie du réseau
};

NETWORK * init_network(size_t NT,size_t T[]){
    //Initialisation du network
    NETWORK * n = NULL;
    n = (NETWORK*)calloc(1,sizeof(NETWORK));
    n->nb_input = T[0];
    n->tab_input = calloc(n->nb_input,sizeof(t_val));
    n->nb_output = T[NT-1];
    n->tab_output = calloc(n->nb_output,sizeof(t_val));

    //Initialisation Graph (dark box)
    //Initialisation des layes
    n->p.nb_layer  = NT;
    //printf("NB layer = %d\n",n->p.nb_layer);
    n->p.tab_layer = (LAYER*)calloc(n->p.nb_layer,sizeof(LAYER)); assert(n->p.tab_layer);
    size_t i_layer;
    for(i_layer = 0;i_layer < n->p.nb_layer;i_layer++){
        //Initialisation nodes
        n->p.tab_layer[i_layer].nb_node  = T[i_layer];
        n->p.tab_layer[i_layer].tab_node = calloc(n->p.tab_layer[i_layer].nb_node,sizeof(NODE));
        size_t i_node;
        for(i_node = 0;i_node < n->p.tab_layer[i_layer].nb_node ;i_node++){
            n->p.tab_layer[i_layer].tab_node[i_node].id = i_layer;
            n->p.tab_layer[i_layer].tab_node[i_node].f  = RAMPE;
            //Initialisation Branch (SSI i_layer > 0)
            if(i_layer == 0){//Le neurone n'a pas de branches mais des valeurs
                n->p.tab_layer[i_layer].tab_node[i_node].nb_branch = 0;
                n->p.tab_layer[i_layer].tab_node[i_node].p_val = &n->tab_input[i_node]; //Ptr vers les données du network
            }
            else{
                n->p.tab_layer[i_layer].tab_node[i_node].nb_branch  = T[i_layer-1];
                n->p.tab_layer[i_layer].tab_node[i_node].tab_branch = calloc(T[i_layer-1],sizeof(BRANCH));
                size_t i_branch;
                for(i_branch = 0; i_branch < n->p.tab_layer[i_layer].tab_node[i_node].nb_branch ; i_branch++){
                    n->p.tab_layer[i_layer].tab_node[i_node].tab_branch[i_branch].p_node  = &n->p.tab_layer[i_layer-1].tab_node[i_branch];
                    n->p.tab_layer[i_layer].tab_node[i_node].tab_branch[i_branch].pds     = 1.0f;// LE POID DE LA BRANCHE
                }
            }
        }
    }
    return n;
}

//ptr vers element desire allege syntaxe
BRANCH * NW_get_branch(NETWORK * n, size_t layer, size_t node, size_t branch){
    return &n->p.tab_layer[layer].tab_node[node].tab_branch[branch];
}
NODE * NW_get_node(NETWORK * n, size_t layer, size_t node){
    return &n->p.tab_layer[layer].tab_node[node];
}
LAYER * NW_get_layer(NETWORK * n, size_t layer){
    return &n->p.tab_layer[layer];
}


//Initialisation apres lecture du fichier
void NW_set_branch(BRANCH * branche,t_pds pds){
    branche->pds = pds;
}
void NW_set_node(NODE * node,e_activation f,t_pds pds){
    node->f=f;
    size_t i;
    for(i=0;i<node->nb_branch;i++){
        NW_set_branch(&node->tab_branch[i],pds);
    }
}
void NW_set_layer(LAYER * layer,e_activation f,t_pds pds){
    size_t i;
    for(i=0;i<layer->nb_node;i++){
        NW_set_node(&layer->tab_node[i],f,pds);
    }
}

NETWORK * NW_cpy(NETWORK * n){
    NETWORK * o = NULL;
    o = (NETWORK*)calloc(1,sizeof(NETWORK));
    o->nb_input = n->nb_input;
    o->tab_input = calloc(o->nb_input,sizeof(t_val));
    o->nb_output = n->nb_output;
    o->tab_output = calloc(o->nb_output,sizeof(t_val));

    //Initialisation Graph (dark box)
    //Initialisation des layes
    o->p.nb_layer  = n->p.nb_layer;
    printf("NB layer = %d\n",o->p.nb_layer);
    o->p.tab_layer = (LAYER*)calloc(o->p.nb_layer,sizeof(LAYER)); assert(o->p.tab_layer);
    size_t i_layer;
    for(i_layer = 0;i_layer < o->p.nb_layer;i_layer++){
        //Initialisation nodes
        o->p.tab_layer[i_layer].nb_node  = n->p.tab_layer[i_layer].nb_node;
        o->p.tab_layer[i_layer].tab_node = calloc(o->p.tab_layer[i_layer].nb_node,sizeof(NODE));
        size_t i_node;
        for(i_node = 0;i_node < n->p.tab_layer[i_layer].nb_node ;i_node++){
            o->p.tab_layer[i_layer].tab_node[i_node].id = n->p.tab_layer[i_layer].tab_node[i_node].id;
            o->p.tab_layer[i_layer].tab_node[i_node].f  = n->p.tab_layer[i_layer].tab_node[i_node].f;
            //Initialisation Branch (SSI i_layer > 0)
            if(i_layer == 0){//Le neurone n'a pas de branches mais des valeurs
                o->p.tab_layer[i_layer].tab_node[i_node].nb_branch = 0;
                o->p.tab_layer[i_layer].tab_node[i_node].p_val = &o->tab_input[i_node]; //Ptr vers les données du network
            }
            else{
                o->p.tab_layer[i_layer].tab_node[i_node].nb_branch  = n->p.tab_layer[i_layer].tab_node[i_node].nb_branch;
                o->p.tab_layer[i_layer].tab_node[i_node].tab_branch = calloc(o->p.tab_layer[i_layer].tab_node[i_node].nb_branch,sizeof(BRANCH));
                size_t i_branch;
                for(i_branch = 0; i_branch < o->p.tab_layer[i_layer].tab_node[i_node].nb_branch ; i_branch++){
                    o->p.tab_layer[i_layer].tab_node[i_node].tab_branch[i_branch].p_node  = &o->p.tab_layer[i_layer-1].tab_node[i_branch];
                    o->p.tab_layer[i_layer].tab_node[i_node].tab_branch[i_branch].pds     = n->p.tab_layer[i_layer].tab_node[i_node].tab_branch[i_branch].pds;
                }
            }
        }
    }
    return o;
}

void NW_free(NETWORK *n){
    size_t i_layer;
    for(i_layer = 0;i_layer < n->p.nb_layer;i_layer++){
        size_t i_node;
        for(i_node = 0;i_node < n->p.tab_layer[i_layer].nb_node ;i_node++){
            if(i_layer == 0){//Le neurone n'a pas de branches mais des valeurs
            }
            else{
                free(n->p.tab_layer[i_layer].tab_node[i_node].tab_branch);
            }
        }
        free(n->p.tab_layer[i_layer].tab_node);
    }
    free(n->p.tab_layer);
    free(n->tab_output);
    free(n->tab_input);
    free(n);

    return n;
}




t_val node_value(NODE * node){//Fonction récursive
    if(node->nb_branch == 0){//pas de branche -> On retourne sa valeur
        return *(node->p_val);
    }else{
        //Sommation
        t_val value = 0;
        size_t ibranch;
        for(ibranch = 0; ibranch < node->nb_branch;ibranch ++){
            value += node->tab_branch[ibranch].pds * node_value(node->tab_branch[ibranch].p_node);
        }
        //Activation
        value = fonction_activation(node->f,value);
        return value;
    }
}

int main(void)
{
    NETWORK *n = NULL;
    size_t init[] = {1000,1000,1000};

    size_t i=1;
    while(i++){//Fuite de memoire
        n = init_network(3,init);
        //NW_free(n);
        printf("%d\r",i);
    }

    NW_set_layer(&n->p.tab_layer[0],RAMPE,1);
    NW_set_layer(&n->p.tab_layer[1],TANH,1);
    NW_set_layer(&n->p.tab_layer[2],ECHELON,1);


    //printf("test %d\n",p.tab_node[1][2].nb_branch);
    n->tab_input[0] = 1;
    n->tab_input[1] = 0;

   printf("Z = %f",node_value(&n->p.tab_layer[n->p.nb_layer-1].tab_node[0]));


    printf("Hello world!\n");
    return 0;
}
