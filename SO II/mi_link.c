/*JOSEP GABRIEL FORNÉS REYNÉS, JORDI FLORIT ENSENYAT, PAU GIRÓN RODRÍGUEZ*/
#include "directorios.h"

int main(int argc, char const *argv[]){

    if (argc != 4){
        fprintf(stderr, ROJO_T "Sintaxis: ./mi_link <disco> </ruta_fichero_original> </ruta_enlace>\n"RESET);
        return FALLO;
    }

    if (argv[2][stlen(argv[2])-1] == '/' && argv[3][strlen(argv[3])-1] == '/'){
        fprintf(stderr, ROJO_T"Solo se permiten ficheros"RESET);
        return FALLO;
    }

    bmount(argv[1]);
    int error = mi_link(argv[2],argv[3]);

    if (error<0){
        mostrar_error_buscar_entrada(error);
        return FALLO;
    }
    bumount(argv[1]);
    
}