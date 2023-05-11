#include "directorios.h"

//Sintaxis: ./mi_cat <disco> </ruta_fichero>

int main(int argc, char **argv){
    //muestra TODO el contenido de un fichero

    char *directorio = argv[1];
    char *ruta = argv[2];
    int longitud = strlen(argv[2]);
    int bytes_leidos;
    int tambuffer = BLOCKSIZE * 4;
    char buf[tambuffer]; 
    int offset = 0;

    if(argc != 3){
        fprintf(stderr, ROJO_T "Sintaxis: ./mi_cat <disco> </ruta_fichero>\n");
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

    bytes_leidos = mi_read(ruta, buf, offset, tambuffer);

    //Desmontamos dispositivo virtual
    if(bumount() == FALLO){
        fprintf(stderr,ROJO_T "escribir.c: Error en bumount\n" RESET);
        return FALLO;
    }
    
    printf("Bytes leidos = %d\n", bytes_leidos);

    return EXITO;
}