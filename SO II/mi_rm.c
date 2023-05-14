/*JOSEP GABRIEL FORNÉS REYNÉS, JORDI FLORIT ENSENYAT, PAU GIRÓN RODRÍGUEZ*/
#include "directorios.h"

int main(int argc, char const *argv[]){
    
    int error;

    if (argc != 3){
        fprintf(stderr, ROJO_T "Sintaxis: ./mi_rm disco /ruta\n"RESET);
        return FALLO;
    }

    if(bmount(argv[1]) == FALLO){
        fprintf(stderr,ROJO_T "Error en bmount\n" RESET);
        return FALLO;
    }   
    
    if(mi_unlink(argv[2]) == FALLO){
        return FALLO;
    }

    bumount();

    return EXITO;
}
