#include "ficheros_basico.h"

int main(){

    struct superbloque SB;

    bread(posSB, &SB);

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

    printf("sizeof struct superbloque is: %lu\n", sizeof(struct superbloque));

    printf ("sizeof struct inodo is: %lu\n", sizeof(struct inodo));

    return 0;
}

/*void imprimirInodosLibres(){

    struct superbloque SB;
    bread(posSB,&SB);
    struct inodo inodos[BLOCKSIZE/INODOSIZE];

    for(int i = SB.posPrimerBloqueAI; i < SB.posUltimoBloqueAI; i++){
        bread(i, &inodos);
        for(int j = 0; j < (BLOCKSIZE / INODOSIZE); j++){
            if(inodos[i] == 'l'){

            }
        }
    }
}*/