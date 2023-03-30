#include "ficheros.h"

/*---------------------------------------------------------------------------------------------------------
* Escribe el contenido procedente de un buffer de memoria de tamaño nbytes, en un fichero/directorio
* Input:    ninodo: numero de inodo que queremos escribir
*           *buf_original: buffer en el que se almacena la información que tenemos que escribir
*           offset: posición de lectura inicial con respecto al inodo
*           nbytes: numero de bytes que hay que escribir
* Output:   cantidad de bytes leídos realmente.
---------------------------------------------------------------------------------------------------------*/

int mi_write_f(unsigned int ninodo, const void *buf_original, unsigned int offset, unsigned int nbytes){

    struct inodo inodo;
    int escritos = 0;
    unsigned int primerBL;
    unsigned int ultimoBL;
    int desp1;
    int desp2;
    int nbfisico;
    unsigned char buf_bloque[BLOCKSIZE];

    // Leemos inodo
    if(leer_inodo(ninodo, &inodo) == FALLO){
        return escritos;
    }

    //Miramos si los permisos corresponden a los de escribir
    if ((inodo.permisos & 2) != 2){
        return escritos;
    }

    // Obtenemos el primer y ultimo bloque que queremos escribir
    primerBL = offset / BLOCKSIZE;
    ultimoBL = (offset + nbytes-1) / BLOCKSIZE;

    // Obtenemos el desplazamiento desde el inicio del bloque
    desp1 = offset % BLOCKSIZE;
    desp2 = (offset + nbytes-1) % BLOCKSIZE;

    // Miramos si lo que queremos escribir cabe todo en un mismo bloque
    if(primerBL == ultimoBL){

        // Obtenemos el numero del bloque fisico
        nbfisico = traducir_bloque_inodo(&inodo, primerBL, 1);
        // Miramos si hay bloque fisico, y si hay, entramos dentro del if
        if(nbfisico != FALLO){

            // Leemos el bloque fisico
            if(bread(nbfisico, buf_bloque) == FALLO){
                return FALLO;
            }

            // Escribimos nbytes, del buf_original en la posición (buf_bloque + desp1)
            memcpy(buf_bloque + desp1, buf_original, nbytes);

            // Escribimos el bloque fisico
            if(bwrite(nbfisico, buf_bloque) == FALLO){
                return FALLO;
            }

        }else{
            return FALLO;
        }
        // Los bytes leidos seran los mismos que los que nos pasan por parametro
        escritos = nbytes;

    }else if(primerBL < ultimoBL){  // Si no cabe todo en un mismo bloque, tenemos que escribir el primer 
                                    // bloque parcial, luego escribir los bloques intermeios y por último 
                                    // escribir el último bloque parcial

        //1. Primer bloque parcial escrito
        // Obtenemos el numero del bloque fisico
        nbfisico = traducir_bloque_inodo(&inodo, primerBL, 1);
        // Miramos si hay bloque fisico, y si hay, entramos dentro del if
        if(nbfisico != FALLO){

            // Leemos el bloque fisico
            if(bread(nbfisico, buf_bloque) == FALLO){
                return FALLO;
            }

            // Escribimos nbytes, del buf_original en la posición (buf_bloque + desp1)
            memcpy(buf_bloque + desp1, buf_original, BLOCKSIZE - 1);

            // Escribimos el bloque fisico
            if(bwrite(nbfisico, buf_bloque) == FALLO){
                return FALLO;
            }

        }else{
            return FALLO;
        }
        // Los bytes escritos en esta parte van a ser la longitud del bloque menos el desplazamiento
        escritos += BLOCKSIZE - desp1;

        //2. Bloques intermedios enteros
        for(int i = primerBL + 1; i < ultimoBL; i++){

            // Obtenemos el numero del bloque fisico sobre cada iteracion
            nbfisico = traducir_bloque_inodo(&inodo, i, 0);
            // Miramos si hay bloque fisico, y si hay, entramos dentro del if
            if(nbfisico != FALLO){
                
                // Escribimos el bloque correspondiente
                if(bwrite(nbfisico, buf_original + (BLOCKSIZE - desp1) + (i - primerBL - 1) * BLOCKSIZE) == FALLO){
                    return FALLO;
                }

            }else{
                return FALLO;
            }
            // Los bytes leidos en esta parte van a ser la longitud del bloque entero
            escritos += BLOCKSIZE;

        }

        //3. Ultimo bloque parcial
        // Obtenemos el numero del bloque fisico
        nbfisico = traducir_bloque_inodo(&inodo, ultimoBL, 1);
        // Miramos si hay bloque fisico, y si hay, entramos dentro del if
        if(nbfisico != FALLO){

            // Leemos el bloque fisico
            if(bread(nbfisico, buf_bloque) == FALLO){
                return FALLO;
            }

            // Escribimos nbytes, del buf_original + (nbytes - (desp2 + 1)) en la posición buf_bloque
            memcpy(buf_bloque, buf_original + (nbytes - (desp2 + 1)), desp2 + 1);

            // Escribimos el bloque fisico
            if(bwrite(nbfisico, buf_bloque) == FALLO){
                return FALLO;
            }

        }else{
            return FALLO;
        }
        // Los bytes escritos seran el desplazamiento final mas 1
        escritos += desp2 + 1;
    }

    //Vamos a actualizar mtime y ctime
    //Primero leemos el inodo
    if(leer_inodo(ninodo, &inodo) == FALLO){
        return escritos;
    }
    // Miramos si vamos hemos escrito más alla del EOF
    if((offset + nbytes) > inodo.tamEnBytesLog){
        inodo.tamEnBytesLog = nbytes + offset;
        inodo.ctime = time(NULL);
    }
    //Modificamos mtime
    inodo.mtime = time(NULL);
    //Lo volvemos a escribir
    if(escribir_inodo(ninodo, &inodo) == FALLO){
        return escritos;
    }

    // Si los bytes escritos no coinciden con los bytes que nos han pasado por parametro devuelve FALLO, 
    // sino devuelve el numero de bytes leidos
    if(escritos != nbytes){
        printf("mi_write_f: MAL\n\n");
        return FALLO;
    }else {
        printf("mi_write_f: BIEN\n");
        return nbytes;
    }

    return EXITO;

}

/*---------------------------------------------------------------------------------------------------------
* Lee información de un fichero/directorio y la almacena en un buffer de memoria
* Input:    ninodo: numero de inodo que queremos leer
*           *buf_original: buffer en el que se almacena la información leida
*           offset: posición de lectura inicial con respecto al inodo
*           nbytes: numero de bytes que hay que leer
* Output:   cantidad de bytes leídos realmente.
---------------------------------------------------------------------------------------------------------*/

int mi_read_f(unsigned int ninodo, void *buf_original, unsigned int offset, unsigned int nbytes){

    struct inodo inodo;
    int leidos = 0;
    unsigned int primerBL;
    unsigned int ultimoBL;
    int desp1;
    int desp2;
    int nbfisico;
    unsigned char buf_bloque[BLOCKSIZE];

    // Leemos inodo
    if(leer_inodo(ninodo, &inodo) == FALLO){
        return leidos;
    }

    //Miramos si los permisos corresponden a los de leer
    if ((inodo.permisos & 4) != 4){
        return leidos;
    }

    //Miramos si el offset supera el EOF
    if(offset >= inodo.tamEnBytesLog){
        leidos = 0; //No podemos leer nada
        return leidos;
    }

    // Miramos si vamos a leer más alla del EOF
    if((offset + nbytes) >= inodo.tamEnBytesLog){
        nbytes = inodo.tamEnBytesLog - offset;
        //leemos solo los bytes que podemos desde el offset hasta EOF
    }

    // Obtenemos el primer y ultimo bloque que queremos leer
    primerBL = offset / BLOCKSIZE;
    ultimoBL = (offset + nbytes-1) / BLOCKSIZE;

    // Obtenemos el desplazamiento desde el inicio del bloque
    desp1 = offset % BLOCKSIZE;
    desp2 = (offset + nbytes-1) % BLOCKSIZE;

    // Miramos si lo que queremos escribir cabe todo en un mismo bloque
    if(primerBL == ultimoBL){

        // Obtenemos el numero del bloque fisico
        nbfisico = traducir_bloque_inodo(&inodo, primerBL, 0);
        // Miramos si hay bloque fisico, y si hay, entramos dentro del if
        if(nbfisico != FALLO){
            
            // Leemos el bloque fisico
            if(bread(nbfisico, buf_bloque) == FALLO){
                return FALLO;
            }

            // Escribimos nbytes, del (buf_bloque + desp1) en la posición buf_original
            memcpy(buf_original, buf_bloque + desp1, nbytes);
        }else{
            return FALLO;
        }
        // Los bytes leidos seran los mismos que los que nos pasan por parametro
        leidos = nbytes;

    }else if(primerBL < ultimoBL){  // Si no cabe todo en un mismo bloque, tenemos que leer el primer 
                                    // bloque parcial, luego leer los bloques intermeios y por último 
                                    // leer el último bloque parcial

        //1. Primer bloque parcial leido
        // Obtenemos el numero del bloque fisico
        nbfisico = traducir_bloque_inodo(&inodo, primerBL, 0);
        // Miramos si hay bloque fisico, y si hay, entramos dentro del if
        if(nbfisico != FALLO){
            
            // Leemos el bloque fisico
            if(bread(nbfisico, buf_bloque) == FALLO){
                return FALLO;
            }

            // Escribimos (BLOCKSIZE - desp1) bytes, del (buf_bloque + desp1) en la posición buf_original
            memcpy(buf_original, buf_bloque + desp1, BLOCKSIZE - desp1);

        }else{
            return FALLO;
        }
        // Los bytes leidos en esta parte van a ser la longitud del bloque menos el desplazamiento
        leidos = BLOCKSIZE - desp1;

        //2. Bloques intermedios enteros
        for(int i = primerBL + 1; i < ultimoBL; i++){

            // Obtenemos el numero del bloque fisico sobre cada iteracion
            nbfisico = traducir_bloque_inodo(&inodo, i, 0);
            // Miramos si hay bloque fisico, y si hay, entramos dentro del if
            if(nbfisico != FALLO){
                
                // Leemos el bloque fisico
                if(bread(nbfisico, buf_bloque) == FALLO){
                    return FALLO;
                }

                // Escribimos (BLOCKSIZE) bytes, del (buf_bloque) en la posición (buf_original + (BLOCKSIZE - desp1) + (i - primerBL - 1) * BLOCKSIZ)
                memcpy(buf_original + (BLOCKSIZE - desp1) + (i - primerBL - 1) * BLOCKSIZE, buf_bloque, BLOCKSIZE);

            }else{
                return FALLO;
            }
            // Los bytes leidos en esta parte van a ser la longitud del bloque entero
            leidos += BLOCKSIZE;

        }

        //3. Ultimo bloque parcial
        // Obtenemos el numero del bloque fisico del ultimo bloque
        nbfisico = traducir_bloque_inodo(&inodo, ultimoBL, 0);
        // Miramos si hay bloque fisico, y si hay, entramos dentro del if
        if(nbfisico != FALLO){
            
            // Leemos el bloque fisico
            if(bread(nbfisico, buf_bloque) == FALLO){
                return FALLO;
            }

            // Escribimos (desp2 + 1) bytes, del (buf_bloque) en la posición (buf_original + (nbytes - desp2 - 1))
            memcpy(buf_original + (nbytes - desp2 - 1), buf_bloque, desp2 + 1);

        }else{
            return FALLO;
        }
        // Los bytes leidos en esta parte van a ser la longitud del desplazamiento final + 1
        leidos += desp2 + 1;
    }

    //Vamos a actualizar atime
    //Primero leemos el inodo
    if(leer_inodo(ninodo, &inodo) == FALLO){
        return leidos;
    }
    //Modificamos atime
    inodo.atime = time(NULL);
    //Lo volvemos a escribir
    if(escribir_inodo(ninodo, &inodo) == FALLO){
        return leidos;
    }

    // Si los bytes leidos no coinciden con los bytes que nos han pasado por parametro devuelve FALLO, 
    // sino devuelve el numero de bytes leidos
    if(leidos != nbytes){
        printf("mi_read_f: MAL\n\n");
        return FALLO;
    }else {
        printf("mi_read_f: BIEN\n");
        return nbytes;
    }

    return EXITO;

}

/*---------------------------------------------------------------------------------------------------------
* Obtiene metadatos de un fichero/directorio (tipo, permisis, cantidad de enlaces de entradas en directiorio, 
* tamaño en bytes lógicos, timestamps y cantidad de bloques ocupados en la zona de datos)
* Input:    ninodo: numero del inodo que se quiere obtener los satos
*           *p_stat: estructura de tipo STAT donde se almacenan los metadatos
* Output:   EXITO si ha ido bien o FALLO si no.
---------------------------------------------------------------------------------------------------------*/

int mi_stat_f(unsigned int ninodo, struct STAT *p_stat){

    struct inodo inodos;

    if(leer_inodo(ninodo, &inodos) == FALLO){
        return FALLO;
    }

    p_stat->atime = inodos.atime;
    p_stat->ctime = inodos.ctime;
    p_stat->mtime = inodos.mtime;
    p_stat->nlinks = inodos.nlinks;
    p_stat->permisos = inodos.permisos;
    p_stat->tipo = inodos.tipo;
    p_stat->tamEnBytesLog = inodos.tamEnBytesLog;
    p_stat->numBloquesOcupados = inodos.numBloquesOcupados;

    return EXITO;

}

/*---------------------------------------------------------------------------------------------------------
* Cambia los permisos de un fichero/directorio
* Input:    ninodo: numero de inodo que tenemos que cambiar los permisos
*           permisos: los nuevos permisos para actualizar
* Output:   nbloque: nº de bloque reservado
---------------------------------------------------------------------------------------------------------*/

int mi_chmod_f(unsigned int ninodo, unsigned char permisos){
    
    struct inodo inodos;

    if(leer_inodo(ninodo, &inodos) == FALLO){
        return FALLO;
    }

    inodos.permisos = permisos; //Cambiamos permisos
    inodos.ctime = time(NULL); //Reseteamos el ctime porque hemos modificado el inodo

    if(escribir_inodo(ninodo, &inodos) == FALLO){
        return FALLO;
    }

    return EXITO;
}