/*JOSEP GABRIEL FORNÉS REYNÉS, JORDI FLORIT ENSENYAT, PAU GIRÓN RODRÍGUEZ*/

#include "ficheros_basico.h"
#include "bloques.h"

#define DEBUGSB 1
#define DEBUG1  0
#define DEBUG2  0
#define DEBUG3  0
#define DEBUG4  0
#define DEBUG5  0


void comprobarMB();
void comprobarBloques();
void comprobarInodo(int inRes, struct inodo inodo);

const char *directorio;
struct superbloque SB;
void *buffer[BLOCKSIZE];

int main(int argc, char **argv){

    directorio = argv[1];
    if (bmount(directorio) == FALLO){
        return FALLO;
    }
    if (bread(posSB, &SB) == FALLO){
        return FALLO;
    }

#if DEBUGSB
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

    //printf("sizeof struct superbloque is: %lu\n", sizeof(struct superbloque));

    //printf ("sizeof struct inodo is: %lu\n", sizeof(struct inodo));
#endif

#if DEBUG1
    printf("\nRECORRIDO LISTA ENLAZADA DE INODOS LIBRES\n");
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
#endif

#if DEBUG2
    //comprobar reservar/liberar bloque
    printf("\nRESERVAMOS UN BLOQUE Y LUEGO LO LIBERAMOS\n");
    comprobarBloques();
#endif

#if DEBUG3
    //comprobar leer/escribir bit
    printf("\nMAPA DE BITS CON BLOQUES DE METADATOS OCUPADOS\n");
    comprobarMB();
#endif

#if DEBUG4
    //comrobar datos del directorio raiz
    printf("\nDATOS DEL DIRECTORIO RAIZ\n");
    comprobarInodo(SB.posInodoRaiz); //directorio raiz es el inodo 0
#endif

#if DEBUG5
    int ninodo = reservar_inodo('f',6);

    if (bread(posSB,&SB) == FALLO){
        return FALLO;
    }

    struct inodo inodoRes;
    
    printf("\nINODO %d. TRADUCCIÓN DE LOS BLOQUES LOGICOS 8,204,30.004,400.004,468.750\n",ninodo);
    leer_inodo(ninodo,&inodoRes);
    traducir_bloque_inodo(&inodoRes,8,1);
    printf("\n");
    traducir_bloque_inodo(&inodoRes,204,1);
    printf("\n");
    traducir_bloque_inodo(&inodoRes,30004,1);
    printf("\n");
    traducir_bloque_inodo(&inodoRes,400004,1);
    printf("\n");
    traducir_bloque_inodo(&inodoRes,468750,1);

    printf("\nDATOS DEL INODO RESERVADO %d\n",ninodo);
    comprobarInodo(ninodo,inodoRes);

    bread(posSB,&SB);
    printf("posPrimerInodoLibre = %d\n", SB.posPrimerInodoLibre);


#endif

    return EXITO;
}



void comprobarMB(){
    printf("posSB: %d -> leer_bit(%d) = %d\n",0,0,leer_bit(0));
    printf("SB.posPrimerBloqueMB: %d -> leer_bit(%d) = %d\n",SB.posPrimerBloqueMB,SB.posPrimerBloqueMB,leer_bit(SB.posPrimerBloqueMB));
    printf("SB.posUltimoBloqueMB: %d -> leer_bit(%d) = %d\n",SB.posUltimoBloqueMB,SB.posUltimoBloqueMB,leer_bit(SB.posUltimoBloqueMB));
    printf("SB.posPrimerBloqueAI: %d -> leer_bit(%d) = %d\n",SB.posPrimerBloqueAI,SB.posPrimerBloqueAI,leer_bit(SB.posPrimerBloqueAI));
    printf("SB.posUltimoBloqueAI: %d -> leer_bit(%d) = %d\n",SB.posUltimoBloqueAI,SB.posUltimoBloqueAI,leer_bit(SB.posUltimoBloqueAI));
    printf("SB.posPrimerBloqueDatos: %d -> leer_bit(%d) = %d\n",SB.posPrimerBloqueDatos,SB.posPrimerBloqueDatos,leer_bit(SB.posPrimerBloqueDatos));
    printf("SB.posUltimoBloqueDatos: %d -> leer_bit(%d) = %d\n",SB.posUltimoBloqueDatos,SB.posUltimoBloqueDatos,leer_bit(SB.posUltimoBloqueDatos));
    
}

void comprobarBloques(){
    printf("Se ha reservado el bloque físico no %d que era el 1er libre indicado por el MB\n",SB.posPrimerBloqueDatos);

    //reservamos el primer bloque de datos
    if(reservar_bloque() == FALLO){
        fprintf(stderr,ROJO_T"FALLO AL RESERVAR BLOQUE\n"RESET);
    }

    bread(posSB,&SB);
    printf("SB.cantBloquesLibres = %d\n",SB.cantBloquesLibres);

    //liberamos ese mismo bloque
    if(liberar_bloque(SB.posPrimerBloqueDatos) == FALLO){
        fprintf(stderr,ROJO_T"FALLO AL LIBERAR BLOQUE\n");
    }

    bread(posSB,&SB);
    printf("Liberamos ese bloque y después SB.cantBloquesLibres = %d\n",SB.cantBloquesLibres);

}

void comprobarInodo(int inRes, struct inodo inodo){
    
    struct tm *ts;
    char atime[80];
    char mtime[80];
    char ctime[80];

    ts = localtime(&inodo.atime);
    strftime(atime, sizeof(atime), "%a %Y-%m-%d %H:%M:%S", ts);
    ts = localtime(&inodo.mtime);
    strftime(mtime, sizeof(mtime), "%a %Y-%m-%d %H:%M:%S", ts);
    ts = localtime(&inodo.ctime);
    strftime(ctime, sizeof(ctime), "%a %Y-%m-%d %H:%M:%S", ts);
    
    printf("ID: %d \nATIME: %s \nMTIME: %s \nCTIME: %s\n",inRes,atime,mtime,ctime);
    printf("tipo: %c\n",inodo.tipo);
    printf("permisos: %d\n",inodo.permisos);
    printf("nlinks: %d\n",inodo.nlinks);
    printf("tamEnBytesLog: %d\n",inodo.tamEnBytesLog);
    printf("numBloquesOcupados: %d\n",inodo.numBloquesOcupados);
}