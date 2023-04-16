/*JOSEP GABRIEL FORNÉS REYNÉS, JORDI FLORIT ENSENYAT, PAU GIRÓN RODRÍGUEZ*/

#include "bloques.h"

static int descriptor = 0;

int bmount(const char *camino){

    umask(000);
    descriptor = open(camino,O_RDWR | O_CREAT,0666);
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

    lseek(descriptor,nbloque * BLOCKSIZE,SEEK_SET);

    size_t bytes_escritos = write(descriptor, buf, BLOCKSIZE);

    if (bytes_escritos == FALLO){
        fprintf(stderr,"Error bwrite %d: %s\n",errno,strerror(errno));
        return FALLO;
    }

    return bytes_escritos;

}


int bread(unsigned int nbloque, void *buf){

    off_t desplazamiento = nbloque * BLOCKSIZE;

    lseek(descriptor,desplazamiento,SEEK_SET);

    size_t bytes_leidos = read(descriptor,  buf,BLOCKSIZE);

    if (bytes_leidos == FALLO){
        fprintf(stderr,"Error bread %d: %s\n",errno,strerror(errno));
        return FALLO;
    }

    return bytes_leidos;

}