#include "ficheros.h"

// ./escribir <nombre_dispositivo> <"$(cat fichero)"> <diferentes_inodos>
int main(int argc, char **argv){
    
    if((argv[1] == NULL) ||(argv[2] == NULL) || (argv[3] == NULL)){
        return FALLO;
    }else{
        char directorio = argv[1];
        int longitud = argv[2];
        int inodos = atoi(argv[3]);
        int ninodo = reservar_inodo('f',6);

    }

}