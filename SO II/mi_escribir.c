#include "directorios.h"

/*---------------------------------------------------------------------------------------------------------
* Permite escribir texto en una posición de un fichero (offset)
* Input:    argc: numero de argumentos que se han introducido
*           **argv: array de punteros a caracteres
* Output:   FALLO o EXITO
---------------------------------------------------------------------------------------------------------*/

//Sintaxis: ./mi_escribir <disco> </ruta_fichero> <texto> <offset>

int main(int argc, char **argv){

    //variables
    char *directorio = argv[1];
    char *ruta = argv[2];
    int longitud = strlen(argv[2]);
    char *buf = argv[3];
    unsigned int longTexto = strlen(argv[3]);
    unsigned int offset = atoi(argv[4]);
    int Bescritos;

    //comprobamos sintaxis
    if(argc != 5){
        fprintf(stderr, ROJO_T "Sintaxis: ./mi_escribir <disco> </ruta_fichero> <texto> <offset>\n");
        return FALLO;
    }

    //miramos si es un fichero, si no lo hes lo imprimimos por pantalla
    if(ruta[longitud - 1] == '/'){
        fprintf(stderr, ROJO_T "No es un fichero\n");
        return FALLO;
    }  

    //montamos dispositivo
    if(bmount(directorio) == FALLO){
        fprintf(stderr,ROJO_T "Error en bmount\n" RESET);
        return FALLO;
    }

    //imprimimos la longitud
    printf("Longitud texto: %d\n", longTexto); 

    //escribimos y obtenemos los bytes escritos
    Bescritos = mi_write(ruta, buf, offset, longTexto);

    //miramos si hay error y lo tratamos
    if(Bescritos < 0){
        mostrar_error_buscar_entrada(Bescritos);
        return FALLO;
    }

    //desmontamos dispositivo
    if(bumount() == FALLO){
        return FALLO;
    }
    
    //imprimimos los bytes escritos
    printf("Bytes escritos = %d\n", Bescritos);

    return EXITO;
}