#include "directorios.h"

//Sintaxis: ./mi_ls <disco> </ruta_directorio>

int main(int argc, char **argv){
    //lista el contenido de un directorio

    int longitud = strlen(argv[2]);
    unsigned char directorio = argv[1];
    char buffer[TAMBUFFER];
    int total;

    if(argc != 3){
        fprintf(stderr, ROJO_T "Sintaxis: ./mi_ls <disco> </ruta_directorio>\n");
        return FALLO;
    }

    if(bmount(directorio) == FALLO){
        return FALLO;
    }


    memset(buffer, 0, TAMBUFFER);

    total = mi_dir(argv[2], buffer);

    if(total < 0){
        return FALLO;
    }else{
        printf("Total: %d\n", total);

        printf("Tipo\tPermisos\tmTime\t\t\tTamaño\tNombre\n");
        printf("--------------------------------------------------------------------------------\n");
        printf("%s\n", buffer);
    }

    bumount();

    return EXITO;
}