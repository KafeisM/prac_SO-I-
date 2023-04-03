#include "ficheros.h"

// ./leer <nombre_dispositivo> <ninodo> 
int main(int argc, char **argv){
    
    if((argc != 3)){
        fprintf(stderr, "Sintaxis: ./leer <nombre_dispositivo> <ninodo>");
        return FALLO;
    }else{
        struct inodo inodo;

        int ninodo = atoi(argv[2]);

        int tamBuffer = 1500;
        unsigned int offset = 0;
        char buffer_texto[tamBuffer];

        memset(buffer_texto,0,tamBuffer);

        if(bmount(argv[1]) == FALLO){
            fprintf(stderr, "Error en montar el dispositivo");
            return FALLO;
        }

        int leidos = mi_read_f(ninodo, buffer_texto, offset, tamBuffer);

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

        fprintf(stderr, "total_bytesleidos: %d\ntamEnBytesLog: %d\n", leidos, inodo.tamEnBytesLog);

        if(bumount() == FALLO){
            fprintf(stderr, "Error en desmontar el dispositivo");
            return FALLO;
        }

        return EXITO;
    }

}