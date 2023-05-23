/*JOSEP GABRIEL FORNÉS REYNÉS, JORDI FLORIT ENSENYAT, PAU GIRÓN RODRÍGUEZ*/

#include "directorios.h"

static struct UltimaEntrada UltimasEntradas[CACHE];
int maxcaxhe = CACHE;

/*---------------------------------------------------------------------------------------------------------
* Dada una cadena de (camino que empieze por '/') separa su contenido
* Input:    camino: direccion del fichero o directorio
*           inicial: directorio/fichero mas proximo en el que estamos
*           final: camino restante
*           tipo: f -> fichero | d -> directorio
* Output:   FALLO o EXITO
---------------------------------------------------------------------------------------------------------*/

int extraer_camino(const char *camino, char *inicial, char *final, char *tipo){

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
        }
    }else{
        strcpy(inicial, (camino + 1));
        strcpy(final, "");
    }

    /*if (!dir){ // miramos si no es un directorio
        strncpy(inicial, camino + 1, sizeof(char) * strlen(camino) - 1);
        strcpy(tipo, "f");
    }*/

    return EXITO;
}

/*---------------------------------------------------------------------------------------------------------
 * Función que buscara una entrada en el sistema de archivos, y reservara o no segun el parametro
 * @reservar un inodo para dicho elemento. Si ha ido bien devulve el id del inodo del elemento.
 * 
 * Input:  camino_parcial: camino a recorrer
 *         p_inodo_dir: id del inodo del directorio padre
 *         p_inodo: id del inodo del destino final a obtener
 *         p_entrada: numero de entrada dentro del directorio padre.
 *         reservar: indicado si se ha de reservar un nuevo elemento: (1 si o 0 no).
 *         permisos: permisos si se tiene que reservar
 * 
 * Output: EXITO o codigo de error
 ---------------------------------------------------------------------------------------------------------*/

int buscar_entrada(const char *camino_parcial, unsigned int *p_inodo_dir, unsigned int *p_inodo, unsigned int *p_entrada,
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
    memset(entrada.nombre, 0, sizeof(entrada.nombre));

    //cuando camino parcial es una /
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

    //extraemos el camino y controlamos el error
    if (extraer_camino(camino_parcial, inicial, final, &tipo) < 0){
        //fprintf(stderr, ROJO_T"buscar_entrada(): Error camino incorrecto\n"RESET);
        return ERROR_CAMINO_INCORRECTO;
    }

    //printf("[buscar_entrada()->inicial: %s, final: %s, reservar: %d]\n", inicial, final, reservar);

    //leemos el inodo y miramos que tenga permiso de lectura
    if (leer_inodo(*p_inodo_dir, &inodo_dir) == FALLO){
        return FALLO;
    }

    if ((inodo_dir.permisos & 4) != 4){
        return ERROR_PERMISO_LECTURA;
    }

    //buffer de lectura
    //struct entrada bufferLectura[BLOCKSIZE/sizeof(struct entrada)];
    //memset(bufferLectura, 0, (BLOCKSIZE/sizeof(struct entrada)*sizeof(struct entrada)));
    memset(entrada.nombre, 0, sizeof(entrada.nombre));

    //cantidad de entradas del inodo y nº de entrada inicial
    cant_entradas_inodo = inodo_dir.tamEnBytesLog/sizeof(struct entrada);
    num_entrada_inodo = 0;

    if (cant_entradas_inodo > 0){

        if (mi_read_f(*p_inodo_dir, &entrada, num_entrada_inodo*sizeof(struct entrada), sizeof(struct entrada)) < 0){
            return ERROR_PERMISO_LECTURA;
        }

        //volvermos a llenar el buffer de 0s
        while(num_entrada_inodo < cant_entradas_inodo && strcmp(inicial, entrada.nombre) != 0){
            num_entrada_inodo++;
            memset(entrada.nombre, 0, sizeof(entrada.nombre));
            if (mi_read_f(*p_inodo_dir, &entrada, num_entrada_inodo*sizeof(struct entrada), sizeof(struct entrada)) < 0){
                return ERROR_PERMISO_LECTURA;
            }
        }

    }
    //si ya se han recorrido todas las entradas y no hemos encontrado la inicial
    if ((num_entrada_inodo == cant_entradas_inodo) && (strcmp(entrada.nombre, inicial) != 0)){

        //casos según reservar
        switch (reservar){
        case 0: //consulta. no modificamos datos
            return ERROR_NO_EXISTE_ENTRADA_CONSULTA;
            break;
        case 1://escribir
        
            //no se puede escribir en ficheros
            if (inodo_dir.tipo == 'f'){
                return ERROR_NO_SE_PUEDE_CREAR_ENTRADA_EN_UN_FICHERO;
            }

            //el inodo necesita permiso de escritura
            if ((inodo_dir.permisos & 2) != 2){
                return ERROR_PERMISO_ESCRITURA;
            }else{
                strcpy(entrada.nombre, inicial);
                if (tipo == 'd'){//es un directorio
                    if (strcmp(final, "/") == 0){
                        //reservamos un inodo para usarlo como directorio en entrada.inodo
                        entrada.ninodo = reservar_inodo('d', permisos);
                        //printf("[buscar_entrada() -> reservado_inodo: %d  tipo: %c con permisos: %d para '%s']\n", entrada.ninodo, tipo, permisos, entrada.nombre);
                    }else{
                        return ERROR_NO_EXISTE_DIRECTORIO_INTERMEDIO;
                    }
                }else{//es un fichero
                    entrada.ninodo = reservar_inodo('f', permisos);
                    //printf("[buscar_entrada() -> reservado_inodo: %d tipo: %c con permisos: %d para '%s']\n", entrada.ninodo, tipo, permisos, entrada.nombre);
                }

                //fprintf(stderr, "[buscar_entrada() -> creada entrada: %s, %d]\n", inicial, entrada.ninodo);
                
                //escribimos la entrada en el directorio padre y controlamos el error

                int error = mi_write_f(*p_inodo_dir, &entrada, num_entrada_inodo * sizeof(struct entrada), sizeof(struct entrada));

                //si hay error liberamos la entrada
                if (error < 0){
                    if (entrada.ninodo != FALLO){
                        liberar_inodo(entrada.ninodo);
                        //fprintf(stderr, "[buscar_entrada() -> liberar inodo %i, reservado a %s]\n", num_entrada_inodo, inicial);
                    }
                    return FALLO;
                }
            }
        
        }

    }

    //si se cumplen las condiciones de final de camino
    if (!strcmp(final, "/") || !strcmp(final, "")){

        //si no existe la entrada y reservar indica modo escritura
        if ((num_entrada_inodo < cant_entradas_inodo) && (reservar == 1)){
            return ERROR_ENTRADA_YA_EXISTENTE;
        }

        *p_inodo = entrada.ninodo;
        *p_entrada = num_entrada_inodo;
        return EXITO;
    }else{
        *p_inodo_dir = entrada.ninodo;
        return buscar_entrada(final, p_inodo_dir, p_inodo, p_entrada, reservar, permisos);
    }   

    return EXITO;
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

/*---------------------------------------------------------------------------------------------------------
* Función de la capa de directorios que crea un fichero/directorio y su entrada de directorio.
* Input:    camino: direccion del fichero o directorio
*           permisos: permisos que le tenemos que pasar a buscar_entrada
* Output:   el error o EXITO
---------------------------------------------------------------------------------------------------------*/

int mi_creat(const char *camino, unsigned char permisos){
    

    mi_waitSem();
    //ponemos las variables a 0
    unsigned int p_inodo_dir = 0;
    unsigned int p_inodo = 0;
    unsigned int p_entrada = 0;

    //llamamos a buscar entrada con reservar a 1
    int error = buscar_entrada(camino, &p_inodo_dir, &p_inodo, &p_entrada, 1, permisos);
    //tratamos el error
    if(error < 0){
        mi_signalSem();
        return error;
    }

    mi_signalSem();

    return EXITO;

}

/*---------------------------------------------------------------------------------------------------------
* Función de la capa de directorios que pone el contenido del directorio en un buffer de memoria
* Input:    camino: direccion del fichero o directorio
*           buffer: metemos el contenido dentro de este buffer
* Output:   numeroEntradas: numero de entradas
*           o FALLO
---------------------------------------------------------------------------------------------------------*/

int mi_dir(const char *camino, char *buffer){

    //definimos variables
    struct tm *tm;
    unsigned int p_inodo_dir = 0;
    unsigned int p_inodo = 0;
    unsigned int p_entrada = 0;
    int numeroEntradas = 0;

    //llamamos a buscar entrada con los permisos correspondientes para leer
    int error = buscar_entrada(camino, &p_inodo_dir, &p_inodo, &p_entrada, 0, 4); //Permisos para leer
    if (error < 0){ //tratamos el error
        mostrar_error_buscar_entrada(error);
        return FALLO;
    }

    //para obtener el inodo correspondiente
    struct inodo inodo;
    if (leer_inodo(p_inodo, &inodo) < 0){
        return FALLO;
    }

    //comprobamos permisos
    if ((inodo.permisos & 4) != 4){
        return FALLO;
    }

    //definimos estos arrays para meter el tiempo y el tamaño de la entrada
    char t[100];
    char tamanyoBytes[10];

    //Buffer de salida
    struct entrada entradas[BLOCKSIZE / sizeof(struct entrada)];
    memset(&entradas, 0, sizeof(struct entrada));

    //obtenemos el numero de entradas
    numeroEntradas = inodo.tamEnBytesLog / sizeof(struct entrada);

    //obtenemos el offset
    int offset = 0;
    offset += mi_read_f(p_inodo, entradas, offset, BLOCKSIZE);

    //hacemos un for para todas las entradas
    for(int i = 0; i < numeroEntradas; i++){

        //obtenemos el inodo correspondiente
        if (leer_inodo(entradas[i % (BLOCKSIZE / sizeof(struct entrada))].ninodo, &inodo) == FALLO){
            return FALLO;
        }

        //obtenemos tipo
        if (inodo.tipo == 'd'){
            strcat(buffer, "d");

        }else{
            strcat(buffer, "f");
        }
        strcat(buffer, "\t");

        //obtenemos permisos
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

        //obtenemos el tiempo
        tm = localtime(&inodo.mtime);
        sprintf(t, "%d-%02d-%02d %02d:%02d:%02d", tm->tm_year + 1900, tm->tm_mon + 1, tm->tm_mday, tm->tm_hour, tm->tm_min,  tm->tm_sec);
        strcat(buffer, t);
        strcat(buffer, "\t");

        //obtenemos el tamaño
        sprintf(tamanyoBytes, "%d", inodo.tamEnBytesLog);
        strcat(buffer, tamanyoBytes);
        strcat(buffer, "\t");

        //obtenemos el nombre
        strcat(buffer, entradas[i % (BLOCKSIZE / sizeof(struct entrada))].nombre);
        while ((strlen(buffer) % TAMFILA) != 0){
            strcat(buffer, " ");
        }

        //pasamos a la siguiente entrada
        strcat(buffer, RESET);
        strcat(buffer, "\n");

        //cambiamos el offset
        if (offset % (BLOCKSIZE / sizeof(struct entrada)) == 0){
            offset += mi_read_f(p_inodo, entradas, offset, BLOCKSIZE);
        }

    }
    
    return numeroEntradas;
}

/*---------------------------------------------------------------------------------------------------------
* Buscar la entrada *camino con buscar_entrada() para obtener el nº de inodo (p_inodo) y llamar a 
* mi_chmod_f
* Input:    camino: direccion del fichero/directorio .
* Output:   FALLO o EXITO
---------------------------------------------------------------------------------------------------------*/

int mi_chmod(const char *camino, unsigned char permisos){

    unsigned int p_inodo_dir = 0;
    unsigned int p_inodo = 0;
    unsigned int p_entrada = 0;
    int error;

    //buscamos la entrada para obtener el inodo
    error = buscar_entrada(camino, &p_inodo_dir, &p_inodo, &p_entrada, 0, 6);
    if (error < 0) {
        return error;
    }

    //cambiamos los permisos del inodo
    if(mi_chmod_f(p_inodo,permisos)){
        return FALLO;
    }
    
    return EXITO;
}

/*---------------------------------------------------------------------------------------------------------
* Función de para escribir contenido en un fichero.
* Input:    camino: direccion del fichero .
*           buf: buffer del contenido a escribir
*           offset: posicion del offset
*           nbytes: bytyes a escribir
* Output:   FALLO o EXITO
---------------------------------------------------------------------------------------------------------*/
int mi_write(const char *camino, const void *buf, unsigned int offset, unsigned int nbytes){
    bool found = false;
    unsigned int p_inodo_dir = 0;
    unsigned int p_inodo = 0;
    unsigned int p_entrada = 0;
    int error;
    int bytes_escritos;

    // miramos en la cache para ver si la lectura es sobre un inodo que tenemos guardadp
    for (int i = 0; i < (maxcaxhe - 1) && !found; i++){

        if (strcmp(UltimasEntradas[i].camino, camino) == 0){ // Si la escritura es sobre el mismo inodo
            p_inodo = UltimasEntradas[i].p_inodo;
            found = true;
        }
    }

    // si no se ha encontrado, buscamos su inodo con buscar entrada y actualizamos la cache
    if (!found){
        error = buscar_entrada(camino, &p_inodo_dir, &p_inodo, &p_entrada, 0, 2);
        if (error < 0){
            return error;
        }

        // miramos si la cache aun no está llena
        if (maxcaxhe > 0){
            // metemos en la cache el camino actual con su correspondiente inodo
            strcpy(UltimasEntradas[CACHE - maxcaxhe].camino, camino);
            UltimasEntradas[CACHE - maxcaxhe].p_inodo = p_inodo;
            maxcaxhe = maxcaxhe - 1; // decrementamos el contador de elementos en la caché actual

<<<<<<< Updated upstream
            fprintf(stderr, AZUL_T"[mi_write() → Utilizamos la caché de lectura]\n"RESET);
=======
            fprintf(stderr, AZUL_T"[mi_write() → Actualizamos la caché de lectura]\n"RESET);
>>>>>>> Stashed changes

        }else{
            // si esta llena debemos remplazar el elemento mas antiguo (modelo FIFO)
            for (int i = 0; i < CACHE - 1; i++){
                // movemos todas las entradas hacia la izquierda (eliminado el mas aniguo y dejando espacio para la nueva entrada)
                strcpy(UltimasEntradas[i].camino, UltimasEntradas[i + 1].camino);
                UltimasEntradas[i].p_inodo = UltimasEntradas[i + 1].p_inodo;
            }

            // añadimos la nueva entrada
            strcpy(UltimasEntradas[CACHE - 1].camino, camino);
            UltimasEntradas[CACHE - 1].p_inodo = p_inodo;

<<<<<<< Updated upstream
            fprintf(stderr, AZUL_T"[mi_write() → Reemplazamos la caché de lectura]\n"RESET);
=======
            fprintf(stderr, AZUL_T"[mi_write() → Actualizamos la caché de lectura]\n"RESET);
>>>>>>> Stashed changes
        }
    }

    //Escribimos en el archivo
    bytes_escritos = mi_write_f(p_inodo, buf, offset, nbytes);
    if (bytes_escritos == FALLO){
        bytes_escritos = 0;
    }
    return bytes_escritos;
}

/*---------------------------------------------------------------------------------------------------------
* Función de para leer contenido de un fichero.
* Input:    camino: direccion del fichero .
*           buf: buffer para posicionar el contenido leido
*           offset: posicion del offset
*           nbytes: bytyes a escribir
* Output:   FALLO o EXITO
---------------------------------------------------------------------------------------------------------*/
int mi_read(const char *camino, void *buf, unsigned int offset, unsigned int nbytes){

    bool found = false;
    unsigned int p_inodo_dir = 0;
    unsigned int p_inodo = 0;
    unsigned int p_entrada = 0;
    int error;
    int bytes_leidos;

    // miramos en la cache para ver si la lectura es sobre un inodo que tenemos guardadp
    for (int i = 0; i < (maxcaxhe - 1) && !found; i++){

        if (strcmp(UltimasEntradas[i].camino, camino) == 0){ // Si la escritura es sobre el mismo inodo
            p_inodo = UltimasEntradas[i].p_inodo;
            found = true;
        }
    }

    // si no se ha encontrado, buscamos su inodo con buscar entrada y actualizamos la cache
    if (!found){
        error = buscar_entrada(camino, &p_inodo_dir, &p_inodo, &p_entrada, 0, 2);
        if (error < 0){
            return error;
        }

        // miramos si la cache aun no está llena
        if (maxcaxhe > 0){
            // metemos en la cache el camino actual con su correspondiente inodo
            strcpy(UltimasEntradas[CACHE - maxcaxhe].camino, camino);
            UltimasEntradas[CACHE - maxcaxhe].p_inodo = p_inodo;
            maxcaxhe = maxcaxhe - 1; // decrementamos el contador de elementos en la caché actual

<<<<<<< Updated upstream
            fprintf(stderr, AZUL_T"[mi_read() → Utilizamos la caché de lectura]\n"RESET);
=======
            fprintf(stderr, AZUL_T"[mi_read() → Actualizamos la caché de lectura]\n"RESET);
>>>>>>> Stashed changes

        }else{
            // si esta llena debemos remplazar el elemento mas antiguo (modelo FIFO)
            for (int i = 0; i < CACHE - 1; i++){
                // movemos todas las entradas hacia la izquierda (eliminado el mas aniguo y dejando espacio para la nueva entrada)
                strcpy(UltimasEntradas[i].camino, UltimasEntradas[i + 1].camino);
                UltimasEntradas[i].p_inodo = UltimasEntradas[i + 1].p_inodo;
            }

            // añadimos la nueva entrada
            strcpy(UltimasEntradas[CACHE - 1].camino, camino);
            UltimasEntradas[CACHE - 1].p_inodo = p_inodo;

<<<<<<< Updated upstream
            fprintf(stderr, AZUL_T"[mi_read() → Reemplazamos la caché de lectura]\n"RESET);
=======
            fprintf(stderr, AZUL_T"[mi_read() → Actualizamos la caché de lectura]\n"RESET);
>>>>>>> Stashed changes
        }
        
    }

    //Escribimos en el archivo
    bytes_leidos = mi_read_f(p_inodo, buf, offset, nbytes);
    if (bytes_leidos == FALLO){
        bytes_leidos = 0;
    }
    return bytes_leidos;
}

/*---------------------------------------------------------------------------------------------------------
* Obtiene la metainformacion del elemento del camino
* Input:    camino: direccion del fichero o directorio
*           p_stat: dirección de la metainformacion
* Output:   p_inodo: puntero al inodo
*           error.
---------------------------------------------------------------------------------------------------------*/

int mi_stat(const char *camino, struct STAT *p_stat){

    //variable superbloque para leer la posicion del inodo raíz
    struct superbloque SB;
    bread(posSB, &SB);
    //variables para buscar la entrada
    unsigned int p_inodo_dir, p_inodo;
    unsigned int p_entrada;
    p_inodo_dir = p_inodo = SB.posInodoRaiz;
    //buscar entrada correspondiente, sin escribir y con permisos de lectura
    int error = buscar_entrada(camino, &p_inodo_dir, &p_inodo, &p_entrada, 0, 4);

    //control del error
    if (error < 0){
        return error;
    }

    //llamamos a la funcion de la capa de ficheros para obtener la metainformacion
    mi_stat_f(p_inodo, p_stat);
    return p_inodo;
}

/*---------------------------------------------------------------------------------------------------------
* Función que crea el enlace entre una entrada de directo al inodo indicado por otra entrada de directorio
* Input:    camino1: direccion del fichero 1.
*           camino2: direccion del fichero donde es creado el enlace
* Output:   FALLO o EXITO
---------------------------------------------------------------------------------------------------------*/

int mi_link(const char *camino1, const char *camino2){


    mi_waitSem();
    //variables para obtener ambas entradas y control de error
    unsigned int p_inodo_dir1, p_inodo1, p_entrada1 = 0;
    p_inodo_dir1 = p_inodo1 = 0;
    unsigned int p_inodo_dir2, p_inodo2, p_entrada2 = 0;
    p_inodo_dir2 = p_inodo2 = 0;
    int error;

    //buscamos la primera entrada con permisos de lectura y no escritura
    error = buscar_entrada(camino1, &p_inodo_dir1, &p_inodo1, &p_entrada1, 0, 4);
    if (error < 0){
        mi_signalSem();
        return error;
    }

    //buscamos segunda entrada, con escritura
    error = buscar_entrada(camino2, &p_inodo_dir2, &p_inodo2, &p_entrada2, 1, 6);
    if (error < 0){
        mi_signalSem();
        return error;
    }

    //leemos el contenido del inodo padre
    struct entrada entrada;
    if (mi_read_f(p_inodo_dir2, &entrada, sizeof(struct entrada) * (p_entrada2), sizeof(struct entrada)) < 0){
        fprintf(stderr, ROJO_T"mi_link(): error mi_read_f"RESET);
        mi_signalSem();
        return FALLO;
    }
    //creamos el link
    entrada.ninodo = p_inodo1;

    //actualizamos los resultados
    if (mi_write_f(p_inodo_dir2, &entrada, sizeof(struct entrada) * (p_entrada2), sizeof(struct entrada)) < 0){
        fprintf(stderr, ROJO_T"mi_link(): error mi_write_f"RESET);
        mi_signalSem();
        return FALLO;
    }

    //liberamos el segundo inodo y controlamos el error
    if (liberar_inodo(p_inodo2) < 0){
        fprintf(stderr, ROJO_T"mi_link(): error liberar_inodo"RESET);
        mi_signalSem();
        return FALLO;
    }

    //leemos el inodo y aumentamos el numero de links
    struct inodo inodo;
    if (leer_inodo(p_inodo1, &inodo) < 0){
        fprintf(stderr, ROJO_T"mi_link(): error leer_inodo"RESET);
        mi_signalSem();
        return FALLO;
    }
    inodo.nlinks++;
    inodo.ctime = time(NULL);
    if (escribir_inodo(p_inodo1, &inodo) < 0){
        fprintf(stderr, ROJO_T"mi_link(): error escribir_inodo"RESET);
        mi_signalSem();
        return FALLO;
    }

    return EXITO;

}

/*---------------------------------------------------------------------------------------------------------
* Función de la capa de directorios que borra la entrada de directorio especificaday , 
* en caso de que fuera el último enlace existente, borrar el propio fichero/directorio.
* Input:    camino: direccion del fichero/directorio .
* Output:   FALLO o EXITO
---------------------------------------------------------------------------------------------------------*/
int mi_unlink(const char *camino){

    mi_waitSem();

    struct superbloque SB;
    struct inodo inodoRM;
    struct inodo inodoDIR;
    unsigned int p_inodo_dir, p_inodo;
    unsigned int p_entrada = 0;
    int error;

    if(bread(posSB, &SB) == FALLO){
        fprintf(stderr, ROJO_T"mi_unlink: Error bread\n"RESET);
    }
    p_inodo_dir = p_inodo = SB.posInodoRaiz;

    if ((error = buscar_entrada(camino, &p_inodo_dir, &p_inodo, &p_entrada, 0, 4)) < 0){
        mostrar_error_buscar_entrada(error);
        mi_signalSem();
        return FALLO;
    }

    if (SB.posInodoRaiz == p_inodo){
        fprintf(stderr, ROJO_T"mi_unlink: El inodo es el inodo raiz\n"RESET);
        mi_signalSem();
        return FALLO;
    }

    
    if (leer_inodo(p_inodo, &inodoRM) < 0){
        return FALLO;
    }
    
    if ((inodoRM.tipo == 'd') && (inodoRM.tamEnBytesLog > 0)){
        fprintf(stderr, ROJO_T"mi_unlink: El directorio '%s' no está vacío\n"RESET ,camino);
        mi_signalSem();
        return FALLO;
    }

    
    if (leer_inodo(p_inodo_dir, &inodoDIR) < 0){
        mi_signalSem();
        return FALLO;
    }

    int num_entrada = inodoDIR.tamEnBytesLog / sizeof(struct entrada);

    if (p_entrada != num_entrada - 1){

        struct entrada entrada;
        if (mi_read_f(p_inodo_dir, &entrada, sizeof(struct entrada) * (num_entrada - 1), sizeof(struct entrada)) < 0){
            mi_signalSem();
            return FALLO;
        }

        if (mi_write_f(p_inodo_dir, &entrada, sizeof(struct entrada) * (p_entrada), sizeof(struct entrada)) < 0){
            mi_signalSem();
            return FALLO;
        }

    }

    if (mi_truncar_f(p_inodo_dir, sizeof(struct entrada) * (num_entrada - 1)) < 0){
        mi_signalSem();
        return FALLO;
    }

    inodoRM.nlinks--;


    if (inodoRM.nlinks == 0){
        
        if (liberar_inodo(p_inodo) < 0){
            mi_signalSem();
            return FALLO;
        }
    }else{

        inodoRM.ctime = time(NULL);
        if (escribir_inodo(p_inodo, &inodoRM) < 0){
            mi_signalSem();
            return FALLO;
        }
    }
    mi_signalSem();
    return EXITO;
}
