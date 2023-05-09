#include "directorios.h";

//Sintaxis: ./mi_chmod <disco> <permisos> </ruta>
int main(int argc, char const *argv[]){

    unsigned int permisos = atoi(argv[2]);
    int longitud = strlen(argv[3]);
    unsigned char directorio = argv[3];

    if(argc != 4){
        fprintf(stderr, ROJO_T "Sintaxis: ./mi_chmod <disco> <permisos> </ruta>\n");
        return FALLO;
    }

    if(permisos < 0 && permisos > 7){
        fprintf(stderr, ROJO_T "Permisos incorrectos(0-7)\n");
        return FALLO;
    }else{
        bmount(argv[1]);
        int error = mi_chmod(directorio,permisos);
        if(error < 0){
            mostrar_error_buscar_entrada(error);
            return FALLO;
        }
         bumount();
    }
    
    return EXITO;
}
