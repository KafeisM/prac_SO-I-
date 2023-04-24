/*JOSEP GABRIEL FORNÉS REYNÉS, JORDI FLORIT ENSENYAT, PAU GIRÓN RODRÍGUEZ*/

#include "directorios.h"
#include <string.h>


int extraer_camino(const char *camino, char *inicial, char *final, char *tipo){
    bool dir = false;

    if(camino[0] == '/'){
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
        
        if(dir){
            return 1; //es directorio
        }else{
            return 0; //es dichero
        }
        
    }else{
        fprintf(stderr,ROJO_T"Camino incorrecto."RESET);
        return FALLO;  
    }

}

int buscar_entrada(const char *camino_parcial, unsigned int *p_inodo, unsigned int *p_inodo_dir, unsigned int *p_entrada, char reservar, unsigned char permisos){

}