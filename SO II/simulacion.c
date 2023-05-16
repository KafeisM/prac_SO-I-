/*JOSEP GABRIEL FORNÉS REYNÉS, JORDI FLORIT ENSENYAT, PAU GIRÓN RODRÍGUEZ*/

#include "simulacion.h"

int acabados = 0;

int main(int argc, char const **argv){

    //control de sintaxis
    if (argc != 2){
        fprintf(stderr, "Error de sintaxis: ./simulacion <disco>\n");
        return FALLO;
    }

    //montamos el dispositivo padre
    if (bmount(argv[1]) == FALLO){
        return FALLO;
    }

    //crear el directorio de simulacion: /simul_aaaammddhhmmss/
    time_t mi_time = time(NULL);
    struct tm tm = *localtime(&mi_time);
    char *tiempo = malloc(14);
    char *camino = malloc(22);
    char buffer[80];

    sprintf(tiempo, "%d%02d%02d%02d%02d%02d", tm.tm_year + 1900, tm.tm_mon + 1,
    tm.tm_mday, tm.tm_hour, tm.tm_min, tm.tm_sec);

    strcpy(camino, "/simul_");
    strcat(camino, tiempo);
    strcat(camino,"/");
    strcpy(buffer, camino);

    if (mi_creat(camino,6) < 0){
        return FALLO;
    }

    for (int proceso=1; proceso <= NUMPROCESOS; proceso++){
        
    }

}