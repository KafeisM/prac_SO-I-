#include "directorios.h"

//Sintaxis: ./mi_mkdir <disco> <permisos> </ruta>

int main(int argc, char **argv){

    unsigned char permisos = atoi(argv[2]);
    int longitud = strlen(argv[3]);
    
    if(argc != 4){
        fprintf(stderr, ROJO_T "Sintaxis: ./mi_mkdir <disco> <permisos> </ruta>\n");
        return FALLO;
    }

    if(atoi(argv[2]) < 0 || atoi(argv[2]) > 7){
        fprintf(stderr, ROJO_T "Error: Los permisos no son válidos. Modo <<%d>> inválido.\n"RESET,atoi(argv[2]));
        return FALLO;
    }

    //Si es un fichero
    if(argv[3][longitud - 1] != '/'){
        fprintf(stderr, ROJO_T "No es una ruta de directorio válida\n");

    }else{//Si no lo es
        
        //Montamos disco
        if(bmount(argv[1]) == FALLO){
            return FALLO;
        }

        int error = mi_creat(argv[3], permisos);
        if(error < 0){
            mostrar_error_buscar_entrada(error);
            return FALLO;
        }
        bumount();
    }

    return EXITO;
}