/*JOSEP GABRIEL FORNÉS REYNÉS, JORDI FLORIT ENSENYAT, PAU GIRÓN RODRÍGUEZ*/
#include "directorios.h"

int main(int argc, char const *argv[]){

    if (argc != 3){
        fprintf(stderr, ROJO_T "Sintaxis: ./mi_stat <disco> </ruta>\n");
        return FALLO;
    }

    struct STAT p_stat;
    int p_inodo;
    
    if (bmount(argv[1]) == FALLO){
        fprintf(stderr, ROJO_T "Error en la creación del disco\n");
        return FALLO;
    }

    p_inodo = mi_stat(argv[2], &p_stat);

    if (p_inodo < 0){
        mostrar_error_buscar_entrada(p_inodo);
        return FALLO;
    }

    struct tm *tm;
    char atime[80];
    char mtime[80];
    char ctime[80];
    
    tm = localtime(&p_stat.atime);
    strftime(atime, sizeof(atime), "%a %Y-%m-%d %H:%M:%S", tm);
    tm = localtime(&p_stat.mtime);
    strftime(mtime, sizeof(mtime), "%a %Y-%m-%d %H:%M:%S", tm);
    tm = localtime(&p_stat.ctime);
    strftime(ctime, sizeof(ctime), "%a %Y-%m-%d %H:%M:%S", tm);

    printf(AZUL_T"Nº de inodo: %i\n", p_inodo);
    printf("tipo: %c\n", p_stat.tipo);
    printf("permisos: %d\n", p_stat.permisos);
    printf("atime: %s\n", atime);
    printf("ctime: %s\n", ctime);
    printf("mtime: %s\n", mtime);
    printf("nlinks: %d\n", p_stat.nlinks);
    printf("tamEnBytesLog: %d\n", p_stat.tamEnBytesLog);
    printf("numBloquesOcupados: %d\n", p_stat.numBloquesOcupados);

    bumount();

}