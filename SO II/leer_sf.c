#include "ficheros_basico.h"

void mostrarSB(){

    struct superbloque SB;

    printf("DATOS DEL SUPERBLOQUE");

    printf("posPrimerBloqueMB = %d", SB.posPrimerBloqueMB);

    printf("posUltimoBloqueMB = %d", SB.posUltimoBloqueMB);

    printf("posPrimerBloqueAI = %d", SB.posPrimerBloqueAI);

    printf("posUltimoBloqueAI = %d", SB.posUltimoBloqueAI);

    printf("posPrimerBloqueDatos = %d", SB.posPrimerBloqueDatos);

    printf("posUltimoBloqueDatos = %d", SB.posUltimoBloqueDatos);

    printf("posInodoRaiz = %d", SB.posInodoRaiz);

    printf("posPrimerInodoLibre = %d", SB.posPrimerInodoLibre);

    printf("cantBloquesLibres = %d", SB.cantBloquesLibres);

    printf("cantInododsLibres = %d", SB.cantInodosLibres);

    printf("totBloques = %d", SB.totBloques);

    printf("totInodos = %d", SB.totInodos);

    printf("sizeof struct inodo is: %lu\n", sizeof(struct superbloque));

}

void mostrarStructInodo(){

    printf ("sizeof struct inodo is: %lu\n", sizeof(struct inodo));

}