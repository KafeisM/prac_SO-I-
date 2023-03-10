#include <string.h> 
#include "ficheros_basico.h"

static int descriptor = 0;

int main(int argc, char **argv){


    if(argc -1 != 2){
        fprintf(stderr,ROJO_T"Sintaxis correcta: ./mi_mkfs <nombre_dispositivo> <nbloques>\n"RESET);
        exit(0);
    }

    //MONTAR DISCO
    descriptor = bmount(argv[1]);
    int num_blocks = atoi(argv[2]);
    unsigned char buffer[BLOCKSIZE];
    
    memset(&buffer, 0, sizeof(buffer));

    //INICIALIZAR a 0's
    for (size_t i = 0; i < num_blocks; i++){
        bwrite(i,&buffer);
        /*if(bw == FALLO){
            fprintf(stderr,ROJO_T"ERROR INICIALIZACIÓN DE FICHERO (bwrite)\n"RESET);
        }else if (bw < BLOCKSIZE){
            fprintf(stderr,ROJO_T"ERROR INICIALIZACIÓN DE FICHERO (bwrite - blocksize)\n"RESET);
        }*/
    }

    initSB(num_blocks, num_blocks/4);

    /*if(isb == FALLO){
        fprintf(stderr,ROJO_T"ERROR INICIALIZACIÓN DE SUPERBLOQUE %i \n"RESET,isb);
    }else if (isb < BLOCKSIZE){
        fprintf(stderr,ROJO_T"ERROR INICIALIZACIÓN DE SUPERBLOQUE (bwrite - blocksize)\n"RESET);
    }*/

    if(initMB() == FALLO){
        fprintf(stderr,ROJO_T"ERROR INICIALIZACIÓN DE MAPA DE BITS\n"RESET);
    }

    if(initAI() == FALLO){
        fprintf(stderr,ROJO_T"ERROR INICIALIZACIÓN DE ARRAY DE INODOS\n"RESET);
    }

    if(reservar_inodo('d',7) == FALLO){
        fprintf(stderr,ROJO_T"ERROR EN LA CREACIÓN DEL DIRECTORIO RAÍZ");
    }

    //DESMONTAR DISCO
    if(bumount() == -1){
        fprintf(stderr,ROJO_T"ERROR INICIALIZACIÓN DE FICHERO\n"RESET);
    }
        
    return 0;

}