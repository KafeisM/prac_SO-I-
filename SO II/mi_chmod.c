/*JOSEP GABRIEL FORNÉS REYNÉS, JORDI FLORIT ENSENYAT, PAU GIRÓN RODRÍGUEZ*/


#include "directorios.h"

//Sintaxis: ./mi_chmod <disco> <permisos> </ruta>
int main(int argc, char const *argv[]){

    unsigned char permisos = atoi(argv[2]);
    //comprobamos la sintaxis correcta
    if(argc != 4){
        fprintf(stderr, ROJO_T "Sintaxis: ./mi_chmod <disco> <permisos> </ruta>\n");
        return FALLO;
    }

    //miramos si el valor de los permisos es correcto
    if(atoi(argv[2]) < 0 || atoi(argv[2]) > 7){
        fprintf(stderr, ROJO_T "Permisos incorrectos(0-7)\n");
        return FALLO;
    }

    //montamos el dispositivo
    if(bmount(argv[1]) == FALLO){
        fprintf(stderr,ROJO_T "Error en bmount\n" RESET);
        return FALLO;
    }   

    //llamamos a chmod con el camino indicado y los permisos correspondientes
    int error = mi_chmod(argv[3],permisos);
    if(error < 0){
        mostrar_error_buscar_entrada(error);
        return FALLO;
    }

    //desmontamos el dispositivo
    bumount();
    return EXITO;
}
