#include <stdio.h>  //printf(), fprintf(), stderr, stdout, stdin
#include <fcntl.h> //O_WRONLY, O_CREAT, O_TRUNC
#include <sys/stat.h> //S_IRUSR, S_IWUSR
#include <stdlib.h>  //exit(), EXIT_SUCCESS, EXIT_FAILURE, atoi()
#include <unistd.h> // SEEK_SET, read(), write(), open(), close(), lseek()
#include <errno.h>  //errno
#include <string.h> // strerror()
#include "bloques.h"

static int descriptor = 0;


//colores para errores
#define RESET "\033[0m"
#define NEGRO_T "\x1b[30m"
#define NEGRO_F "\x1b[40m"
#define GRIS_T "\x1b[94m"
#define ROJO_T "\x1b[31m"
#define VERDE_T "\x1b[32m"
#define AMARILLO_T "\x1b[33m"
#define AZUL_T "\x1b[34m"
#define MAGENTA_T "\x1b[35m"
#define CYAN_T "\x1b[36m"
#define BLANCO_T "\x1b[97m"
#define NEGRITA "\x1b[1m"

int main(int argc, char **argv){
    int num_blocks = atoi(argv[2]);
    printf("%d\n",num_blocks);

    if(argc != 3){
        fprintf(stderr,"\nSintaxis correcta: ./mi_mkfs <nombre_dispositivo> <nbloques>");
    }else{
        // MONTAR EL DISPOSITIVO VIRTUAL
       // descriptor = bmount(argv[1]);
        // INICIAR FICHERO
        
        
        
        
    }
    return 0;
}