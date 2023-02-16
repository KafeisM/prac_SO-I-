#include "bloques.h"

int main(){
    return 0;
}

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

}


int bread(unsigned int nbloque, void *buf){

}