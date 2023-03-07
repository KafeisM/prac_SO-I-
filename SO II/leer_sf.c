#include "ficheros_basico.h"
#include "bloques.h"

const char *directorio;
struct superbloque SB;
void *buffer[BLOCKSIZE];

int main(int argc, char **argv){

    directorio = argv[1];
    bmount(directorio);
    bread(posSB, buffer);
    memcpy(&SB,buffer,sizeof(struct superbloque));

    printf("DATOS DEL SUPERBLOQUE\n");

    printf("posPrimerBloqueMB = %d\n", SB.posPrimerBloqueMB);

    printf("posUltimoBloqueMB = %d\n", SB.posUltimoBloqueMB);

    printf("posPrimerBloqueAI = %d\n", SB.posPrimerBloqueAI);

    printf("posUltimoBloqueAI = %d\n", SB.posUltimoBloqueAI);

    printf("posPrimerBloqueDatos = %d\n", SB.posPrimerBloqueDatos);

    printf("posUltimoBloqueDatos = %d\n", SB.posUltimoBloqueDatos);

    printf("posInodoRaiz = %d\n", SB.posInodoRaiz);

    printf("posPrimerInodoLibre = %d\n", SB.posPrimerInodoLibre);

    printf("cantBloquesLibres = %d\n", SB.cantBloquesLibres);

    printf("cantInododsLibres = %d\n", SB.cantInodosLibres);

    printf("totBloques = %d\n", SB.totBloques);

    printf("totInodos = %d\n", SB.totInodos);

    printf("sizeof struct superbloque is: %lu\n", sizeof(struct superbloque));

    printf ("sizeof struct inodo is: %lu\n", sizeof(struct inodo));


    printf("\nRECORRIDO LISTA ENLAZADA DE INODOS LIBRES\n");
    //Podéis hacer también un recorrido de la lista de inodos libres (mostrando para cada inodo el campo punterosDirectos[0]).
    struct inodo inodos[BLOCKSIZE / INODOSIZE];
    int cont = 0;

    for (int i = SB.posPrimerBloqueAI; i <= SB.posUltimoBloqueAI; i++){
        //&inodos
        if (bread(i, inodos) == EXIT_FAILURE){
            return EXIT_FAILURE;
        }

        for (int j = 0; j < BLOCKSIZE / INODOSIZE; j++){

            if ((inodos[j].tipo == 'l')){
                cont++;
                if (cont < 20){
                    printf("%d ", cont);
                }
                else if (cont == 21){
                    printf("... ");
                }
                else if ((cont > 24990) && (cont < SB.totInodos)){
                    printf("%d ", cont);
                }
                else if (cont == SB.totInodos){
                    printf("-1 \n");
                }
                cont--;
            }
            cont++;
        }
    }

    return EXITO;
}