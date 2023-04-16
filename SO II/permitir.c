/*JOSEP GABRIEL FORNÉS REYNÉS, JORDI FLORIT ENSENYAT, PAU GIRÓN RODRÍGUEZ*/

#include "ficheros.h"

//permitir <nombre_dispositivo> <ninodo> <permisos>

/*---------------------------------------------------------------------------------------------------------
* Va a modificar los permisos
* Input:    argc: numero de argumentos que se han introducido
*           **argv: array de punteros a caracteres
* Output:   FALLO o EXITO
---------------------------------------------------------------------------------------------------------*/
int main(int argc, char **argv){

    //Si la sintaxis es incorrecta, va a saltar un mensaje de error
    if(argc != 4){
        fprintf(stderr,ROJO_T "Sintaxis: ./permitir <nombre_dispositivo> <ninodo> <permisos>\n" RESET);
        return FALLO;
    }
    
    char *directorio = argv[1];
    int ninodo = atoi(argv[2]);
    int permisos = atoi(argv[3]);

    //Montamos dispositivo virtual
    if(bmount(directorio) == FALLO){
        fprintf(stderr, ROJO_T "permisos: Error montar dispositivo" RESET);
        return FALLO;
    }

    //Modificamos permisos pasados por parametro al inodo pasado por parametro
    if(mi_chmod_f(ninodo, permisos) == FALLO){
        fprintf(stderr, ROJO_T "permisos: Error mi_chmod_f" RESET);
        return FALLO;
    }

    //Desmontamos dispositivo virtual
    if(bumount(directorio) == FALLO){
        fprintf(stderr, ROJO_T "permisos: Error desmontar dispositivo" RESET);
        return FALLO;
    }

    return EXITO;
}