/*JOSEP GABRIEL FORNÉS REYNÉS, JORDI FLORIT ENSENYAT, PAU GIRÓN RODRÍGUEZ*/

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
        fprintf(stderr, ROJO_T "mi_write_f: Error leer_inodo\n" RESET);
        return escritos;
    }

    //Miramos si los permisos corresponden a los de escribir
    if ((inodo.permisos & 2) != 2){
        fprintf(stderr, ROJO_T "No hay permisos de escritura\n" RESET);
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

        mi_waitSem();
        // Obtenemos el numero del bloque fisico
        nbfisico = traducir_bloque_inodo(&inodo, primerBL, 1);
        // Miramos si hay bloque fisico, y si hay, entramos dentro del if
        if(nbfisico != FALLO){

            // Leemos el bloque fisico
            if(bread(nbfisico, buf_bloque) == FALLO){
                fprintf(stderr, ROJO_T "mi_write_f: Error bread\n" RESET);
                return FALLO;
            }

            // Escribimos nbytes, del buf_original en la posición (buf_bloque + desp1)
            memcpy(buf_bloque + desp1, buf_original, nbytes);

            // Escribimos el bloque fisico
            if(bwrite(nbfisico, buf_bloque) == FALLO){
                fprintf(stderr, ROJO_T "mi_write_f: Error bwrite\n" RESET);
                return FALLO;
            }

        }

        mi_signalSem();
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
                fprintf(stderr, ROJO_T "mi_write_f: Error bread\n" RESET);
                return FALLO;
            }

            // Escribimos nbytes, del buf_original en la posición (buf_bloque + desp1)
            memcpy(buf_bloque + desp1, buf_original, BLOCKSIZE - desp1);

            // Escribimos el bloque fisico
            if(bwrite(nbfisico, buf_bloque) == FALLO){
                fprintf(stderr, ROJO_T "mi_write_f: Error bwrite\n" RESET);
                return FALLO;
            }

        }
        // Los bytes escritos en esta parte van a ser la longitud del bloque menos el desplazamiento
        escritos += BLOCKSIZE - desp1;

        //2. Bloques intermedios enteros
        for(int i = primerBL + 1; i < ultimoBL; i++){

            mi_waitSem();
            // Obtenemos el numero del bloque fisico sobre cada iteracion
            nbfisico = traducir_bloque_inodo(&inodo, i, 1);
            // Miramos si hay bloque fisico, y si hay, entramos dentro del if
            if(nbfisico != FALLO){
                
                mi_signalSem();
                // Escribimos el bloque correspondiente
                if(bwrite(nbfisico, buf_original + (BLOCKSIZE - desp1) + (i - primerBL - 1) * BLOCKSIZE) == FALLO){
                    fprintf(stderr, ROJO_T "mi_write_f: Error bwrite\n" RESET);
                    return FALLO;
                }

            }
            mi_signalSem();
            // Los bytes leidos en esta parte van a ser la longitud del bloque entero
            escritos += BLOCKSIZE;

        }

        mi_waitSem();
        //3. Ultimo bloque parcial
        // Obtenemos el numero del bloque fisico
        nbfisico = traducir_bloque_inodo(&inodo, ultimoBL, 1);
        // Miramos si hay bloque fisico, y si hay, entramos dentro del if
        if(nbfisico != FALLO){

            mi_signalSem();
            // Leemos el bloque fisico
            if(bread(nbfisico, buf_bloque) == FALLO){
                fprintf(stderr, ROJO_T "mi_write_f: Error bread\n" RESET);
                return FALLO;
            }

            // Escribimos nbytes, del buf_original + (nbytes - (desp2 + 1)) en la posición buf_bloque
            memcpy(buf_bloque, buf_original + (nbytes - (desp2 + 1)), desp2 + 1);
            
            // Escribimos el bloque fisico
            if(bwrite(nbfisico, buf_bloque) == FALLO){
                fprintf(stderr, ROJO_T "mi_write_f: Error bwrite\n" RESET);
                return FALLO;
            }

        }
        mi_signalSem();
        // Los bytes escritos seran el desplazamiento final mas 1
        escritos += desp2 + 1;
    }


    mi_waitSem();
    //Vamos a actualizar mtime y ctime

    // Miramos si vamos hemos escrito más alla del EOF
    if((ultimoBL * BLOCKSIZE + desp2 + 1) > inodo.tamEnBytesLog){
        inodo.tamEnBytesLog = ultimoBL * BLOCKSIZE + desp2 + 1;
        inodo.ctime = time(NULL);
    }
    //Modificamos mtime
    inodo.mtime = time(NULL);
    //Lo volvemos a escribir
    if(escribir_inodo(ninodo, &inodo) == FALLO){
        fprintf(stderr, ROJO_T "mi_write_f: Error escribir_inodo\n" RESET);
        return escritos;
    }
    mi_signalSem();

    // Si los bytes escritos no coinciden con los bytes que nos han pasado por parametro devuelve FALLO, 
    // sino devuelve el numero de bytes leidos
    if(escritos != nbytes){
        fprintf(stderr, ROJO_T "mi_read_f: Error escritos\n" RESET);
        return FALLO;
    }else {
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
        fprintf(stderr, ROJO_T "mi_read_f: Error leer_inodo\n" RESET);
        return leidos;
    }

    //Miramos si los permisos corresponden a los de leer
    if ((inodo.permisos & 4) != 4){
        fprintf(stderr, ROJO_T "No hay permisos de lectura\n" RESET);
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
                fprintf(stderr, ROJO_T "mi_read_f: Error bread\n" RESET);
                return FALLO;
            }

            // Escribimos nbytes, del (buf_bloque + desp1) en la posición buf_original
            memcpy(buf_original, buf_bloque + desp1, nbytes);
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
                fprintf(stderr, ROJO_T "mi_read_f: Error bread\n" RESET);
                return FALLO;
            }

            // Escribimos (BLOCKSIZE - desp1) bytes, del (buf_bloque + desp1) en la posición buf_original
            memcpy(buf_original, buf_bloque + desp1, BLOCKSIZE - desp1);

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
                    fprintf(stderr, ROJO_T "mi_read_f: Error bread\n" RESET);
                    return FALLO;
                }

                // Escribimos (BLOCKSIZE) bytes, del (buf_bloque) en la posición (buf_original + (BLOCKSIZE - desp1) + (i - primerBL - 1) * BLOCKSIZ)
                memcpy(buf_original + (BLOCKSIZE - desp1) + (i - primerBL - 1) * BLOCKSIZE, buf_bloque, BLOCKSIZE);

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
                fprintf(stderr, ROJO_T "mi_read_f: Error bread\n" RESET);
                return FALLO;
            }

            // Escribimos (desp2 + 1) bytes, del (buf_bloque) en la posición (buf_original + (nbytes - desp2 - 1))
            memcpy(buf_original + (nbytes - desp2 - 1), buf_bloque, desp2 + 1);

        }
        // Los bytes leidos en esta parte van a ser la longitud del desplazamiento final + 1
        leidos += desp2 + 1;
    }

    mi_waitSem();
    //Vamos a actualizar atime
    
    //Modificamos atime
    inodo.atime = time(NULL);
    //Lo volvemos a escribir
    if(escribir_inodo(ninodo, &inodo) == FALLO){
        fprintf(stderr, ROJO_T "mi_read_f: Error escribir_inodo\n" RESET);
        return leidos;
    }
    mi_signalSem();

    // Si los bytes leidos no coinciden con los bytes que nos han pasado por parametro devuelve FALLO, 
    // sino devuelve el numero de bytes leidos
    if(leidos != nbytes){
        fprintf(stderr, ROJO_T "mi_read_f: Error leidos\n" RESET);
        return FALLO;
    }else {
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

    //Leemos inodo
    if(leer_inodo(ninodo, &inodos) == FALLO){
        fprintf(stderr, ROJO_T "mi_stat_f: Error leer_inodoºn\n" RESET);
        return FALLO;
    }

    //Pasamos todos los datos que se encuentran en el struct inodo al struct stat pasado por parametro
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
    
    mi_waitSem();
    struct inodo inodos;

    //Leemos inodo
    if(leer_inodo(ninodo, &inodos) == FALLO){
        fprintf(stderr, ROJO_T "mi_chmod_f: Error leer_inodo\n" RESET);
        mi_signalSem();
        return FALLO;
    }

    inodos.permisos = permisos; //Cambiamos permisos
    inodos.ctime = time(NULL); //Reseteamos el ctime porque hemos modificado el inodo

    //Escribimos inodo
    if(escribir_inodo(ninodo, &inodos) == FALLO){
        fprintf(stderr, ROJO_T "mi_chmod_f: Error escribir_inodo\n" RESET);
        mi_signalSem();
        return FALLO;
    }
    mi_signalSem();
    return EXITO;
}

/*---------------------------------------------------------------------------------------------------------
* Trunca un fichero liberando los bloques necesarios
* Input:    ninodo: numero de inodo donde está el fichero
*           nbytes: numero de bytes a liberar
* Output:   bloquesL: bloques liberados
---------------------------------------------------------------------------------------------------------*/

int mi_truncar_f(unsigned int ninodo, unsigned int nbytes){

    struct inodo inodo;
    unsigned int primerBL = 0;

    leer_inodo(ninodo, &inodo);

    if ((inodo.permisos & 2) != 2){
        fprintf(stderr, ROJO_T"No tiene permiso de escritura\n"RESET);
        return FALLO;
    }

    if (nbytes >= inodo.tamEnBytesLog){
        return FALLO;
    }

    if (nbytes % BLOCKSIZE == 0){
        primerBL = nbytes/BLOCKSIZE;
    }else{
        primerBL = nbytes/BLOCKSIZE + 1;
    }

    unsigned int bloquesL = liberar_bloques_inodo(primerBL, &inodo);
    inodo.tamEnBytesLog = nbytes;
    inodo.mtime = time(NULL);
    inodo.ctime = time(NULL);
    inodo.numBloquesOcupados -= bloquesL;
    escribir_inodo(ninodo, &inodo);

    return bloquesL;

}