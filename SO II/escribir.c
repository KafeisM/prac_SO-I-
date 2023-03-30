#include "ficheros.h"

// ./escribir <nombre_dispositivo> <"$(cat fichero)"> <diferentes_inodos>
int main(int argc, char **argv){
    
    if((argc != 4)){
        printf(ROJO_T "Sintaxis: ./escribir <nombre_dispositivo> <'$(cat fichero)'> <diferentes_inodos>\n"RESET);
        printf(ROJO_T "Offsets: 9000, 209000, 30725000, 409605000, 480000000\n"RESET);
        printf(ROJO_T "Si diferentes_inodos = 0 se reserva un solo inodo para todos los offsets\n" RESET);

        return FALLO;
    }

    printf(ROJO_T "hola" RESET);

    //char *directorio = argv[1];
    int dif_inodos = atoi(argv[3]);
    int ninodo = reservar_inodo('f', 6);
    printf("%i", ninodo);
    int longitud = strlen(argv[2]);
    char *buf = malloc(longitud);
    int escritos;
    struct STAT p_stat;
      
    int offsets[5] = {9000, 209000, 30725000, 409605000, 480000000};

    if(ninodo == FALLO){
        fprintf(stderr, "Error en reservar inodo");
        return FALLO;
    }

    if(bmount(argv[1]) == FALLO){
        fprintf(stderr, "Error en montar el dispositivo");
        return FALLO;
    }

    for(int i = 0; i < 5; i++){
        fprintf(stderr, "aaaa");
        fprintf(stdout, "ninodo: %i", ninodo);
        fprintf(stdout, "offset: %i", offsets[i]);
        escritos = mi_write_f(ninodo, buf, offsets[i], longitud);
        if(escritos == FALLO){
            return FALLO;
        }
        fprintf(stdout, "escritos: %i", escritos);

        if(mi_stat_f(ninodo, &p_stat) == FALLO){
            return FALLO;
        }else{
            fprintf(stdout, "Tamaño en bytes lógico del inodo: %i\n", p_stat.tamEnBytesLog);
            fprintf(stdout, "Nº de bloques ocupados: %i\n", p_stat.numBloquesOcupados);
        }

        //Si <diferentes_inodods> = 1, hay que reservar inodos para cada offset
        if(dif_inodos == 1){
            ninodo = reservar_inodo('f', 6);
            if(ninodo == FALLO){
                return FALLO;
            }
        }

    }

    if(bumount(argv[1]) == FALLO){
        fprintf(stderr, "Error en desmontar el dispositivo");
        return FALLO;
    }

    return EXITO;

}