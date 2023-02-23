 #include "ficheros_basico.h"

/*---------------------------------------------------------------------------------------------------------
* Calcula el número de bloques necesarios para el mapa de bits 
* Input:    nbloques: número de bloques reservados
* Output:   número de bloques necesarios para el mapa de bits 
---------------------------------------------------------------------------------------------------------*/
 
 int tamMB(unsigned int nbloques){

    int res = (nbloques / 8) / BLOCKSIZE;
    if (res = (nbloques / 8) % BLOCKSIZE != 0){
        res += 1;
    }
    return res;

 }

/*---------------------------------------------------------------------------------------------------------
* Calcula el tamaño en bloques del array de inodos 
* Input:    ninodos: número de inodos
* Output:   tamaño en bloques del array de inodos
---------------------------------------------------------------------------------------------------------*/

 int tamAI(unsigned int ninodos){

    int res = (ninodos / INODOSIZE) / BLOCKSIZE;
    if (res = (ninodos / INODOSIZE) % BLOCKSIZE != 0){
        res += 1;
    }
    return res;

 }

 /*---------------------------------------------------------------------------------------------------------
* FUNCION
* Input:    nbloques: número de bloques reservados; ninodos: número de inodos
* Output:   OUTPUT
---------------------------------------------------------------------------------------------------------*/

 int initSB(unsigned int nbloques, unsigned int ninodos){

 }

/*---------------------------------------------------------------------------------------------------------
* FUNCION
* Input:    -
* Output:   OUTPUT
---------------------------------------------------------------------------------------------------------*/

 int initMB(){

 }

/*---------------------------------------------------------------------------------------------------------
* Inicializa la lista de nodos libres
* Input:    -
* Output:   OUTPUT
---------------------------------------------------------------------------------------------------------*/

int initAI(){

}