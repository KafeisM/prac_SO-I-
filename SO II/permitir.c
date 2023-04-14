#include "ficheros.h"

//permitir <nombre_dispositivo> <ninodo> <permisos>
int main(int argc, char **argv){

    if(argc != 4){
        fprintf(stderr,ROJO_T "Sintaxis: ./permitir <nombre_dispositivo> <ninodo> <permisos>\n" RESET);
        return FALLO;
    }
    
    char *directorio = argv[1];
    int ninodo = atoi(argv[2]);
    int permisos = atoi(argv[3]);

    if(bmount(directorio) == FALLO){
        return FALLO;
    }
        
    if(mi_chmod_f(ninodo, permisos) == FALLO){
        return FALLO;
    }

    if(bumount(directorio) == FALLO){
        return FALLO;
    }

    return EXITO;
}