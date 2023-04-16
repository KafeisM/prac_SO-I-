/*JOSEP GABRIEL FORNÉS REYNÉS, JORDI FLORIT ENSENYAT, PAU GIRÓN RODRÍGUEZ*/

#include "ficheros.h"

// ./leer <nombre_dispositivo> <ninodo>
int main(int argc, char **argv){

    if ((argc != 3)){
        fprintf(stderr, ROJO_T "Sintaxis: ./leer <nombre_dispositivo> <ninodo>\n" RESET);
        return FALLO;
    }

    struct inodo inodo;
    char *directorio = argv[1];
    int ninodo = atoi(argv[2]);

    int tamBuffer = 1500;

    int totalLeidos = 0;
    unsigned int offset = 0;
    char buffer_texto[tamBuffer];

    memset(buffer_texto, 0, tamBuffer);

    if (bmount(directorio) == FALLO){
        fprintf(stderr, "Error en montar el dispositivo");
        return FALLO;
    }

    int leidos = mi_read_f(ninodo, buffer_texto, offset, tamBuffer);
    totalLeidos = leidos;

    while (leidos > 0){
        write(1, buffer_texto, leidos);
        memset(buffer_texto, 0, tamBuffer);
        offset += tamBuffer;
        leidos = mi_read_f(ninodo, buffer_texto, offset, tamBuffer);
        totalLeidos += leidos;
    }

    write(1, buffer_texto, leidos);

    if (leer_inodo(ninodo, &inodo)){
        fprintf(stderr, "Error en leer inodo");
        return FALLO;
    }

    fprintf(stderr, "\ntotal_bytesleidos: %d\n", totalLeidos);
    fprintf(stderr, "tamEnBytesLog: %d\n", inodo.tamEnBytesLog);

    if (bumount() == FALLO){
        fprintf(stderr, "Error en desmontar el dispositivo");
        return FALLO;
    }

    return EXITO;
}