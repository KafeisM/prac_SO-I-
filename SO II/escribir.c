/*JOSEP GABRIEL FORNÉS REYNÉS, JORDI FLORIT ENSENYAT, PAU GIRÓN RODRÍGUEZ*/

#include "ficheros.h"

// ./escribir <nombre_dispositivo> <"$(cat fichero)"> <diferentes_inodos>

/*---------------------------------------------------------------------------------------------------------
* Escribirá texto en uno o varios inodos
* Input:    argc: numero de argumentos que se han introducido
*           **argv: array de punteros a caracteres
* Output:   FALLO o EXITO
---------------------------------------------------------------------------------------------------------*/
int main(int argc, char **argv){

    char *directorio = argv[1];
    int dif_inodos = atoi(argv[3]);
    int escritos;
    struct STAT p_stat;
      
    int offsets[5] = {9000, 209000, 30725000, 409605000, 480000000};
    int longitidOffsets = sizeof(offsets)/sizeof(int);
    
    //Si la sintaxis es incorrecta, va a saltar un mensaje de error
    if((argc != 4)){
        fprintf(stderr,ROJO_T "Sintaxis: ./escribir <nombre_dispositivo> <'$(cat fichero)'> <diferentes_inodos>\n"RESET);
        fprintf(stderr,ROJO_T "Offsets: 9000, 209000, 30725000, 409605000, 480000000\n"RESET);
        fprintf(stderr,ROJO_T "Si diferentes_inodos = 0 se reserva un solo inodo para todos los offsets\n" RESET);

        return FALLO;
    }

    //Obtenemos la longitud del fichero a través del strlen
    printf("Longitud texto: %ld\n\n", strlen(argv[2]));   

    //Montamos dispositivo virtual
    if(bmount(directorio) == FALLO){
        fprintf(stderr,ROJO_T "escribir.c: Error en bmount\n" RESET);
        return FALLO;
    }

    //Reservamos inodo
    unsigned int ninodo = reservar_inodo('f', 6);
    if(ninodo == FALLO){
        fprintf(stderr, ROJO_T "escribir.c: Error en reservar_inodo\n"RESET);
        return FALLO;
    }

    //For para poder escribir en todos los offsets
    for(int i = 0; i < longitidOffsets; i++){
        fprintf(stdout, "Nº inodo reservado: %i\n", ninodo);
        fprintf(stdout, "Offset: %i\n", offsets[i]);

        //Escribimos en el fichero
        escritos = mi_write_f(ninodo, argv[2], offsets[i], strlen(argv[2]));
        
        if(escritos == FALLO){
            fprintf(stderr,ROJO_T "escribir.c: Error en mi_write_f\n" RESET);
            return FALLO;
        }

        fprintf(stdout,RESET"Bytes escritos: %i\n", escritos);

        //Obtenemos los datos
        if(mi_stat_f(ninodo, &p_stat) == FALLO){
            fprintf(stderr,ROJO_T "escribir.c: Error en mi_stat_f\n" RESET);
            return FALLO;
        }else{
            //Imprimimos por pantalla tamaño en bytes lógico del inodo y nº de bloques ocupados
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

    //Desmontamos dispositivo virtual
    if(bumount() == FALLO){
        fprintf(stderr,ROJO_T "escribir.c: Error en bumount\n" RESET);
        return FALLO;
    }

    return EXITO;

}