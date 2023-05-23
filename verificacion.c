/*JOSEP GABRIEL FORNÉS REYNÉS, JORDI FLORIT ENSENYAT, PAU GIRÓN RODRÍGUEZ*/
#include "verificacion.h"

int main(int argc, char **argv){

    struct STAT stat;
    int numeroEntradas = 0;

    if (argc != 3) {
      fprintf(stderr, ROJO_T"Sintaxis: ./verificacion <nombre_dispositivo> <directorio_simulación>\n" RESET);
      exit(FALLO);
    }

    if(bmount(argv[1]) == FALLO){
        fprintf(stderr, ROJO_T"Error montar dispositivo"RESET);
        return FALLO;
    }

    mi_stat(argv[2], &stat);

    numeroEntradas = (stat.tamEnBytesLog / sizeof(struct entrada)); //obtenemos numero de entradas
    
    fprintf("Numero de entradas: %d", numeroEntradas);


}

