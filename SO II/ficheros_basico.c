 #include "ficheros_basico.h"

int binaryToDecimal(int byte[]);




/*---------------------------------------------------------------------------------------------------------
* Calcula el número de bloques necesarios para el mapa de bits 
* Input:    nbloques: número de bloques reservados
* Output:   número de bloques necesarios para el mapa de bits 
---------------------------------------------------------------------------------------------------------*/
 
 int tamMB(unsigned int nbloques){

    int res = (nbloques / 8) / BLOCKSIZE;
    if ((nbloques / 8) % BLOCKSIZE != 0){
        res++;
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
    if ((ninodos * INODOSIZE) % BLOCKSIZE != 0){
        res++;
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

      void *buf = &SB;

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

      return bwrite(posSB,buf);
 }

/*---------------------------------------------------------------------------------------------------------
* Inicializa el mapa de bits poniendo a 1 los bits que representan los metadatos.
* Input:    -
* Output:   OUTPUT
---------------------------------------------------------------------------------------------------------*/

int initMB(){

    struct superbloque SB;

    if (bread(posSB,&SB) == FALLO){
        perror("initMB: error bread SB");
        return FALLO;
    }

    //unsigned int nbits = SB.posUltimoBloqueAI;
    unsigned int nbits = tamSB + tamMB(SB.totBloques) + tamAI(SB.totInodos);
    unsigned int nbloques = nbits/8/BLOCKSIZE;
    unsigned char bufferMB[BLOCKSIZE];
    unsigned int nbytes = nbits/8;
    unsigned int nbitsrest = nbits%8;

    for (int i=0; i<nbytes%BLOCKSIZE; i++){
        bufferMB[i] = 255;
    }

    if (nbitsrest != 0){
        unsigned char byteaux = 128; //10000000
        for (int i=0; i<nbitsrest; i++){
            bufferMB[nbytes%BLOCKSIZE] |= byteaux;
            byteaux >>= 1;
        }
    }

    for (int i = nbytes%BLOCKSIZE + 1; i<BLOCKSIZE; i++){
        bufferMB[i] = 0;
    }

    if (nbloques == 0){
        if (bwrite(SB.posPrimerBloqueMB,&bufferMB) == FALLO){
            perror("initMB: error bwrite bufferMB");
            return FALLO;
        }
    }else{
        unsigned char bufferAux[BLOCKSIZE];
        memset(bufferAux,255,BLOCKSIZE);
        for (int i=0; i<nbloques; i++){
            if (bwrite(SB.posPrimerBloqueMB + i,&bufferAux) == FALLO){
                perror("initMB: error bwrite bufferMB (+1 bloques)");
                return FALLO;
            }
        }
        if (bwrite(SB.posPrimerBloqueMB + nbloques,&bufferMB) == FALLO){
            perror("initMB: error bwrite bufferMB");
            return FALLO;
        }
    }

    SB.cantBloquesLibres = SB.cantBloquesLibres -(tamMB(SB.totBloques) + tamSB + tamAI(SB.totInodos));

    if (bwrite(posSB,&SB) == FALLO){
        perror("initMB: error bwrite SB");
        return FALLO;
    }

    return EXITO;

}

int initMB2(){    
    
    struct superbloque SB;
    
    if(bread(posSB,&SB) == FALLO){
        return FALLO;
    } 

    char bufferMB[BLOCKSIZE];
    int nbits = SB.posPrimerBloqueDatos;
    int nbytes = nbits / 8;
    int primerBloq = SB.posPrimerBloqueMB;
    int totalBloq = 1;

   //mirar si necesitamos bloques extra, o solo 1
    if ((nbits / 8) / BLOCKSIZE != 0){
        int bloqExtra = (nbits / 8) / BLOCKSIZE;
        memset(bufferMB,255,BLOCKSIZE);
        for(int i = 0; i < bloqExtra; i++){
            if(bwrite(primerBloq, &bufferMB) == FALLO){
                return FALLO;
            }
            totalBloq++;
            primerBloq++;
        }
        
    }
    
    //rellenamos el ultimo bloque
    for (int i = 0; i < nbytes; i++){
        bufferMB[i] = 255; // ponemos todos los bytes a 1 (elementos de la array)
    }

    // calcular resto de bits que pueden quedar por meter
    int rest = nbits % 8;
    if (rest != 0){
        //byte auxiliar
        int byte[8];
        for (int i = 0; i < rest; i++){
           byte[i] = 1;
        }
        bufferMB[nbytes] = binaryToDecimal(byte);
    }else{
        bufferMB[nbytes] = 0;
    }

   //acabar de iniciar el resto de bits 
    for (int aux = nbytes + 1; aux < BLOCKSIZE; aux++){
        bufferMB[aux] = 0;
    }

    if(bwrite(SB.posPrimerBloqueMB, &bufferMB) == FALLO){
        return FALLO;
    }

    SB.cantBloquesLibres = SB.cantBloquesLibres -(tamMB(SB.totBloques) + tamSB + tamAI(SB.totInodos));
    if(bwrite(posSB,&SB) == FALLO){
        return FALLO;
    }

    return EXITO;
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
* Inicializa la lista de inodos libres
* Input:    -
* Output:   OUTPUT
---------------------------------------------------------------------------------------------------------*/

int initAI(){

   struct superbloque SB;

   if(bread(posSB,&SB) == FALLO){
        return FALLO;
    }
   
   struct inodo inodos[BLOCKSIZE/INODOSIZE];

   int contInodos = SB.posPrimerInodoLibre + 1;
   int i = 0;

   for (i = SB.posPrimerBloqueAI; (i <= SB.posUltimoBloqueAI); i++){
      bread(i, &inodos);
      for(int j = 0; (j < (BLOCKSIZE / INODOSIZE)); j++){
         inodos[j].tipo = 'l';  //libres
         if(contInodos < SB.totInodos){
            inodos[j].punterosDirectos[0] = contInodos;
            contInodos++;
         } else {
            inodos[j].punterosDirectos[0] = UINT_MAX;
            j = BLOCKSIZE/INODOSIZE;
         }
      }
      if(bwrite(i, &inodos) == FALLO){
            return FALLO;
      }
   }

   return EXITO;

}

/*---------------------------------------------------------------------------------------------------------
* Escribe el valor indicado por bit {0,1} en un determinado bit del MB
* Input:    bit: valor a escribir; nbloque: numero del bloque físico
* Output:   OUTPUT
---------------------------------------------------------------------------------------------------------*/
int escribir_bit(unsigned int nbloque, unsigned int bit){
    struct superbloque SB;

    if(bread(posSB,&SB) == FALLO){
        return FALLO;
    }    

    unsigned int posbyte = nbloque/8;
    unsigned int posbit = nbloque%8;
    unsigned nbloqueMB = posbyte/BLOCKSIZE; //numero de bloque de forma relativa al MB
    unsigned int nbloqueabs = SB.posPrimerBloqueMB + nbloqueMB; //posicion absoluta del bloque en el dispositivo
    unsigned char bufferMB[BLOCKSIZE];

    unsigned char mascara = 128; //10000000
    mascara >>= posbit; //desplazamos el bit de la mascara al bit deseado

    //cargamos el bloque que contiene el bit para leer
    if(bread(nbloqueabs,bufferMB) == FALLO){
        return FALLO;
    }

    posbyte = posbyte % BLOCKSIZE; //localizar el byte dentro del bloque leido

    //mirar si se debe escribir un 0 o 1
    if(bit == 1){
        bufferMB[posbyte] |= mascara;
    }else{
        bufferMB[posbyte] &= ~mascara;
    }

    if(bwrite(nbloqueabs,bufferMB) == FALLO){
        return FALLO;
    }

    return EXITO;
}

/*---------------------------------------------------------------------------------------------------------
* Lee un determinado bit del MB y devuelve el valor leido
* Input:    nbloque: numero del bloque físico
* Output:   OUTPUT
---------------------------------------------------------------------------------------------------------*/
char leer_bit(unsigned int nbloque){

    struct superbloque SB;

    if(bread(posSB,&SB) == FALLO){
        return FALLO;
    }   


    unsigned int posbyte = nbloque/8;
    unsigned int posbit = nbloque%8;
    unsigned nbloqueMB = posbyte/BLOCKSIZE; //numero de bloque de forma relativa al MB
    unsigned int nbloqueabs = SB.posPrimerBloqueMB + nbloqueMB; //posicion absoluta del bloque en el dispositivo
    unsigned char bufferMB[BLOCKSIZE];

    printf(GRIS_T"[leer_bit(%d)->posbyte:%d,posbit:%d,nbloqueMB:%d,nbloqueabs:%d]\n"RESET,nbloque,posbyte,posbit,nbloqueMB,nbloqueabs);
    
    //cargamos el bloque que contiene el bit para leer
    if(bread(nbloqueabs,bufferMB) == FALLO){
        return FALLO;
    }

    posbyte = posbyte % BLOCKSIZE; //localizar el byte dentro del bloque leido

    unsigned char mascara = 128; //10000000
    mascara >>= posbit; //desplazamos el bit de la mascara al bit deseado
    mascara &= bufferMB[posbyte];
    mascara >>= (7 - posbit); //dejar a 0 o 1 el extremo derecho y leerlo en decimal

    return mascara;
    
}

/*---------------------------------------------------------------------------------------------------------
* Encuentra el primer bloque libre, consultando el MB, lo ocupa y devuelve su posición.
* Input:    -
* Output:   nbloque: nº de bloque reservado
---------------------------------------------------------------------------------------------------------*/

int reservar_bloque(){

    struct superbloque SB; //leemos el superbloque
    if (bread(posSB,&SB) == FALLO){
        return FALLO;
    }

    if (SB.cantBloquesLibres == 0){ //comprobar si hay bloques disponibles
        return FALLO;
    }
    
    unsigned char bufferMB[BLOCKSIZE]; //buffer para el mapa de bits
    unsigned char bufferAux[BLOCKSIZE]; //buffer auxiliar para encontrar el primer bloque con un 0
    memset(bufferAux,255,BLOCKSIZE); //llenamos el buffer aux con 1s

    int nbloqueabs = SB.posPrimerBloqueMB;
    bool found = false;

    while (found == false && nbloqueabs < SB.posUltimoBloqueMB){ //encontramos el primer bloque con un 0 y guardamos su contenido en bufferMB

        if (bread(nbloqueabs,bufferMB) == FALLO){
            perror("reservar_bloque: Error en bread de bloque con 0");
            return FALLO;
        }

        if (memcmp(bufferMB,bufferAux,BLOCKSIZE) != 0){
            found = true;
            break;
        }

        nbloqueabs++;

    }

    unsigned int posByte = 0;
    found = false;

    while (found == false && posByte < BLOCKSIZE){ //encontramos el primer byte con un 0

        if (bufferMB[posByte] != 255){
            found = true;
            break;
        }else{
            posByte++;
        }

    }

    unsigned int posBit = 0;
    unsigned char mascara = 128; //máscara 10000000

    while (bufferMB[posByte] & mascara){ //encontramos el primer bit a 0

        bufferMB[posByte] <<= 1;
        posBit++;

    }

    //buscamos el bloque fisico que representa el bit
    unsigned int nbloque = ((nbloqueabs - SB.posPrimerBloqueMB) * BLOCKSIZE + posByte) * 8 + posBit;
    if(escribir_bit(nbloque,1) == FALLO){
        perror("reservar_bloque: Error al escribir el bit");
        return FALLO;
    } //ponemos el bit a 1

    SB.cantBloquesLibres--; //decrementamos la cantidad de bloques libres

    if (bwrite(posSB,&SB) == FALLO){ //modificamos y salvamos el superbloque
        perror("reservar_bloque: Error bwrite SB");
        return FALLO;
    }

    memset(bufferAux,0,BLOCKSIZE); //ponemos a 0s el bloque reservado
    if (bwrite(nbloque,bufferAux) == FALLO){
        perror("reservar_bloque: Error bwrite 0s");
        return FALLO;
    }

    return nbloque;

}

/*---------------------------------------------------------------------------------------------------------
* Libera el bloque especificado por parámetro
* Input:    nbloque: bloque que liberaremos
* Output:   nbloque: bloque liberado
---------------------------------------------------------------------------------------------------------*/

int liberar_bloque(unsigned int nbloque){

    escribir_bit(nbloque,0); //actualizamos el mapa de bits

    struct superbloque SB; //actualizamos el superbloque y lo salvamos
    if (bread(posSB,&SB) == FALLO){
        perror("liberar_bloque: Error bread SB");
        return FALLO;
    }
    SB.cantBloquesLibres++; //aumentamos la cantidad de bloques libres
    if (bwrite(posSB,&SB) == FALLO){
        perror("liberar_bloque: Error bwrite SB");
        return FALLO;
    }

    return nbloque;

}

/*---------------------------------------------------------------------------------------------------------
* Escribe el contenido de una variable de tipo struct inodo, pasada por referencia, en un determinado 
* inodo del array de inodos, inodos.
* Input:    ninodo: posición que se encuentra el inodo  
*           *inodo: variable de tipo struct que su contenido debe ser escrito en un array de inodos
* Output:   OUTPUT
---------------------------------------------------------------------------------------------------------*/

int escribir_inodo(unsigned int ninodo, struct inodo *inodo){

    struct superbloque SB;
    struct inodo inodos[BLOCKSIZE/INODOSIZE];
    unsigned int posBloque;

    if(bread(posSB,&SB) == FALLO){
        return FALLO;
    }

    posBloque = ninodo/(BLOCKSIZE/INODOSIZE) + SB.posPrimerBloqueAI;
    
    if(bread(posBloque, inodos) == FALLO){
        return FALLO;
    }

    inodos[ninodo%(BLOCKSIZE/INODOSIZE)] = *inodo;

    if(bwrite(posBloque,inodos) == FALLO){
        return FALLO;
    }

    return EXITO;
}

/*---------------------------------------------------------------------------------------------------------
* Lee un determinado inodo del array de inodos para volcarlo en una variable de tipo 
* struct inodo pasada por referencia.
* Input:    ninodo: posición que se encuentra el inodo  
*           *inodo: variable de tipo struct donde se debe escribir un determinado inodo
* Output:   OUTPUT
---------------------------------------------------------------------------------------------------------*/

int leer_inodo(unsigned int ninodo, struct inodo *inodo){

    struct superbloque SB;
    struct inodo inodos[BLOCKSIZE/INODOSIZE];
    struct inodo leido;
    unsigned int posBloque;

    if(bread(posSB,&SB) == FALLO){
        return FALLO;
    }

    posBloque = ninodo/(BLOCKSIZE/INODOSIZE) + SB.posPrimerBloqueAI;

    if(bread(posBloque, inodos) == FALLO){
        return FALLO;
    }

    leido = inodos[ninodo%(BLOCKSIZE/INODOSIZE)];

    memcpy(inodo, &leido, INODOSIZE);

    return EXITO;
}

/*---------------------------------------------------------------------------------------------------------
* Encuentra el primer inodo libre, lo reserva y actualiza la lista enlazada de inodos libres
* Input:    tipo: ('l':libre, 'd':directorio o 'f':fichero)
*           permisos: (lectura y/o escritura y/o ejecución)
* Output:   posInodoReservado: posición del inodo reservado
---------------------------------------------------------------------------------------------------------*/

int reservar_inodo(unsigned char tipo, unsigned char permisos){

    struct superbloque SB;
    if (bread(posSB,&SB) == FALLO){
        perror("reservar_inodo: Error bread SB");
        return FALLO;
    }
    if (SB.cantInodosLibres == 0){
        perror("reservar_inodo: no hay inodos libres");
        return FALLO;
    }

    unsigned int posInodoReservado = SB.posPrimerInodoLibre;

    SB.posPrimerInodoLibre++;
    SB.cantInodosLibres--;

    if (bwrite(posSB,&SB) == FALLO){
        perror("reservar_inodo: Error bwrite SB");
        return FALLO;
    }

    struct inodo inodo;
    inodo.tipo = tipo;
    inodo.permisos = permisos;
    inodo.nlinks = 1;
    inodo.tamEnBytesLog = inodo.numBloquesOcupados = 0;
    inodo.atime = inodo.ctime = inodo.mtime = time(NULL);
    for (int i=0; i<12; i++){
        for (int j=0; j<3; j++){
            inodo.punterosIndirectos[j] = 0;
        }
        inodo.punterosDirectos[i] = 0;
    }

    escribir_inodo(posInodoReservado,&inodo);
    return posInodoReservado;

}

/*---------------------------------------------------------------------------------------------------------
*
* Input:    
* Output:   
---------------------------------------------------------------------------------------------------------*/

int obtener_nRangoBL(struct inodo *inodo, unsigned int nblogico, unsigned int *ptr){

    int nrangoBL;

    if(nblogico<DIRECTOS){
        *ptr = inodo->punterosDirectos[nblogico];
        nrangoBL = 0;   
    }else if(nblogico<INDIRECTOS0){
        *ptr = inodo->punterosIndirectos[0];
        nrangoBL = 1;
    }else if(nblogico<INDIRECTOS1){
        *ptr = inodo->punterosIndirectos[1];
        nrangoBL = 2;
    }else if(nblogico<INDIRECTOS2){
        *ptr = inodo->punterosIndirectos[2];
        nrangoBL = 3;
    }else{
        *ptr = 0;
        perror("obtener_nRangoBL: Bloque logico fuera de rango/n");
        nrangoBL = FALLO;
    }
    return nrangoBL;
}

/*---------------------------------------------------------------------------------------------------------
*
* Input:    
* Output:   
---------------------------------------------------------------------------------------------------------*/

int obtener_indice(unsigned int nblogico, int nivel_punteros){

    int ind;

    if(nblogico < DIRECTOS){
        ind = nblogico;
    }else if(nblogico < INDIRECTOS0){
        ind = nblogico-DIRECTOS;
    }else if(nblogico < INDIRECTOS1){
        if(nivel_punteros == 2){
            ind = (nblogico - INDIRECTOS0) / NPUNTEROS;
        }else if(nivel_punteros == 1){
            ind = (nblogico - INDIRECTOS0) % NPUNTEROS;
        }
    }else if(nblogico < INDIRECTOS2){
        if(nivel_punteros == 3){
            ind = (nblogico - INDIRECTOS1) / (NPUNTEROS * NPUNTEROS);
        }else if(nivel_punteros == 2){
            ind = ((nblogico - INDIRECTOS1) % (NPUNTEROS * NPUNTEROS)) / NPUNTEROS;
        }else if(nivel_punteros == 1){
            ind = ((nblogico - INDIRECTOS1) % (NPUNTEROS * NPUNTEROS)) % NPUNTEROS;
        }
    }
    return ind;
}

/*---------------------------------------------------------------------------------------------------------
*
* Input:    
* Output:   
---------------------------------------------------------------------------------------------------------*/
int traducir_bloque_inodo(struct inodo *inodo, unsigned int nblogico, unsigned char reservar) {
    unsigned int ptr, ptr_ant;
    int nRangoBL, nivel_punteros, indice;
    unsigned int buffer[NPUNTEROS];

    ptr = 0, ptr_ant = 0;
    nRangoBL = obtener_nRangoBL(inodo, nblogico, &ptr);
    nivel_punteros = nRangoBL;
    
    while (nivel_punteros > 0) {
        if (ptr == 0) {
            if (reservar == 0){
                return FALLO;
            }else{
                ptr = reservar_bloque();
                inodo->numBloquesOcupados++;
                inodo->ctime = time(NULL);
            
                if(nivel_punteros == nRangoBL) {
                    inodo->punterosIndirectos[nRangoBL - 1] = ptr;
                    fprintf(stderr,GRIS_T "[traducir_bloque_inodo() → inodo.punterosIndirectos[%i] = %i (reservado BF %i para punteros_nivel%i\n",
                    nRangoBL-1, ptr, ptr, nivel_punteros);
                } else {
                    buffer[indice] = ptr;
                    fprintf(stderr,GRIS_T "[traducir_bloque_inodo() → punteros_nivel%i[%i] = %i (reservado BF %i para punteros_nivel%i)\n",
                    nivel_punteros+1, indice, ptr, ptr, nivel_punteros);
                    bwrite(ptr_ant, buffer);
                }
                memset(buffer, 0, BLOCKSIZE);
            }
        }else{
             bread(ptr, buffer);
        }
       
        indice = obtener_indice(nblogico, nivel_punteros);
        ptr_ant = ptr;
        ptr = buffer[indice];
        nivel_punteros--;
    }
    
    if (ptr == 0) {
        if (reservar == 0){
            return FALLO;
        }else{
            ptr = reservar_bloque();
            inodo->numBloquesOcupados++;
            inodo->ctime = time(NULL);
            if(nRangoBL == 0) {
                inodo->punterosDirectos[nblogico] = ptr;
                fprintf(stderr,GRIS_T "[traducir_bloque_inodo() → inodo.punterosDirectos[%i] = %i (reservado BF %i para BL %i)\n",
                nblogico, ptr, ptr, nblogico);
            } else {
                buffer[indice] = ptr;
                fprintf(stderr,GRIS_T "[traducir_bloque_inodo() → inodo.punteros_nivel%d[%i] = %i (reservado BF %i para BL %i)\n"RESET,nivel_punteros +1,
                indice, ptr, ptr, nblogico);
                bwrite(ptr_ant, buffer);
            }
        }    
    }

    //printf("%d",inodo->numBloquesOcupados);
    
    return ptr;
}