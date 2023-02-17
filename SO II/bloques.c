#include "bloques.h"

static int descriptor = 0;

int bmount(const char *camino){

    umask(000);
    descriptor = open(camino,O_RDWR | O_CREAT);
    if (descriptor < 0){
        perror("Error en la apertura del fichero");
        return FALLO;
    }
    return descriptor;

}

int bumount(){

    if (close(descriptor) < 0){
        return FALLO;
    }
    return EXITO;

}


int bwrite(unsigned int nbloque, const void *buf){

    off_t desplazamiento = nbloque * BLOCKSIZE;

    if (lseek(descriptor,desplazamiento,SEEK_SET) < 0){
        return FALLO;
    }

    if (write(descriptor, buf, BLOCKSIZE) < 0){
        return FALLO;
    }

    return BLOCKSIZE;

}


int bread(unsigned int nbloque, void *buf){

}