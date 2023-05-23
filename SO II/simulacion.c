/*JOSEP GABRIEL FORNÉS REYNÉS, JORDI FLORIT ENSENYAT, PAU GIRÓN RODRÍGUEZ*/

#include "simulacion.h"

int acabados = 0;

int main(int argc, char const **argv){

    signal(SIGCHLD, reaper);

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
        pid_t pid = fork();
        if (pid == 0){ //si es el hijo

            bmount(argv[1]);
            char directorio[100];
            sprintf(directorio, "%sproceso_PID%d/", buffer, getpid());

            if (mi_creat(directorio, 6) < 0){
                fprintf(stderr, ROJO_T"te la chingaste cabron creat 1\n"RESET);
                bumount();
                exit(0);
            }

            char fichero[120];
            sprintf(fichero, "%sprueba.dat", directorio);

            if (mi_creat(fichero, 2) < 0){
                fprintf(stderr, ROJO_T"te la chingaste cabron creat 2\n"RESET);
                bumount();
                exit(0);
            }

            srand(time(NULL) + getpid());

            for (int nescritura=1; nescritura <= NUMESCRITURAS; nescritura++){
                
                struct REGISTRO reg;
                reg.fecha = time(NULL);
                reg.pid = getpid();
                reg.nEscritura = nescritura;
                reg.nRegistro = rand() % REGMAX;

                if (mi_write(fichero, &reg, reg.nRegistro*sizeof(struct REGISTRO), sizeof(struct REGISTRO)) == 0){
                    fprintf(stderr, ROJO_T"te la chingaste cabron\n"RESET);
                }

                fprintf(stderr, "[simulacion.c -> Escritura %i en %s]\n", nescritura, fichero);
                usleep(50000);
            }

            fprintf(stderr, "Proceso %i: Completadas %i escrituras en %s\n", proceso, NUMESCRITURAS, fichero);

            bumount();
            exit(0);
        }

        usleep(150000);
        
    }

    

    while (acabados < NUMPROCESOS){
        pause();
    }

    if (bumount < 0){
        fprintf(stderr, "Error al desmontar el dispositivo\n");
        exit(0);
    }

}

void reaper(){
    pid_t ended;
    signal(SIGCHLD, reaper);
    fprintf(stderr, AZUL_T"acabados: %i\n"RESET, acabados);
    while ((ended = waitpid(-1, NULL, WNOHANG)) > 0){
        acabados++;
    }
}