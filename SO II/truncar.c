/*JOSEP GABRIEL FORNÉS REYNÉS, JORDI FLORIT ENSENYAT, PAU GIRÓN RODRÍGUEZ*/

#include "ficheros.h"

/*---------------------------------------------------------------------------------------------------------
* Programa para comprobar que funciona el borrado total o parcial del contenido de un disp. virtual
* Input:    argc: numero de argumentos que se han introducido
*           **argv: array de punteros a caracteres
* Output:   FALLO o EXITO
---------------------------------------------------------------------------------------------------------*/

int main(int argc, char **argv){

    //validación de sintaxis
    if(argv[1] == NULL || argv[2] == NULL || argv[3] == NULL){
        fprintf(stderr, "Error. Sintaxis: truncar <nombre_dispositivo> <ninodo> <nbytes>\n");
        return FALLO;
    }

    //guardamos en variables los argumentos
    const char *dir = argv[1];
    int ninodo = atoi(argv[2]);
    int nbytes = atoi(argv[3]);

    //montamos el dispositivo virtual
    if (bmount(dir) == FALLO){
        fprintf(stderr, ROJO_T"truncar.c: Error bmount\n"RESET);
        return FALLO;
    }

    // Si nbytes = 0 liberamos el inodo y sino truncamos el archivo
    if (nbytes == 0){
        if (liberar_inodo(ninodo) == FALLO){
            fprintf(stderr, ROJO_T"truncar.c: Error liberar_inodo\n"RESET);
            return FALLO;
        }
    }else{
        mi_truncar_f(ninodo, nbytes);
    }

    //datos del inodo liberado
    struct STAT p_stat;
    if (mi_stat_f(ninodo, &p_stat) == FALLO){
        fprintf(stderr, ROJO_T"truncar.c: Error mi_stat_f\n"RESET);
        return FALLO;
    }

    //variables usadas para cambiar los datos de la fecha y hora
    struct tm *tm;
    char atime[100];
    char mtime[100];
    char ctime[100];
    tm = localtime(&p_stat.atime);
    strftime(atime, sizeof(atime), "%a %Y.%m-%d %H:%M:%S", tm);
    tm = localtime(&p_stat.mtime);
    strftime(mtime, sizeof(mtime), "%a %Y.%m-%d %H:%M:%S", tm);
    tm = localtime(&p_stat.ctime);
    strftime(ctime, sizeof(ctime), "%a %Y.%m-%d %H:%M:%S", tm);

    //mostramos por pantalla la información del inodo escrito
    printf("DATOS INODO %d:\n",ninodo);
    printf("tipo=%c\n", p_stat.tipo);
    printf("permisos=%d\n", p_stat.permisos);
    printf("atime=%s\n", atime);
    printf("ctime=%s\n", ctime);
    printf("mtime=%s\n", mtime);
    printf("nLinks=%d\n",p_stat.nlinks);
    printf("tamEnBytesLog=%d\n", p_stat.tamEnBytesLog);
    printf("numBloquesOcupados=%d\n", p_stat.numBloquesOcupados);

    //liberamos
    if (bumount() == FALLO){
        fprintf(stderr,ROJO_T"truncar.c: Error bumount\n"RESET);
        return FALLO;
    }

    return EXITO;

}