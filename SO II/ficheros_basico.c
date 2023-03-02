 #include "ficheros_basico.h"

int binaryToDecimal(int byte[]);



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

    int res = (ninodos * INODOSIZE) / BLOCKSIZE;
    if (res = (ninodos * INODOSIZE) % BLOCKSIZE != 0){
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
      struct superbloque SB;

      SB.posPrimerBloqueMB = posSB + tamSB; //Posición del primer bloque del mapa de bits

      SB.posUltimoBloqueMB = SB.posPrimerBloqueMB + tamMB(nbloques) - 1; //Posición del último bloque del mapa de bits

      SB.posPrimerBloqueAI = SB.posUltimoBloqueMB + 1; //Posición del primer bloque del array de inodos

      SB.posUltimoBloqueAI = SB.posPrimerBloqueAI + tamAI(ninodos) - 1; //Posición del ultimo bloque del array de inodos

      SB.posPrimerBloqueDatos = SB.posUltimoBloqueAI + 1; //Posición del primer bloque de datos

      SB.posUltimoBloqueDatos = nbloques-1; //Posición del último bloque de datos

      SB.posInodoRaiz = 0; //Posición del inodo del directorio raíz en el array de inodos

      SB.posPrimerInodoLibre = 0; //Posición del primer inodo libre en el array de inodos

      SB.cantBloquesLibres = nbloques; //Cantidad de bloques libres en el SF

      SB.cantInodosLibres = ninodos; //Cantidad de inodos libres en el array de inodos

      SB.totBloques = nbloques; //Cantidad total de bloques

      SB.totInodos = ninodos; //Cantidad total de inodos
 }

/*---------------------------------------------------------------------------------------------------------
* Inicializa el mapa de bits poniendo a 1 los bits que representan los metadatos.
* Input:    -
* Output:   OUTPUT
---------------------------------------------------------------------------------------------------------*/

 int initMB(){    
    struct superbloque SB;
    bread(posSB,&SB);
    char bufferMB[BLOCKSIZE];
    //numero de BITS del mapa (es el tamaño de SB + MB + AI), cada bit representa un bloque fisico
    int nbits = SB.posPrimerBloqueDatos;

   //miramos si todos estos BITS caben en 1 bloque
   if((nbits/8)/BLOCKSIZE == 0){
      int nbytes = nbits/8;
      
      for (int i = 0; i < nbytes; i++){
         bufferMB[i] = 255; //ponemos todos los bytes a 1 (elementos de la array)
      }

      //calcular resto de bits que pueden quedar por meter
      int rest = nbits % 8;
      if(rest != 0){

         int byte[8];
         for(int i = 0; i <rest; i++){
            byte[i] = 1;
         }
         bufferMB[nbytes] = binaryToDecimal(byte);
      }else{
         bufferMB[nbytes] = 0;
      }

      

   }


    

 }

/*---------------------------------------------------------------------------------------------------------
* Función auxiliar para calcular el decimal de un número binario
* Input:    byte: array con el número binario
* Output:   decimal: valor decimal
---------------------------------------------------------------------------------------------------------*/

 //funcion auxiliar para calcular el decimal de un numero binario
 int binaryToDecimal(int byte[]) {
    int decimal = 0;
    int base = 1;
    int i;
    
    // Recorre el array del byte en binario en orden inverso
    for (i = 7; i >= 0; i--) {
        // Si el bit actual es 1, suma su valor a decimal
        if (byte[i] == 1) {
            decimal += base;
        }
        // Actualiza la base multiplicando por 2 para pasar al siguiente bit
        base *= 2;
    }
    
    return decimal;
}

/*---------------------------------------------------------------------------------------------------------
* Inicializa la lista de nodos libres
* Input:    -
* Output:   OUTPUT
---------------------------------------------------------------------------------------------------------*/

int initAI(){

   struct superbloque SB;
   bread(posSB,&SB);
   
   struct inodo inodos[BLOCKSIZE/INODOSIZE];

   int contInodos = SB.posPrimerInodoLibre + 1;
   for (int i = SB.posPrimerBloqueAI; i <= SB.posUltimoBloqueAI; i++){
   }

}