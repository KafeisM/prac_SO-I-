#include "directorios.h"

//Sintaxis: ./mi_cat <disco> </ruta_fichero>

int main(int argc, char **argv){

    char *directorio = argv[1];
    
    if (argc != 3){
        fprintf(stderr, ROJO_T"Sintaxis: ./mi_cat <disco> </ruta_fichero>\n");
        return FALLO;
    }
    
    //montamos dispositivo
    if (bmount(directorio) == FALLO){
        fprintf(stderr, "Error al montar el disco.\n");
        return FALLO;
    }


    int tambuffer = BLOCKSIZE * 4;
    int bytes_leidos = 0;
    int offset = 0;
    
    char buffer[tambuffer];

    memset(buffer, 0, sizeof(buffer));

    int bytes_leidosAux = mi_read(argv[2], buffer, offset, tambuffer);

    while (bytes_leidosAux > 0){


        bytes_leidos += bytes_leidosAux;
        write(1, buffer, bytes_leidosAux);

        memset(buffer, 0, sizeof(buffer));
        offset += tambuffer;

        bytes_leidosAux = mi_read(argv[2], buffer, offset, tambuffer);
    }

    fprintf(stderr, " \n");

    if (bytes_leidos < 0){
        mostrar_error_buscar_entrada(bytes_leidos);
        bytes_leidos = 0;
    } 

    fprintf(stderr, "\nTotal_leidos: %d\n", bytes_leidos);

    if(bumount() == FALLO){
        return FALLO;
    }

    return EXIT_SUCCESS;
}