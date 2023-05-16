/*JOSEP GABRIEL FORNÉS REYNÉS, JORDI FLORIT ENSENYAT, PAU GIRÓN RODRÍGUEZ*/


#include "directorios.h"

/*---------------------------------------------------------------------------------------------------------
* Programa (comando) que muestra TODO el contenido de un fichero
* Input:    argc: numero de argumentos que se han introducido
*           **argv: array de punteros a caracteres
* Output:   FALLO o EXITO
---------------------------------------------------------------------------------------------------------*/

//Sintaxis: ./mi_cat <disco> </ruta_fichero>

int main(int argc, char **argv){

    //variables
    char *directorio = argv[1];
    int tambuffer = BLOCKSIZE * 4;
    int Bleidos = 0;
    int offset = 0;
    char buffer[tambuffer];
    
    //comprobamos sintaxis
    if (argc != 3){
        fprintf(stderr, ROJO_T"Sintaxis: ./mi_cat <disco> </ruta_fichero>\n");
        return FALLO;
    }
    
    //montamos dispositivo
    if (bmount(directorio) == FALLO){
        fprintf(stderr, "Error al montar el disco.\n");
        return FALLO;
    }

    //ponemos a 0s el buffer
    memset(buffer, 0, sizeof(buffer));

    //obtenemos los bytes leidos dentro de una variable auxiliar
    int aux = mi_read(argv[2], buffer, offset, tambuffer);
    //comprobamos si no hay error
    while (aux > 0){

        //vamos actualizando los bytes leidos
        Bleidos += aux;

        //escribimos el contenido del buffer en la variable auxiliar
        write(1, buffer, aux);

        //volvemos a poner a 0 el buffer (lo limpiamos)
        memset(buffer, 0, sizeof(buffer));

        //actualizamos offset
        offset += tambuffer;

        //volvemos a leer en el offset
        aux = mi_read(argv[2], buffer, offset, tambuffer);
    }

    //miramos los bytes leidos y si hay error lo tratamos
    if (Bleidos < 0){
        mostrar_error_buscar_entrada(Bleidos);
        Bleidos = 0;
    } 

    fprintf(stderr, "\nTotal_leidos: %d\n", Bleidos);

    //desmontamos el dispositivo
    if(bumount() == FALLO){
        return FALLO;
    }

    return EXITO;
}