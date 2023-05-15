#include "directorios.h"

//Sintaxis: ./mi_chmod <disco> <permisos> </ruta>
int main(int argc, char const *argv[]){

    unsigned char permisos = atoi(argv[2]);

    if(argc != 4){
        fprintf(stderr, ROJO_T "Sintaxis: ./mi_chmod <disco> <permisos> </ruta>\n");
        return FALLO;
    }

    if(atoi(argv[2]) < 0 || atoi(argv[2]) > 7){
        fprintf(stderr, ROJO_T "Permisos incorrectos(0-7)\n");
        return FALLO;
    }

    bmount(argv[1]);

    int error = mi_chmod(argv[3],permisos);
    if(error < 0){
        mostrar_error_buscar_entrada(error);
        return FALLO;
    }
    bumount();
    return EXITO;
}
