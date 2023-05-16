/*JOSEP GABRIEL FORNÉS REYNÉS, JORDI FLORIT ENSENYAT, PAU GIRÓN RODRÍGUEZ*/

#include "bloques.h"
#include "semaforo_mutex_posix.h"

static int descriptor = 0;
static sem_t *mutex;
static unsigned int inside_sc=0;

/*---------------------------------------------------------------------------------------------------------
* Función para montar el dispositiivo virtual
* Input:    *camino: nombre del dispositivo
* Output:   FALLO o descriptor de fichero
---------------------------------------------------------------------------------------------------------*/
int bmount(const char *camino){

    if (!mutex){
        mutex = initSem();
        if (mutex == SEM_FAILED){
            return FALLO;
        }
    }
    umask(000);
    descriptor = open(camino,O_RDWR | O_CREAT,0666);    //abrimos fichero existente
    if (descriptor < 0){
        fprintf(stderr,ROJO_T"bmoubt: Error en la apertura del fichero " RESET);
        return FALLO;
    }
    return descriptor;

}

/*---------------------------------------------------------------------------------------------------------
* Función para desmontar el disposiitivo virtual
* Input:    -
* Output:   FALLO o EXITO
---------------------------------------------------------------------------------------------------------*/
int bumount(){

    deleteSem();
    //Cerramos descriptor
    if (close(descriptor) < 0){
        fprintf(stderr, ROJO_T "bumount: Error desmontar dispositivo " RESET);
        return FALLO;
    }
    return EXITO;

}

/*---------------------------------------------------------------------------------------------------------
* Escribe un bloque en el dispositivo virtual, en el bloque fisico especificado por nbloque
* Input:    nbloque: numero de bloque físico
*           *buf: apunta a un buffer de memoria
* Output:   FALLO o numero de bytes escritos
---------------------------------------------------------------------------------------------------------*/
int bwrite(unsigned int nbloque, const void *buf){

    //movemos el puntero del fichero en el offset correcto
    lseek(descriptor,nbloque * BLOCKSIZE,SEEK_SET);

    //Volcamos el contenido del buffer en dicha posicion del dispositivo virtual
    size_t bytes_escritos = write(descriptor, buf, BLOCKSIZE);

    if (bytes_escritos == FALLO){
        fprintf(stderr,ROJO_T"bwrite %d: %s\n"RESET,errno,strerror(errno));
        return FALLO;
    }

    return bytes_escritos;

}

/*---------------------------------------------------------------------------------------------------------
* Lee un bloque del dispositivo virtual, que se corresponde con el bloque físico especificado en nbloque
* Input:    nbloque: numero de bloque físico
*           *buf: apunta a un buffer de memoria
* Output:   FALLO o numero de bytes leidos
---------------------------------------------------------------------------------------------------------*/
int bread(unsigned int nbloque, void *buf){

    //calculamos el desplazamiento dentro del dispositivo virtual donde hay que leer
    off_t desplazamiento = nbloque * BLOCKSIZE;

    //movemos el puentero del fichero en el offset correcto
    lseek(descriptor,desplazamiento,SEEK_SET);

    //volcamos en el buffer el contenido de los nbytes a partir de dicha posición del dispositivo virtual
    size_t bytes_leidos = read(descriptor,  buf,BLOCKSIZE);

    if (bytes_leidos == FALLO){
        fprintf(stderr,ROJO_T"bread %d: %s\n"RESET,errno,strerror(errno));
        return FALLO;
    }

    return bytes_leidos;

}

void mi_waitSem(){
    if (!inside_sc){
        waitSem(mutex);
    }
    inside_sc++;
}

void mi_signalSem(){
    inside_sc--;
    if (!inside_sc){
        signalSem(mutex);
    }
}