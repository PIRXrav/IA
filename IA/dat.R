#Définition du graph;
#G=nb_layer
#C[layer]=nb_neurones_sur_layer
G=3 
C[0]=2 
C[1]=3
C[2]=1

#Définition d'une layer; 
#Tous les neurones de la layer herite les propriete de celle ci;
#L[layer]=activation,pds_branche
L[0]=RAMPE,1
L[1]=TANH,1
L[2]=ECHELON,1

#Définition d'un node;
#Tous les branches du neurone herite de celui ci;
#S[layer][node]=activation,pd_branche
#N[0][0]=RAMPE,1

#Définition d'une branche
#B[layer][node][branche]=pds


