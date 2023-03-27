#include "ficheros.h"

// ./escribir <nombre_dispositivo> <"$(cat fichero)"> <diferentes_inodos>
int main(int argc, char **argv){
    
    if((argc =! 4) ||(argv[1] == NULL) ||(argv[2] == NULL) ||(argv[3] == NULL)){
        return FALLO;
    }else{
        struct inodo inodo;

        char directorio = argv[1];
        int dif_inodos = atoi(argv[3]);
        int ninodo = reservar_inodo('f', 6);
        int longitud = strlen(argv[2]);
        char *buf = malloc(longitud);
        int escritos;
        
        int offsets[5] = {9000, 209000, 30725000, 409605000, 480000000};
        int longitud_offsets = sizeof(offsets) / sizeof(int);

        if(ninodo == FALLO){
            return FALLO;
        }

        if(bmount(directorio) == FALLO){
            return FALLO;
        }

        for(int i = 0; i < longitud_offsets; i++){
            printf("ninodo: %i", ninodo);
            printf("offset: %i", offsets[i]);
            escritos = mi_write_f(ninodo, buf, offsets[i], longitud);
            if(escritos == FALLO){
                return FALLO;
            }
            printf("escritos: %i", escritos);
        }

        

        return EXITO;
    }

}