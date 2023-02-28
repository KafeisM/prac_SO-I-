 #include "ficheros_basico.h"

#define posSB 0 // el superbloque se escribe en el primer bloque de nuestro FS
#define tamSB 1
#define INODOSIZE 128 // tamaño en bytes de un inodo

struct superbloque {
   unsigned int posPrimerBloqueMB;          // Posición absoluta del primer bloque del mapa de bits
   unsigned int posUltimoBloqueMB;          // Posición absoluta del último bloque del mapa de bits
   unsigned int posPrimerBloqueAI;            // Posición absoluta del primer bloque del array de inodos
   unsigned int posUltimoBloqueAI;            // Posición absoluta del último bloque del array de inodos
   unsigned int posPrimerBloqueDatos;     // Posición absoluta del primer bloque de datos
   unsigned int posUltimoBloqueDatos;     // Posición absoluta del último bloque de datos
   unsigned int posInodoRaiz;                     // Posición del inodo del directorio raíz (relativa al AI)
   unsigned int posPrimerInodoLibre;        // Posición del primer inodo libre (relativa al AI)
   unsigned int cantBloquesLibres;            // Cantidad de bloques libres (en todo el disco)
   unsigned int cantInodosLibres;              // Cantidad de inodos libres (en el AI)
   unsigned int totBloques;                          // Cantidad total de bloques del disco
   unsigned int totInodos;                            // Cantidad total de inodos (heurística)
   char padding[BLOCKSIZE - 12 * sizeof(unsigned int)]; // Relleno para ocupar el bloque completo
};

struct inodo {     // comprobar que ocupa 128 bytes haciendo un sizeof(inodo)!!!
   unsigned char tipo;     // Tipo ('l':libre, 'd':directorio o 'f':fichero)
   unsigned char permisos; // Permisos (lectura y/o escritura y/o ejecución)


   /* Por cuestiones internas de alineación de estructuras, si se está utilizando
    un tamaño de palabra de 4 bytes (microprocesadores de 32 bits):
   unsigned char reservado_alineacion1 [2];
   en caso de que la palabra utilizada sea del tamaño de 8 bytes
   (microprocesadores de 64 bits): unsigned char reservado_alineacion1 [6]; */
   unsigned char reservado_alineacion1[6];


   time_t atime; // Fecha y hora del último acceso a datos
   time_t mtime; // Fecha y hora de la última modificación de datos
   time_t ctime; // Fecha y hora de la última modificación del inodo


   /* comprobar que el tamaño del tipo time_t para vuestra plataforma/compilador es 8:
   printf ("sizeof time_t is: %ld\n", sizeof(time_t)); */


   unsigned int nlinks;             // Cantidad de enlaces de entradas en directorio
   unsigned int tamEnBytesLog;      // Tamaño en bytes lógicos (EOF)
   unsigned int numBloquesOcupados; // Cantidad de bloques ocupados zona de datos


   unsigned int punterosDirectos[12];  // 12 punteros a bloques directos
   unsigned int punterosIndirectos[3]; /* 3 punteros a bloques indirectos:
   1 indirecto simple, 1 indirecto doble, 1 indirecto triple */


   /* Utilizar una variable de alineación si es necesario  para vuestra plataforma/compilador   */
   char padding[INODOSIZE - 2 * sizeof(unsigned char) - 3 * sizeof(time_t) - 18 * sizeof(unsigned int) - 6 * sizeof(unsigned char)];
   // Fijarse que también se resta lo que ocupen las variables de alineación utilizadas!!!
};




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
    bread(0,&SB);

    
    char bufferMB[BLOCKSIZE];
    //numero de BITS del mapa (es el tamaño de SB + MB + AI), cada bit representa un bloque fisico
    int nbits = SB.posPrimerBloqueDatos;

   //miramos si todos estos BITS caben en 1 bloque
   if((nbits/8)/BLOCKSIZE == 0){
      int nbytes = nbits/8;
      

      for (int i = 0; i < nbytes; i++){
         bufferMB[i] = 255; //ponemos todos los bytes a 1 (elementos de la array)
      }

      //calcular resto de bits que quedan
      int rest = nbits % 8;
      if(rest != 0){
         bufferMB[nbytes] = 224; //QUEDA IMPLEMENTAR-LO
      }
      

   }else{

   }

    

 }

/*---------------------------------------------------------------------------------------------------------
* Inicializa la lista de nodos libres
* Input:    -
* Output:   OUTPUT
---------------------------------------------------------------------------------------------------------*/

int initAI(){

}