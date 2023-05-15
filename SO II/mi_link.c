/*JOSEP GABRIEL FORNÉS REYNÉS, JORDI FLORIT ENSENYAT, PAU GIRÓN RODRÍGUEZ*/
#include "directorios.h"

/*---------------------------------------------------------------------------------------------------------
* Crea un enlace a un fichero mediante la funcion mi_link de la capa de directorios. Controlamos los casos
* de errores y el fallo en la sintaxis.
---------------------------------------------------------------------------------------------------------*/

int main(int argc, char const *argv[]){

    //control de sintaxis
    if (argc != 4){
        fprintf(stderr, ROJO_T "Sintaxis: ./mi_link <disco> </ruta_fichero_original> </ruta_enlace>\n"RESET);
        return FALLO;
    }

    //miramos que no haya directorios
    if (argv[2][strlen(argv[2])-1] == '/' && argv[3][strlen(argv[3])-1] == '/'){
        fprintf(stderr, ROJO_T"Solo se permiten ficheros"RESET);
        return FALLO;
    }

    //montamos el dispositivo
    bmount(argv[1]);

    //creamos el enlace pasandole ambos caminos
    int error = mi_link(argv[2],argv[3]);

    //controlamos los posibles errores
    if (error<0){
        mostrar_error_buscar_entrada(error);
        return FALLO;
    }

    //desmontamos el dispositivo
    bumount(argv[1]);
    
}