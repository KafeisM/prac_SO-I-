/*JOSEP GABRIEL FORNÉS REYNÉS, JORDI FLORIT ENSENYAT, PAU GIRÓN RODRÍGUEZ*/

#include "directorios.h"

/*---------------------------------------------------------------------------------------------------------
* Dada una cadena de (camino que empieze por '/') separa su contenido
* Input:    camino: direccion del fichero o directorio
*           inicial: directorio/fichero mas proximo en el que estamos
*           final: camino restante
*           tipo: f -> fichero | d -> directorio
* Output:   FALLO o EXITO
---------------------------------------------------------------------------------------------------------*/

int extraer_camino(const char *camino, char *inicial, char *final, char *tipo){
    bool dir = false;

    if (camino[0] != '/'){
        return FALLO;
    }

    // buscamos la siguiente '/'
    for (int i = 1; i < strlen(camino); i++){
        if (camino[i] == '/'){
            dir = true;
            strncpy(inicial, camino + 1, sizeof(char) * i - 1); // guardar la porcion entre los 2 primeros '/'
            strcpy(final, camino + 1);                          // guardar el resto del camino a partir del segundo '/'
            strcpy(tipo, "d");
            break;
        }
    }

    if (!dir){ // miramos si no es un directorio
        strncpy(inicial, camino + 1, sizeof(char) * strlen(camino) - 1);
        strcpy(tipo, "f");
        // en caso de fichero no guardamos nada en final
    }

    return EXITO;
}

int buscar_entrada(const char *camino_parcial, unsigned int *p_inodo, unsigned int *p_inodo_dir, unsigned int *p_entrada,
char reservar, unsigned char permisos){

    //variables
    struct entrada entrada;
    struct inodo inodo_dir;
    char inicial[sizeof(entrada.nombre)];
    memset(inicial, 0, sizeof(entrada.nombre));
    char final[strlen(camino_parcial)];
    memset(final, 0, strlen(camino_parcial));
    char tipo;
    int cant_entradas_inodo, num_entrada_inodo;

    if (strcmp(camino_parcial, "/") == 0){
        struct superbloque SB;
        if (bread(posSB, &SB) == FALLO){
            fprintf(stderr, ROJO_T"buscar_entrada(): Error bread SB\n"RESET);
            return FALLO;
        }
        *p_inodo = SB.posInodoRaiz;
        *p_entrada = 0;
        return EXITO;
    }

    fprintf(stderr, ROJO_T"Llego buscar entrada"RESET); 

    if (extraer_camino(camino_parcial, inicial, final, &tipo) == FALLO){
        fprintf(stderr, ROJO_T"buscar_entrada(): Error camino incorrecto\n"RESET);
        return ERROR_CAMINO_INCORRECTO;
    }

    printf("[buscar_entrada()->inicial: %s, final: %s, reservar: %d]\n", inicial, final, reservar);

    if (leer_inodo(*p_inodo_dir, &inodo_dir) == FALLO){
        return ERROR_PERMISO_LECTURA;
    }

    //buffer de lectura
    struct entrada bufferLectura[BLOCKSIZE/sizeof(struct entrada)];
    memset(bufferLectura, 0, (BLOCKSIZE/sizeof(struct entrada)*sizeof(struct entrada)));

    //cantidad de entradas del inodo y nº de entrada inicial
    cant_entradas_inodo = inodo_dir.tamEnBytesLog/sizeof(struct entrada);
    num_entrada_inodo = 0;

    if (cant_entradas_inodo > 0){

        if (mi_read_f(*p_inodo_dir, &entrada, num_entrada_inodo*sizeof(struct entrada), sizeof(struct entrada)) < 0){
            return ERROR_PERMISO_LECTURA;
        }

        while(num_entrada_inodo < cant_entradas_inodo && strcmp(inicial, entrada.nombre) != 0){
            num_entrada_inodo++;
            if (mi_read_f(*p_inodo_dir, &entrada, num_entrada_inodo*sizeof(struct entrada), sizeof(struct entrada)) < 0){
                return ERROR_PERMISO_LECTURA;
            }
        }

    }

    if ((inicial != bufferLectura[num_entrada_inodo%(BLOCKSIZE/sizeof(struct entrada))].nombre) && (num_entrada_inodo == cant_entradas_inodo)){

        switch (reservar){
        case 0:
            return ERROR_NO_EXISTE_ENTRADA_CONSULTA;
            break;
        case 1:
            if (inodo_dir.tipo == 'f'){
                return ERROR_NO_SE_PUEDE_CREAR_ENTRADA_EN_UN_FICHERO;
            }

            if ((inodo_dir.permisos & 2) != 2){
                return ERROR_PERMISO_ESCRITURA;
            }else{
                strcpy(entrada.nombre, inicial);
                if (tipo == 'd'){
                    if (strcmp(final, "/") == EXITO){
                        entrada.ninodo = reservar_inodo(tipo, permisos);
                        printf("[buscar_entrada() -> reservado_inodo: %d  tipo: %c con permisos: %d para '%s']\n", entrada.ninodo, tipo, permisos, entrada.nombre);
                    }else{
                        return ERROR_NO_EXISTE_DIRECTORIO_INTERMEDIO;
                    }
                }else{
                    entrada.ninodo = reservar_inodo(tipo, permisos);
                    printf("[buscar_entrada() -> reservado_inodo: %d tipo: %c con permisos: %d para '%s']\n", entrada.ninodo, tipo, permisos, entrada.nombre);
                }

                fprintf(stderr, "[buscar_entrada() -> creada entrada: %s, %d]\n", inicial, entrada.ninodo);

                if (mi_write_f(*p_inodo_dir, &entrada, inodo_dir.tamEnBytesLog, sizeof(struct entrada)) == FALLO){
                    if (entrada.ninodo != FALLO){
                        liberar_inodo(entrada.ninodo);
                        fprintf(stderr, "[buscar_entrada() -> liberar inodo %i, reservado a %s]\n", num_entrada_inodo, inicial);
                    }
                    return FALLO;
                }
            }
        default:
            break;
        }

    }

    if (!strcmp(final, "/") || !strcmp(final, "")){

        if ((num_entrada_inodo < cant_entradas_inodo) && (reservar == 1)){
            return ERROR_ENTRADA_YA_EXISTENTE;
        }

        *p_inodo = num_entrada_inodo;
        *p_entrada = entrada.ninodo;
        return EXITO;
    }else{
        *p_inodo_dir = entrada.ninodo;
        return buscar_entrada(final, p_inodo_dir, p_inodo, p_entrada, reservar, permisos);
    }

}

void mostrar_error_buscar_entrada(int error){
    // fprintf(stderr, "Error: %d\n", error);
    switch (error){
    case -2:
        fprintf(stderr, "Error: Camino incorrecto.\n");
        break;
    case -3:
        fprintf(stderr, "Error: Permiso denegado de lectura.\n");
        break;
    case -4:
        fprintf(stderr, "Error: No existe el archivo o el directorio.\n");
        break;
    case -5:
        fprintf(stderr, "Error: No existe algún directorio intermedio.\n");
        break;
    case -6:
        fprintf(stderr, "Error: Permiso denegado de escritura.\n");
        break;
    case -7:
        fprintf(stderr, "Error: El archivo ya existe.\n");
        break;
    case -8:
        fprintf(stderr, "Error: No es un directorio.\n");
        break;
    }
}
