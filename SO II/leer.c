#include "ficheros.h"

// ./leer <nombre_dispositivo> <ninodo> 
int main(int argc, char **argv){
    
    if((argc != 3)){
        fprintf(stderr, "Sintaxis: ./leer <nombre_dispositivo> <ninodo>");
        return FALLO;
    }else{
        struct inodo inodo;

        char directorio = argv[1];
        int ninodo = atoi(argv[2]);
        int leidos = 0;
        int tamBuffer = 1500;
        unsigned int offset = 0;
        char buffer_texto[tamBuffer];

        if(bmount(directorio) == FALLO){
            fprintf(stderr, "Error en montar el dispositivo");
            return FALLO;
        }

        memset(buffer_texto, 0, tamBuffer);

        leidos = mi_read_f(ninodo, buffer_texto, offset, tamBuffer);

        while (leidos > 0){

            write(1, buffer_texto, leidos);

            memset(buffer_texto, 0, tamBuffer);

            offset += tamBuffer;

            leidos += mi_read_f(ninodo, buffer_texto, offset, tamBuffer);
        }

        if(leer_inodo(ninodo, &inodo)){
            fprintf(stderr, "Error en leer inodo");
            return FALLO;
        }

        if(inodo.tamEnBytesLog == leidos){
            char string[128];
            sprintf(string, "bytes leídos %d\n", leidos);
            write(2, string, strlen(string));
        }else{
            return FALLO;
        }

        if(bumount(directorio) == FALLO){
            fprintf(stderr, "Error en desmontar el dispositivo");
            return FALLO;
        }

        return EXITO;
    }

}