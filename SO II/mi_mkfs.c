/*JOSEP GABRIEL FORNÉS REYNÉS, JORDI FLORIT ENSENYAT, PAU GIRÓN RODRÍGUEZ*/

#include <string.h> 
#include "ficheros_basico.h"
#include "directorios.h"

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
        bwrite(i,buffer);  
    }

    //INICIALIZAR EL SUPERBLOQUE
    initSB(num_blocks, num_blocks/4);   

    //INICIALIZAR EL MAPA DE BITS
    if(initMB() == FALLO){
        fprintf(stderr,ROJO_T"mi_mkfs: ERROR INICIALIZACIÓN DE MAPA DE BITS\n"RESET);
    }

    //INICIALIZAR LA ARRAY DE INODOS
    if(initAI() == FALLO){
        fprintf(stderr,ROJO_T"mi_mkfs: ERROR INICIALIZACIÓN DE ARRAY DE INODOS\n"RESET);
    }

    //CREAR INODO RAIZ
    if(reservar_inodo('d',7) == FALLO){ 
        fprintf(stderr,ROJO_T"mi_mkfs: ERROR EN LA CREACIÓN DEL DIRECTORIO RAÍZ");
    }

    //DESMONTAR DISCO
    if(bumount() == FALLO){
        fprintf(stderr,ROJO_T"mi_mkfs: ERROR INICIALIZACIÓN DE FICHERO\n"RESET);
    }
        
    return EXITO;

}