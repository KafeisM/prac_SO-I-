 /*JOSEP GABRIEL FORNÉS REYNÉS, JORDI FLORIT ENSENYAT, PAU GIRÓN RODRÍGUEZ*/
 
 #include "ficheros_basico.h"
 #include <math.h>

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
    
    //leemos el superbloque
    struct superbloque SB;
    if (bread(posSB, &SB) == FALLO){
        fprintf(stderr, ROJO_T"initMB: Error en bread de SB\n"RESET);
        return FALLO;
    }

    //buffer para ir a memoria
    unsigned char bufferMB[BLOCKSIZE];
    if (!memset(bufferMB, 0, BLOCKSIZE)){
        fprintf(stderr, ROJO_T"initMB: Error en memset\n"RESET);
        return FALLO;
    }

    int nBloquesMetadatos = tamSB + tamMB(SB.totBloques) + tamAI(SB.totInodos); //bits que ocupan los metadatos
    int nbytes = nBloquesMetadatos / 8;  //bytes ocupados
    int bitsres = nBloquesMetadatos % 8; //bits restantes

    //bloques que ocupan los metadatos
    int bloquesmetadatos = nbytes / BLOCKSIZE;

    //ei hay más de 0 bloques ocupados, ponemos el buffer a 1's
    if (bloquesmetadatos >= 1){

        if (!memset(bufferMB, 255, BLOCKSIZE)){
            fprintf(stderr, ROJO_T"initMB: Error en memset\n"RESET);
            return FALLO;
        }

        //escribir todos los bloques
        for (int i = 0; i < bloquesmetadatos; i++){
            if (bwrite(SB.posPrimerBloqueMB+i, bufferMB) == FALLO){
                fprintf(stderr, ROJO_T"initMB: Error en bwrite de los bloques\n"RESET);
                return FALLO;
            }
        }
        //reseteamos el buffer
        if (!memset(bufferMB, 0, BLOCKSIZE)){
            fprintf(stderr, ROJO_T"initMB: Error en memset\n"RESET);
            return FALLO;
        }
    }

    //bytes que ocupan los bloques enteros
    int nbytesent = nbytes - BLOCKSIZE * bloquesmetadatos;

    //poner a 1's todos los bytes completos
    for (int i = 0; i < nbytesent; i++){
        if (!memset(bufferMB+i, 255, 1)){
            fprintf(stderr, ROJO_T"initMB: Error en memset\n"RESET);
            return FALLO;
        }
    }

    //poner a 1's todos los bits restantes
    if (bitsres != 0){
        unsigned int aux = 0;
        int e = 7; //variable para el exponente (es el maximo)
        for (int i = 0; i < bitsres; i++){
            aux = aux + pow(2, e);
            e--;
        }
        if (!memset(bufferMB + nbytesent, aux, 1)){
            fprintf(stderr, ROJO_T"initMB: Error en memset\n"RESET);
            return FALLO;
        }
    }

    //escribimos los bits restantes
    if (bwrite(SB.posPrimerBloqueMB + bloquesmetadatos, bufferMB) == FALLO){
        fprintf(stderr, ROJO_T"initMB: Error en bwrite de bits restantes\n"RESET);
        return FALLO;
    }

    //escribimos SB actualizado
    SB.cantBloquesLibres = SB.cantBloquesLibres - nBloquesMetadatos;

    if (bwrite(posSB, &SB) == FALLO){
        fprintf(stderr, ROJO_T"initMB: Error en bwrite del SB\n"RESET);
        return FALLO;
    }

    return EXITO;
}

/*---------------------------------------------------------------------------------------------------------
* Inicializa la lista de inodos libres
* Input:    -
* Output:   OUTPUT
---------------------------------------------------------------------------------------------------------*/

int initAI(){


    //leemos el SB
    struct superbloque SB;
    if(bread(posSB,&SB) == FALLO){
        fprintf(stderr, ROJO_T"initAI: Error en bread\n"RESET);
        return FALLO;
    }
   
   //array de inodos con tamaño = cantidad de inodos por bloque, y variables
   struct inodo inodos[BLOCKSIZE/INODOSIZE];
   int contInodos = SB.posPrimerInodoLibre + 1;
   int i = 0;

    //recorremos todos los bloques donde está el AI
    for (i = SB.posPrimerBloqueAI; (i <= SB.posUltimoBloqueAI); i++){
        //leemos un bloque
        bread(i, &inodos);
        //inicializamos cada inodo
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
        //escribimos en memoria
        if(bwrite(i, &inodos) == FALLO){
            fprintf(stderr, ROJO_T"initAI: Error en bwrite\n"RESET);
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
        fprintf(stderr, ROJO_T"escribir_bit: Error en el bread de SB\n"RESET);
        return FALLO;
    }    

    unsigned int posbyte = nbloque/8;
    unsigned int posbit = nbloque%8;
    unsigned int nbloqueMB = posbyte/BLOCKSIZE; //numero de bloque de forma relativa al MB
    unsigned int nbloqueabs = SB.posPrimerBloqueMB + nbloqueMB; //posicion absoluta del bloque en el dispositivo
    unsigned char bufferMB[BLOCKSIZE];

    //cargamos el bloque que contiene el bit para leer
    if(bread(nbloqueabs,bufferMB) == FALLO){
        fprintf(stderr, ROJO_T"escribir_bit: Error en el bread del bloque\n"RESET);
        return FALLO;
    }

    posbyte = posbyte % BLOCKSIZE; //localizar el byte dentro del bloque leido

    unsigned char mascara = 128; //10000000
    mascara = mascara >> posbit; //desplazamos el bit de la mascara al bit deseado

    //mirar si se debe escribir un 0 o 1
    if(bit == 1){
        bufferMB[posbyte] |= mascara;
    }else if (bit==0){
        bufferMB[posbyte] &= ~mascara;
    }else{
        return FALLO;
    }

    if(bwrite(nbloqueabs,bufferMB) == FALLO){
        fprintf(stderr, ROJO_T"escribir_bit: Error en el bwrite de SB\n"RESET);
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
        fprintf(stderr, ROJO_T"leer_bit: Error en el bread de SB\n"RESET);
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
        fprintf(stderr, ROJO_T"leer_bit: Error en el bread del bloque\n"RESET);
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
        fprintf(stderr, ROJO_T"reservar_bloque: Error en el bread de SB\n"RESET);
        return FALLO;
    }

    if (SB.cantBloquesLibres == 0){ //comprobar si hay bloques disponibles
        fprintf(stderr, ROJO_T"reservar_bloque: Error, no hay bloques disponibles\n"RESET);
        return FALLO;
    }
    
    unsigned char bufferMB[BLOCKSIZE]; //buffer para el mapa de bits
    unsigned char bufferAux[BLOCKSIZE]; //buffer auxiliar para encontrar el primer bloque con un 0
    memset(bufferAux,255,BLOCKSIZE); //llenamos el buffer aux con 1s

    unsigned int nbloqueabs = SB.posPrimerBloqueMB;
    bool found = false;

    while (found == false && nbloqueabs < SB.posUltimoBloqueMB){ //encontramos el primer bloque con un 0 y guardamos su contenido en bufferMB

        if (bread(nbloqueabs,bufferMB) == FALLO){
            fprintf(stderr, ROJO_T"reservar_bloque: Error en bread de bloque con 0\n"RESET);
            return FALLO;
        }

        if (memcmp(bufferMB,bufferAux,BLOCKSIZE) != 0){
            found = true;
            break;
        }

        nbloqueabs++;

    }

    unsigned int posByte = 0;
    while (bufferMB[posByte] == 255){
        posByte++;
    }

    unsigned int posBit = 0;
    unsigned char mascara = 128; //máscara 10000000

    while (bufferMB[posByte] & mascara){ //encontramos el primer bit a 0

        bufferMB[posByte] <<= 1;
        posBit++;

    }

    //buscamos el bloque fisico que representa el bit
    int nbloque = ((nbloqueabs - SB.posPrimerBloqueMB) * BLOCKSIZE + posByte) * 8 + posBit;
    if(escribir_bit(nbloque,1) == FALLO){
        fprintf(stderr, ROJO_T"reservar_bloque: Error al escribir el bit\n"RESET);
        return FALLO;
    } //ponemos el bit a 1

    SB.cantBloquesLibres--; //decrementamos la cantidad de bloques libres

    if (bwrite(posSB,&SB) == FALLO){ //modificamos y salvamos el superbloque
        fprintf(stderr, ROJO_T"reservar_bloque: Error bwrite SB\n"RESET);
        return FALLO;
    }
    unsigned char buffer[BLOCKSIZE];
    memset(buffer,0,BLOCKSIZE); //ponemos a 0s el bloque reservado
    if (bwrite(nbloque,buffer) == FALLO){
        fprintf(stderr, ROJO_T"reservar_bloque: Error bwrite 0s\n"RESET);
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
        fprintf(stderr, ROJO_T"liberar_bloque: Error bread SB\n"RESET);
        return FALLO;
    }
    SB.cantBloquesLibres++; //aumentamos la cantidad de bloques libres
    if (bwrite(posSB,&SB) == FALLO){
        fprintf(stderr, ROJO_T"liberar_bloque: Error bwrite SB\n"RESET);
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

    //leemos el SB
    if(bread(posSB,&SB) == FALLO){
        fprintf(stderr, ROJO_T"escribir_inodo: Error bread SB\n"RESET);
        return FALLO;
    }

    //bloque donde está el inodo solicitado
    posBloque = ninodo/(BLOCKSIZE/INODOSIZE) + SB.posPrimerBloqueAI;
    
    //leemos bloque
    if(bread(posBloque, inodos) == FALLO){
        fprintf(stderr, ROJO_T"escribir_inodo: Error bread del bloque\n"RESET);
        return FALLO;
    }

    //añadimos el inodo pasado por parámetro al buffer
    inodos[ninodo%(BLOCKSIZE/INODOSIZE)] = *inodo;

    //escribimos el buffer en memoria
    if(bwrite(posBloque,inodos) == FALLO){
        fprintf(stderr, ROJO_T"escribir_inodo: Error bwrite del bloque\n"RESET);
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

    //leemos el SB
    if(bread(posSB,&SB) == FALLO){
        fprintf(stderr, ROJO_T"leer_inodo: Error bread SBºn"RESET);
        return FALLO;
    }

    //posicion del bloque donde está el inodo solicitado
    posBloque = ninodo/(BLOCKSIZE/INODOSIZE) + SB.posPrimerBloqueAI;

    //leemos todo el bloque en un array de inodos
    if(bread(posBloque, inodos) == FALLO){
        fprintf(stderr, ROJO_T"leer_inodo: Error bread del bloque\n"RESET);
        return FALLO;
    }

    //guardamos el que queremos
    leido = inodos[ninodo%(BLOCKSIZE/INODOSIZE)];
    //y lo cargamos en la variable solicitada del resultado
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

    //leemos SB
    struct superbloque SB;
    if (bread(posSB,&SB) == FALLO){
        fprintf(stderr, ROJO_T"reservar_inodo: Error bread SB\n"RESET);
        return FALLO;
    }
    //si no hay inodos libres error
    if (SB.cantInodosLibres == 0){
        fprintf(stderr, ROJO_T"reservar_inodo: Error, no hay inodos libres\n"RESET);
        return FALLO;
    }

    //posicion del inodo a reservar
    unsigned int posInodoReservado = SB.posPrimerInodoLibre;

    //actualizamos SB
    SB.posPrimerInodoLibre++;
    SB.cantInodosLibres--;
    if (bwrite(posSB,&SB) == FALLO){
        fprintf(stderr, ROJO_T"reservar_inodo: Error bwrite SB\n"RESET);
        return FALLO;
    }

    //inicializamos nuevo inodo
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

    //escribimos el nuevo inodo
    escribir_inodo(posInodoReservado,&inodo);
    return posInodoReservado;

}

/*---------------------------------------------------------------------------------------------------------
* Liberamos el inodo especificado, lo pasamos al frente de la lista de inodos libres y aumentan los inodos
* libres
* Input:  ninodo: número de inodo a liberar   
* Output: ninodo: número de inodo liberado
---------------------------------------------------------------------------------------------------------*/
int liberar_inodo(unsigned int ninodo){
    struct inodo inodo;
    int bloqliberados;

    //leer el inodo
    if(leer_inodo(ninodo,&inodo) == FALLO){
        return FALLO;
    }

    bloqliberados = liberar_bloques_inodo(0,&inodo);
    inodo.numBloquesOcupados = inodo.numBloquesOcupados - bloqliberados;

    //comprobar que se ha liberado correctamente
    if(inodo.numBloquesOcupados != 0){
        return FALLO;
    }

    //actualizar datos del inodo liberado
    inodo.tamEnBytesLog = 0;
    inodo.tipo = 'l';

    //ACTUALIZAR LISTA DE INODOS LIBRES
    struct superbloque SB;
    if(bread(posSB,&SB) == FALLO){
        return FALLO;
    }

    //Introducir el inodo liberado en la lista como el primer inodo libre, y enlazamos este con el anterior
    int auxposInodo = SB.posPrimerInodoLibre;
    SB.posPrimerInodoLibre = ninodo;
    inodo.punterosDirectos[0] = auxposInodo;
    SB.cantInodosLibres++;

    //guardar el superbloque
    if(bwrite(posSB,&SB) == FALLO){
        return FALLO;
    }

    //actualizar el ctime
    inodo.ctime = time(NULL);

    //guardar el inodo
    if(escribir_inodo(ninodo,&inodo) == FALLO){
        return FALLO;
    }

    return ninodo;

}

/*---------------------------------------------------------------------------------------------------------
* (optimizar)
* Input:    
* Output:   
---------------------------------------------------------------------------------------------------------*/
int liberar_bloques_inodo(unsigned int primerBL, struct inodo *inodo){
    
    unsigned int nivel_punteros,indice,ptr,nBL,ultimoBL;
    int nRangoBL;
    unsigned int bloques_punteros[3][NPUNTEROS]; //array bloques de punteros
    unsigned char bufAux_punteros[BLOCKSIZE]; //para llenar de 0s i comparar
    int ptr_nivel[3]; //punteros a bloques de punteros de cada nivel
    int indices[3]; //indices de cada nivel
    int liberado = 0; //nº de bloques liberados

    int tbread = 0;
    int tbwrite = 0;

    if(inodo->tamEnBytesLog == 0){
        return 0; // el fichero esta vacio
    }

    //obtenemos el ultimo bloque logico del inodo (fichero)
    if(inodo->tamEnBytesLog % BLOCKSIZE == 0){
        ultimoBL = ((inodo->tamEnBytesLog)/BLOCKSIZE)-1;
    }else{
        ultimoBL = (inodo->tamEnBytesLog)/BLOCKSIZE;
    }

    memset(bufAux_punteros,0,BLOCKSIZE);
    ptr = 0;

    fprintf(stderr, GRIS_T"[liberar_bloques_inodo() -> primer BL: %i, último BL: %i]\n",primerBL, ultimoBL);

    for(nBL = primerBL; nBL <= ultimoBL; nBL++){ //recorrido de los bloques logicos
        nRangoBL = obtener_nRangoBL(inodo,nBL,&ptr); //0:D, 1:I0, 2:I1; 3:I2

        if(nRangoBL < 0){
            return FALLO;
        }

        nivel_punteros = nRangoBL; //el nivel_punteros +alto cuelga del inodo

        while(ptr > 0 && nivel_punteros > 0){ //cuelgan bloques de punteros
            indice = obtener_indice(nBL,nivel_punteros);
            if(indice == 0 || nBL == primerBL){
                
                if(bread(ptr,bloques_punteros[nivel_punteros-1])==FALLO){
                    return FALLO;
                }else{
                    tbread++;
                }

            }
            ptr_nivel[nivel_punteros-1] = ptr;
            indices[nivel_punteros-1] = indice;
            ptr = bloques_punteros[nivel_punteros-1][indice];
            nivel_punteros--;
        }

        if(ptr > 0){ //si existe bloque de datos
            liberar_bloque(ptr);
            liberado++;
            fprintf(stderr,"[liberar_bloques_inodo()→ liberado BF %i de datos para BL %i]\n", ptr, nBL);

            if(nRangoBL == 0){ //es un puntero directo
                inodo->punterosDirectos[nBL] = 0;
            }else{
                nivel_punteros = 1;
                while(nivel_punteros <= nRangoBL){
                    indice = indices[nivel_punteros-1];
                    bloques_punteros[nivel_punteros-1][indice] = 0;
                    ptr = ptr_nivel[nivel_punteros-1];
                    if(memcmp(bloques_punteros[nivel_punteros-1],bufAux_punteros,BLOCKSIZE) == 0){
                        // No cuelgan mas bloques ocupados, hay que liberar el bloque de punteros
                        liberar_bloque(ptr);
                        liberado++;

                        fprintf(stderr,"[liberar_bloques_inodo()→ liberado BF %i de punteros_nivel%i correspondiente al BL %i]\n", 
                        ptr, nivel_punteros, nBL);

                        //INTRODUCIR AQUI LA MEJORA SALTANDO LOS BLOQUES QUE NO SEA NECESARIO EXPLORAR

                        if(nivel_punteros == nRangoBL){
                            inodo->punterosDirectos[nRangoBL-1] = 0;
                        }
                        nivel_punteros++;

                    }else{ // escribimos en el dispositovo el bloque de punteros modificado
                        if(bwrite(ptr,bloques_punteros[nivel_punteros-1]) == FALLO){
                            return FALLO;
                        }else{
                            tbwrite++;
                        }
                        //hemos de salir del bucle ya que no será necesario liberar los bloques de niveles superiores al que cuelga
                        nivel_punteros = nRangoBL+1;
                    }
                }
            }
                //INCULIR MEJORA SALTANDO LOS BLOQUES QUE NO SEA NECESARIO EXPLORAR AL VALER 0 UN PUNTERO
        }

    }
    
    fprintf(stderr, "[liberar_bloques_inodo() -> total bloques liberados: %i, total_breads: %i, total_bwrites: %i]\n"RESET, liberado,tbread,tbwrite);
    return liberado;
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
    unsigned int nRangoBL, nivel_punteros, indice;
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
                    fprintf(stderr,GRIS_T "[traducir_bloque_inodo() → inodo.punterosIndirectos[%u] = %u (reservado BF %u para punteros_nivel%u)\n",
                    nRangoBL-1, ptr, ptr, nivel_punteros);
                } else {
                    buffer[indice] = ptr;
                    bwrite(ptr_ant, buffer);
                    fprintf(stderr,GRIS_T "[traducir_bloque_inodo() → inodo.punteros_nivel%u[%u] = %u (reservado BF %u para punteros_nivel%u)\n",
                    nivel_punteros+1, indice, ptr, ptr, nivel_punteros);                   
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
                fprintf(stderr,GRIS_T "[traducir_bloque_inodo() → inodo.punterosDirectos[%u] = %u (reservado BF %u para BL %u)\n",
                nblogico, ptr, ptr, nblogico);
            } else {
                buffer[indice] = ptr;
                fprintf(stderr,GRIS_T "[traducir_bloque_inodo() → inodo.punteros_nivel1[%u] = %u (reservado BF %u para BL %u)\n"RESET,
                indice, ptr, ptr, nblogico);
                bwrite(ptr_ant, buffer);
            }
        }    
    }

    
    //printf("%d",inodo->numBloquesOcupados);
    
    return ptr;
}