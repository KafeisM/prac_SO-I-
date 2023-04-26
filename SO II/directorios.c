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

    if(camino[0] == '/'){

        //buscamos la siguiente '/'
        for(int i = 1; i < strlen(camino);i++){
            if(camino[i] == '/'){
                dir = true;
                strncpy(inicial,camino+1,sizeof(char)*i-1); //guardar la porcion entre los 2 primeros '/'
                strcpy(final, camino + 1); //guardar el resto del camino a partir del segundo '/'
                strcpy(tipo,"d"); //indicamos que es tipo directorio
                break;
            }
        }

        if(!dir){ //miramos si no es un directorio
            strncpy(inicial,camino+1,sizeof(char)*strlen(camino)-1); 
            strcpy(tipo,"f");

            //en caso de fichero no guardamos nada en final
        }

    }else{
        return FALLO;  
    }

    return EXITO;

}

int buscar_entrada(const char *camino_parcial, unsigned int *p_inodo, unsigned int *p_inodo_dir, unsigned int *p_entrada, char reservar, unsigned char permisos){

}

void mostrar_error_buscar_entrada(int error) {
   // fprintf(stderr, "Error: %d\n", error);
   switch (error) {
   case -2: fprintf(stderr, "Error: Camino incorrecto.\n"); break;
   case -3: fprintf(stderr, "Error: Permiso denegado de lectura.\n"); break;
   case -4: fprintf(stderr, "Error: No existe el archivo o el directorio.\n"); break;
   case -5: fprintf(stderr, "Error: No existe algún directorio intermedio.\n"); break;
   case -6: fprintf(stderr, "Error: Permiso denegado de escritura.\n"); break;
   case -7: fprintf(stderr, "Error: El archivo ya existe.\n"); break;
   case -8: fprintf(stderr, "Error: No es un directorio.\n"); break;
   }
}