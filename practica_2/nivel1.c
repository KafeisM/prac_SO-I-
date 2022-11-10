#define _POSIX_C_SOURCE 200112L
#define COMMAND_LINE_SIZE 1024
#define ARGS_SIZE 64

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
int internal_cd(char **args);
int internal_export(char **args);
int internal_source(char **args);
int internal_jobs(char **args);
int internal_fg(char **args);
int internal_bg(char **args);


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

int parse_args(char **args,char *line){

    int res = 0;
    const char s[2] = " ";
    char *token;
    token = strtok(line,s);

    while(token != NULL){
        args[res] = token;
        printf(GRIS_T NEGRITA"Token %i: %s\n",res,args[res]);
        if(args[res][0] != '#'){
            token = strtok(NULL,s);
            res++;           
        }else{
            token = NULL;
        }
    }
    args[res] = NULL;
    printf(GRIS_T NEGRITA"Token %i: %s\n",res,args[res]);
    printf(GRIS_T NEGRITA"Numero total de tokens: %i\n",res);

    return res;

}

int check_internal(char **args){
    if(args[0] == NULL){
        return 0;
    }else if(strcmp(args[0],"cd") == 0){
        internal_cd(args);
        return 1;
    }else if(strcmp(args[0],"export") == 0){
        internal_export(args);
        return 1;
    }else if(strcmp(args[0],"source")== 0){
        internal_source(args);
        return 1;
    }else if(strcmp(args[0],"jobs")== 0){
        internal_jobs(args);
        return 1;
    }else if(strcmp(args[0],"fg")== 0){
        internal_fg(args);
        return 1;
    }else if(strcmp(args[0],"bg")== 0){
        internal_bg(args);
        return 1;
    }else if(strcmp(args[0],"exit")== 0){
        exit(0);
    }else{  
        printf("No es un comando interno\n");
        return 0;
    }
}

int internal_cd(char **args){
    fprintf(stderr, GRIS_T "[internal_cd()→ Esta función cambiará de directorio]\n" RESET);
    return 1;
}

int internal_export(char **args)
{

    fprintf(stderr, GRIS_T "[internal_export()→ Esta función indica cual es el intérprete de comando que se esta usando]\n" RESET);
    return 1;
}

int internal_source(char **args)
{

    fprintf(stderr, GRIS_T "[internal_source()→ Esta función hace que el proceso se ejecute sin crear ningún proceso hijo]\n" RESET);

    return -1;
}

int internal_jobs(char **args)
{

    fprintf(stderr, GRIS_T "[internal_jobs()→ Esta función nos muestra los trabajos vinculados a la terminal desde donde se ejecuta]\n" RESET);

    return 1;
}

int internal_fg(char **args)
{

    fprintf(stderr, GRIS_T "[internal_fg()→ Esta función lleva los procesos más recientes a primer plano]\n" RESET);

    return 1;
}

int internal_bg(char **args)
{

    fprintf(stderr, GRIS_T "[internal_bg()→ Esta función enseña los procesos parados o en segundo plano]\n" RESET);

    return 1;
}

int execute_line(char *line){
    char *args[ARGS_SIZE];
    int num_tokens;
    int interno;
    num_tokens = parse_args(args, line);
    interno = check_internal(args);
}
