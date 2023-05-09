/*JOSEP GABRIEL FORNÉS REYNÉS, JORDI FLORIT ENSENYAT, PAU GIRÓN RODRÍGUEZ*/

#include "directorios.h"

static struct UltimaEntrada UltimasEntradas[CACHE];

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

    // buscamos la primera barra y truncamos el camino
    char *resto = strchr((camino + 1), '/');
    strcpy(tipo, "f");

    if (resto){
        //Inicial = camino - resto (
        strncpy(inicial, (camino + 1), (strlen(camino) - strlen(resto) - 1));
        //Final = resto
        strcpy(final, resto);

        //Miramos si es un directorio
        if (final[0] == '/'){
            strcpy(tipo, "d");
            dir = true;
        }
    }

    if (!dir){ // miramos si no es un directorio
        strncpy(inicial, camino + 1, sizeof(char) * strlen(camino) - 1);
        strcpy(final, "");
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
            //fprintf(stderr, ROJO_T"buscar_entrada(): Error bread SB\n"RESET);
            return FALLO;
        }
        *p_inodo = SB.posInodoRaiz;
        *p_entrada = 0;
        return EXITO;
    }


    if (extraer_camino(camino_parcial, inicial, final, &tipo) == FALLO){
        //fprintf(stderr, ROJO_T"buscar_entrada(): Error camino incorrecto\n"RESET);
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
                    if (strcmp(final, "/") == 0){
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
    switch (error){
    case -2:
        fprintf(stderr,ROJO_T "Error: Camino incorrecto.\n"RESET);
        break;
    case -3:
        fprintf(stderr,ROJO_T "Error: Permiso denegado de lectura.\n"RESET);
        break;
    case -4:
        fprintf(stderr,ROJO_T "Error: No existe el archivo o el directorio.\n"RESET);
        break;
    case -5:
        fprintf(stderr,ROJO_T "Error: No existe algún directorio intermedio.\n"RESET);
        break;
    case -6:
        fprintf(stderr,ROJO_T "Error: Permiso denegado de escritura.\n"RESET);
        break;
    case -7:
        fprintf(stderr,ROJO_T "Error: El archivo ya existe.\n"RESET);
        break;
    case -8:
        fprintf(stderr,ROJO_T "Error: No es un directorio.\n"RESET);
        break;
    }
}

int mi_creat(const char *camino, unsigned char permisos){
    //crea fichero/directorio y su entrada de directorio
    unsigned int p_inodo_dir = 0;
    unsigned int p_inodo = 0;
    unsigned int p_entrada = 0;

    int error = buscar_entrada(camino, &p_inodo_dir, &p_inodo, &p_entrada, 1, permisos);

    if(error < 0){
        return error;
    }

    return EXITO;

}

int mi_dir(const char *camino, char *buffer){
    //pone el contenido del directorio en un buffer de memoria y devuelve el número de entadas

    struct tm *tm;

    unsigned int p_inodo_dir = 0;
    unsigned int p_inodo = 0;
    unsigned int p_entrada = 0;
    int error;
    int nEntradas = 0;

    error = buscar_entrada(camino, &p_inodo_dir, &p_inodo, &p_entrada, 0, 4); //Permisos para leer
    if (error < 0){
        mostrar_error_buscar_entrada(error);
        return FALLO;
    }

    struct inodo inodo;
    if (leer_inodo(p_inodo, &inodo) < 0){
        return FALLO;
    }

    if ((inodo.permisos & 4) != 4){
        return FALLO;
    }

    char tiempo[100];       //Para el tiempo
    char tamEnBytes[10]; //10 = valor maximo de un unsigned int

    //Buffer de salida
    struct entrada entradas[BLOCKSIZE / sizeof(struct entrada)];
    memset(&entradas, 0, sizeof(struct entrada));

    nEntradas = inodo.tamEnBytesLog / sizeof(struct entrada);

    int offset = 0;
    offset += mi_read_f(p_inodo, entradas, offset, BLOCKSIZE);

    for(int i = 0; i < nEntradas; i++){

        if (leer_inodo(entradas[i % (BLOCKSIZE / sizeof(struct entrada))].ninodo, &inodo) < 0){
            return FALLO;
        }

        //Tipo
        if (inodo.tipo == 'd'){
            strcat(buffer, "d");

        }else{
            strcat(buffer, "f");
        }
        strcat(buffer, "\t");

        //Permisos
        if (inodo.permisos & 4){
        strcat(buffer, "r"); 
        }else{
            strcat(buffer, "-");
        }

        if(inodo.permisos & 2){
            strcat(buffer, "w"); 
        }else{
            strcat(buffer, "-");
        } 

        if(inodo.permisos & 1){
            strcat(buffer, "x");
        }else{
            strcat(buffer, "-");
        }
        strcat(buffer, "\t");

        //Tiempo
        tm = localtime(&inodo.mtime);
        sprintf(tiempo, "%d-%02d-%02d %02d:%02d:%02d", tm->tm_year + 1900, tm->tm_mon + 1, tm->tm_mday, tm->tm_hour, tm->tm_min,  tm->tm_sec);
        strcat(buffer, tiempo);
        strcat(buffer, "\t");

        //Tamaño
        sprintf(tamEnBytes, "%d", inodo.tamEnBytesLog);
        strcat(buffer, tamEnBytes);
        strcat(buffer, "\t");

        //Nombre
        strcat(buffer, entradas[i % (BLOCKSIZE / sizeof(struct entrada))].nombre);
        while ((strlen(buffer) % TAMFILA) != 0){
            strcat(buffer, " ");
        }

        //Siguiente
        strcat(buffer, RESET);
        strcat(buffer, "\n");

        if (offset % (BLOCKSIZE / sizeof(struct entrada)) == 0){
            offset += mi_read_f(p_inodo, entradas, offset, BLOCKSIZE);
        }

    }
    
    return nEntradas;
}



int mi_chmod(const char *camino, unsigned char permisos){
    unsigned int p_inodo_dir = 0;
    unsigned int p_inodo = 0;
    unsigned int p_entrada = 0;
    int error;
    error = buscar_entrada(camino, &p_inodo_dir, &p_inodo, &p_entrada, 0, 6);
   
    if (error < 0) {
        return error;
    }

    if(mi_chmod_f(p_inodo,permisos)){
        return FALLO;
    }
    
    return EXITO;
}

int mi_write(const char *camino, const void *buf, unsigned int offset, unsigned int nbytes){
    //Escribe el contenido en un fichero

    unsigned int p_inodo_dir = 0;
    unsigned int p_inodo = 0;
    unsigned int p_entrada = 0;
    int error;

    for(int i = 0; i < CACHE; i++){
        if(strcmp(UltimasEntradas[i].camino, camino) == 0){ //Si la escritura es sobre el mismo inodo
            p_inodo = UltimasEntradas[i].p_inodo;
        }else{
            error = buscar_entrada(camino, &p_inodo_dir, &p_inodo, &p_entrada, 0, 4);
            if(error < 0){
                return FALLO;
            }else{
                strcpy(UltimasEntradas[i].camino, camino);
                UltimasEntradas[i].p_inodo = p_inodo;
            }


        }
    }

    


    
}
