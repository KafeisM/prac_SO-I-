#include <stdio.h>  //printf(), fprintf(), stderr, stdout, stdin
#include <fcntl.h> //O_WRONLY, O_CREAT, O_TRUNC
#include <sys/stat.h> //S_IRUSR, S_IWUSR
#include <stdlib.h>  //exit(), EXIT_SUCCESS, EXIT_FAILURE, atoi()
#include <unistd.h> // SEEK_SET, read(), write(), open(), close(), lseek()
#include <errno.h>  //errno
#include <string.h> // strerror()
#include "bloques.h"

//colores para errores
#define RESET "\033[0m"
#define ROJO_T "\x1b[31m"
#define NEGRITA "\x1b[1m"

static int descriptor = 0;

int main(int argc, char **argv){


    if(argc -1 != 2){
        fprintf(stderr,ROJO_T"Sintaxis correcta: ./mi_mkfs <nombre_dispositivo> <nbloques>\n"RESET);
        exit(0);
    }

    int num_blocks = atoi(argv[2]);
    unsigned char buffer[1024];
    memset(buffer, 0, sizeof(buffer));

    // MONTAR EL DISPOSITIVO VIRTUAL
    descriptor = bmount(argv[1]);
    // INICIAR FICHERO
    for (size_t i = 0; i < num_blocks; i++){
        if(bwrite(i,&buffer) == -1){
            fprintf(stderr,ROJO_T"ERROR INICIALIZACIÓN DE FICHERO\n"RESET);
        }
    }
    //DESMONTAR FICHERO
    if(bumount() == -1){
        fprintf(stderr,ROJO_T"ERROR INICIALIZACIÓN DE FICHERO\n"RESET);
    }
        
    return 0;
}