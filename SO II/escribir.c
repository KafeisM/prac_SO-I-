#include "ficheros.h"

// ./escribir <nombre_dispositivo> <"$(cat fichero)"> <diferentes_inodos>
int main(int argc, char **argv){
    
    if((argc != 4)){
        fprintf(stderr, "Sintaxis: ./escribir <nombre_dispositivo> <'$(cat fichero)'> <diferentes_inodos>");
        fprintf(stderr, "Offsets: 9000, 209000, 30725000, 409605000, 480000000");
        fprintf(stderr, "Si diferentes_inodos = 0 se reserva un solo inodo para todos los offsets");

        return FALLO;
    }
    struct inodo inodo;

    char directorio = argv[1];
    int dif_inodos = atoi(argv[3]);
    int ninodo = reservar_inodo('f', 6);
    int longitud = strlen(argv[2]);
    char *buf = malloc(longitud);
    int escritos;
    struct STAT p_stat;
      
    int offsets[5] = {9000, 209000, 30725000, 409605000, 480000000};
    int longitud_offsets = sizeof(offsets) / sizeof(int);

    if(ninodo == FALLO){
        fprintf(stderr, "Error en reservar inodo");
        return FALLO;
    }

    if(bmount(directorio) == FALLO){
        fprintf(stderr, "Error en montar el dispositivo");
        return FALLO;
    }

    for(int i = 0; i < longitud_offsets; i++){
        printf("ninodo: %i", ninodo);
        printf("offset: %i", offsets[i]);
        escritos = mi_write_f(ninodo, buf, offsets[i], longitud);
        if(escritos == FALLO){
            return FALLO;
        }
        printf("escritos: %i", escritos);

        if(mi_stat_f(ninodo, &p_stat) == FALLO){
            return FALLO;
        }else{
            printf("Tamaño en bytes lógico del inodo: %i\n", p_stat.tamEnBytesLog);
            printf("Nº de bloques ocupados: %i\n", p_stat.numBloquesOcupados);
        }

        //Si <diferentes_inodods> = 1, hay que reservar inodos para cada offset
        if(dif_inodos == 1){
            ninodo = reservar_inodo('f', 6);
            if(ninodo == FALLO){
                return FALLO;
            }
        }

    }

    if(bumount(directorio) == FALLO){
        fprintf(stderr, "Error en desmontar el dispositivo");
        return FALLO;
    }

    return EXITO;

}