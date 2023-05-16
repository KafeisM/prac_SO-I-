/*JOSEP GABRIEL FORNÉS REYNÉS, JORDI FLORIT ENSENYAT, PAU GIRÓN RODRÍGUEZ*/

#include "directorios.h"

/*---------------------------------------------------------------------------------------------------------
* Programa (comando) que lista el contenido de un directorio (nombres de las entradas), 
* llamando a la función mi_dir()
* Input:    argc: numero de argumentos que se han introducido
*           **argv: array de punteros a caracteres
* Output:   FALLO o EXITO
---------------------------------------------------------------------------------------------------------*/

//Sintaxis: ./mi_ls <disco> </ruta_directorio>

int main(int argc, char **argv){

    char buffer[TAMBUFFER];
    int total;

    //comprobamos sintaxis
    if(argc != 3){
        fprintf(stderr, ROJO_T "Sintaxis: ./mi_ls <disco> </ruta_directorio>\n");
        return FALLO;
    }

    //montamos dispositivo
    if(bmount(argv[1]) == FALLO){
        return FALLO;
    }

    //ponemos 0s en el buffer
    memset(buffer, 0, TAMBUFFER);

    //llamamos a la funcion y nos devuelve el total de entradas
    total = mi_dir(argv[2], buffer);
    
    //si las entradas es menor que 0, devuelve FALLO
    if(total <= 0){
        return FALLO;

    }else{  //Si no, imprimimos las entradas que se encuentran dentro de el buffer
        printf("Total: %d\n", total);

        printf("Tipo\tPermisos\tmTime\t\tTamaño\tNombre\n");
        printf("--------------------------------------------------------------------------------\n");
        printf("%s\n", buffer);
    }

    //desmontamos dispositivo
    if(bumount() == FALLO){
        return FALLO;
    }

    return EXITO;
}