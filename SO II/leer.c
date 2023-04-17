/*JOSEP GABRIEL FORNÉS REYNÉS, JORDI FLORIT ENSENYAT, PAU GIRÓN RODRÍGUEZ*/

#include "ficheros.h"

// ./leer <nombre_dispositivo> <ninodo>

/*---------------------------------------------------------------------------------------------------------
* Va a leer un inodo pasado por parámetro (igual que la función CAT de LINUX)
* Input:    argc: numero de argumentos que se han introducido
*           **argv: array de punteros a caracteres
* Output:   FALLO o EXITO
---------------------------------------------------------------------------------------------------------*/
int main(int argc, char **argv){

    //Si la sintaxis es incorrecta, va a saltar un mensaje de error
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

    //Guardamos el tamaño del buffer a memoria
    memset(buffer_texto, 0, tamBuffer);

    //Montamos dispositivo virtual
    if (bmount(directorio) == FALLO){
        fprintf(stderr, ROJO_T"leer.c: Error en bmount\n"RESET);
        return FALLO;
    }

    //Leemos el inodo y obtenemos el numero de bytes leidos
    int leidos = mi_read_f(ninodo, buffer_texto, offset, tamBuffer);
    totalLeidos = leidos;

    //Mientras los bytes leidos sean mayor a 0 se van leyendo los inodos
    while (leidos > 0){
        write(1, buffer_texto, leidos); //Escribimos en el buffer de texto
        memset(buffer_texto, 0, tamBuffer); //Reservamos memoria
        offset += tamBuffer;
        leidos = mi_read_f(ninodo, buffer_texto, offset, tamBuffer);    //Volvemos a leer inodo
        totalLeidos += leidos;  
    }

    //Debemos escribir el último leido
    write(1, buffer_texto, leidos);

    //Leemos inodo
    if (leer_inodo(ninodo, &inodo)){
        fprintf(stderr, ROJO_T"leer.c: Error en leer inodo\n"RESET);
        return FALLO;
    }

    fprintf(stderr, "\ntotal_bytesleidos: %d\n", totalLeidos);
    fprintf(stderr, "tamEnBytesLog: %d\n", inodo.tamEnBytesLog);

    //Desmontamos dispositivo virtual
    if (bumount() == FALLO){
        fprintf(stderr, ROJO_T"leer.c: Error en desmontar el dispositivo\n"RESET);
        return FALLO;
    }

    return EXITO;
}