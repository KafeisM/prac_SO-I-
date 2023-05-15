/*JOSEP GABRIEL FORNÉS REYNÉS, JORDI FLORIT ENSENYAT, PAU GIRÓN RODRÍGUEZ*/
#include "directorios.h"

int main(int argc, char const *argv[]){
    
    //comprobamos la sintaxis
    if (argc != 3){
        fprintf(stderr, ROJO_T "Sintaxis: ./mi_rm disco /ruta\n"RESET);
        return FALLO;
    }

    //montamos el dispositivo
    if(bmount(argv[1]) == FALLO){
        fprintf(stderr,ROJO_T "Error en bmount\n" RESET);
        return FALLO;
    }   
    
    //borra un fichero o directorio, llamando a la función mi_unlink()  
    if(mi_unlink(argv[2]) < 0){
        return FALLO;
    }

    //desmontamos el dispositivo
    bumount();

    return EXITO;
}
