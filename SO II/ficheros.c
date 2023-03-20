#include "ficheros.h"

int mi_write_f(unsigned int ninodo, const void *buf_original, unsigned int offset, unsigned int nbytes){

}

int mi_read_f(unsigned int ninodo, void *buf_original, unsigned int offset, unsigned int nbytes){

}

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