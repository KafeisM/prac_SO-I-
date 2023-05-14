#include "directorios.h"

//Sintaxis: ./mi_escribir <disco> </ruta_fichero> <texto> <offset>

int main(int argc, char **argv){

    char *directorio = argv[1];
    char *ruta = argv[2];
    int longitud = strlen(argv[2]);
    char *buf = argv[3];
    unsigned int longTexto = strlen(argv[3]);
    unsigned int offset = atoi(argv[4]);
    int bytes_escritos;

    if(argc != 5){
        fprintf(stderr, ROJO_T "Sintaxis: ./mi_escribir <disco> </ruta_fichero> <texto> <offset>\n");
        return FALLO;
    }

    //Miramos si es un fichero
    if(ruta[longitud - 1] == '/'){
        fprintf(stderr, ROJO_T "No es un fichero\n");
        return FALLO;
    }  

    //Montamos dispositivo virtual
    if(bmount(directorio) == FALLO){
        fprintf(stderr,ROJO_T "Error en bmount\n" RESET);
        return FALLO;
    }

    //Obtenemos la longitud del fichero a través del strlen
    printf("Longitud texto: %d\n", longTexto); 

    bytes_escritos = mi_write(ruta, buf, offset, longTexto);

    if(bytes_escritos < 0){
        mostrar_error_buscar_entrada(bytes_escritos);
        return FALLO;
    }

    //Desmontamos dispositivo virtual
    if(bumount() == FALLO){
        fprintf(stderr,ROJO_T "escribir.c: Error en bumount\n" RESET);
        return FALLO;
    }
    
    printf("Bytes escritos = %d\n", bytes_escritos);

    return EXITO;
}