#include "directorios.h"

/*---------------------------------------------------------------------------------------------------------
* Programa (comando) que crea un fichero llamando a la función mi_creat()
* Input:    argc: numero de argumentos que se han introducido
*           **argv: array de punteros a caracteres
* Output:   FALLO o EXITO
---------------------------------------------------------------------------------------------------------*/

//Sintaxis: ./mi_touch <disco> <permisos> </ruta>

int main(int argc, char **argv){

    unsigned char permisos = atoi(argv[2]);
    int longitud = strlen(argv[3]);

    //comprobamos sintaxis
    if(argc != 4){
        fprintf(stderr, ROJO_T "Sintaxis: ./mi_touch <disco> <permisos> </ruta>\n");
        return FALLO;
    }

    //comprobamos permisos
    if( atoi(argv[2]) < 0 || atoi(argv[2]) > 7){
        fprintf(stderr, ROJO_T "Permisos incorrectos\n");
        return FALLO;
    }

    //Si no es un fichero
    if(argv[3][longitud - 1] == '/'){
        fprintf(stderr, ROJO_T "No es una ruta de fichero válida\n");

    }else{//Si lo es
        
        //Montamos dispositivo
        if(bmount(argv[1]) == FALLO){
            return FALLO;
        }

        //creamos el directorio y miramos si hay error 
        int error = mi_creat(argv[3], permisos);
        if(error < 0){
            mostrar_error_buscar_entrada(error);
            return FALLO;
        }

        //desmontamos dispositivo
        if(bumount() == FALLO){
            return FALLO;
        }
        
    }

    return EXITO;
}