/*JOSEP GABRIEL FORNÉS REYNÉS, JORDI FLORIT ENSENYAT, PAU GIRÓN RODRÍGUEZ*/

#include "ficheros.h"

// ./escribir <nombre_dispositivo> <"$(cat fichero)"> <diferentes_inodos>
int main(int argc, char **argv){

    char *directorio = argv[1];
    int dif_inodos = atoi(argv[3]);
    int escritos;
    struct STAT p_stat;
      
    int offsets[5] = {9000, 209000, 30725000, 409605000, 480000000};
    int longitidOffsets = sizeof(offsets)/sizeof(int);
    
    if((argc != 4)){
        fprintf(stderr,ROJO_T "Sintaxis: ./escribir <nombre_dispositivo> <'$(cat fichero)'> <diferentes_inodos>\n"RESET);
        fprintf(stderr,ROJO_T "Offsets: 9000, 209000, 30725000, 409605000, 480000000\n"RESET);
        fprintf(stderr,ROJO_T "Si diferentes_inodos = 0 se reserva un solo inodo para todos los offsets\n" RESET);

        return FALLO;
    }

    printf("Longitud texto: %ld\n\n", strlen(argv[2]));   

    if(bmount(directorio) == FALLO){
        fprintf(stderr, "Error en montar el dispositivo");
        return FALLO;
    }

    unsigned int ninodo = reservar_inodo('f', 6);
    if(ninodo == FALLO){
        fprintf(stderr, "Error en reservar inodo");
        return FALLO;
    }

    for(int i = 0; i < longitidOffsets; i++){
        fprintf(stdout, "Nº inodo reservado: %i\n", ninodo);
        fprintf(stdout, "Offset: %i\n", offsets[i]);

        escritos = mi_write_f(ninodo, argv[2], offsets[i], strlen(argv[2]));
        
        if(escritos == FALLO){
            return FALLO;
        }

        fprintf(stdout,RESET"Bytes escritos: %i\n", escritos);


        if(mi_stat_f(ninodo, &p_stat) == FALLO){
            return FALLO;
        }else{
            printf("stat.tamEnBytesLog: %i\n", p_stat.tamEnBytesLog);
            printf("stat.numBloquesOcupados: %i\n\n", p_stat.numBloquesOcupados);
        }

        //Si <diferentes_inodods> = 1, hay que reservar inodos para cada offset
        if(dif_inodos == 1){
            ninodo = reservar_inodo('f', 6);
            if(ninodo == FALLO){
                return FALLO;
            }
        }

    }

    if(bumount() == FALLO){
        fprintf(stderr, "Error en desmontar el dispositivo");
        return FALLO;
    }

    return EXITO;

}