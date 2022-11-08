#define COMMAND_LINE_SIZE 1024
#define ARGS_SIZE 64
#define COMMAND_LINE_SIZE 1024

//colores para prompt y errores
#define RESET "\033[0m"
#define NEGRO_T "\x1b[30m"
#define NEGRO_F "\x1b[40m"
#define GRIS_T "\x1b[94m"
#define ROJO_T "\x1b[31m"
#define VERDE_T "\x1b[32m"
#define AMARILLO_T "\x1b[33m"
#define AZUL_T "\x1b[34m"
#define MAGENTA_T "\x1b[35m"
#define CYAN_T "\x1b[36m"
#define BLANCO_T "\x1b[97m"
#define NEGRITA "\x1b[1m"

char const PROMPT = '$';

#include <stdlib.h>
#include <stdio.h>
#include <stdbool.h>
#include <string.h>

char *read_line(char *line); 
int execute_line(char *line);
int parse_args(char **args,char *line);


int main(){
    char line[COMMAND_LINE_SIZE];  
    while(true){
        if(read_line(line)){
            execute_line(line);
            fflush(stdout);
        }
    }
}

char *read_line(char *line){

    //temporal (implementar metodo imprimirpromt())
    printf(BLANCO_T NEGRITA"%c: "RESET,PROMPT);
    
    //si la linea es diferente a null, cambiamos el salto de linea (\n) por un fin de linea (\0)
    if (fgets(line,COMMAND_LINE_SIZE,stdin) != NULL){
        char *salto;
        salto = strchr(line, '\n');
        if (salto){
            *salto = '\0';
        }
        //sino, miramos si hay final de fichero y salimos
    }else{
        if(feof(stdin)){
            printf(GRIS_T NEGRITA"saliendo del minishell...\n");
            exit(0);
        }
    }

    fflush(stdin);
    return line; 
}

int execute_line(char *line){

    fprintf(stderr,GRIS_T"EJEMPLO EJECUCION DE : %s\n"RESET,line);
}

int parse_args(char **args,char *line){

    int res = 0;
    const char s[2] = " ";
    char *token = strtok(line,s);
    bool salir = false;

    while((!salir) || (token != NULL)){
        if(args[res][0] != '#'){
            printf(GRIS_T NEGRITA"Token %i: %s\n",res,token);
            args[res++] = token;
            token = strtok(NULL,s);            
        }else{
            token = NULL;
            salir = true;
        }
    }

    printf(GRIS_T NEGRITA"Numero total de tokens: %i\n",res);

    return res;

}

int check_internal(char **args){
    
}
