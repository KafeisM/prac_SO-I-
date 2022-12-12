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
#define SUCCES 0
#define FAILURE -1
#define COMMAND_LINE_SIZE 1024
#define PWD

char const PROMPT = '$';
char *user;
char *home;

#include <stdlib.h>
#include <stdio.h>
#include <stdbool.h>
#include <string.h>
#include <unistd.h>

char *read_line(char *line); 
int execute_line(char *line);
int parse_args(char **args,char *line);
int cd_avanzado(char **args);
int internal_cd(char **args);
int internal_export(char **args);
int internal_source(char **args);
int internal_jobs(char **args);
int internal_fg(char **args);
int internal_bg(char **args);
void imprimir_prompt();

int main(){
    char line[COMMAND_LINE_SIZE];  
    while(true){
        if(read_line(line)){
            execute_line(line);
            fflush(stdout);
        }
    }
}

void imprimir_prompt(){
    user = getenv("USER");
    home = getenv("HOME");

    char cwd[COMMAND_LINE_SIZE];
    if(getcwd(cwd,COMMAND_LINE_SIZE)!=NULL){
        printf(BLANCO_T NEGRITA"%s:"RESET,user);
        printf(CYAN_T "~%s" RESET, cwd);
        printf(BLANCO_T"%c ",PROMPT);
    }else{
        perror("getcwd() error");
        
    }
   
}

char *read_line(char *line){

    imprimir_prompt();
    
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
            printf(GRIS_T NEGRITA"\nsaliendo del minishell...\n");
            exit(0);
        }
    }

    fflush(stdin);
    return line; 
}

int parse_args(char **args,char *line){
    int res = 0;
    char *token;
    token = strtok(line, " \t\n\r");

    while(token != NULL){
        args[res] = token;
        if(args[res][0] != '#'){
            token = strtok(NULL," \t\n\r");
            res++;           
        }else{
            token = NULL;
        }
    }
    args[res] = NULL;

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

int cd_avanzado(char **args){

    char *token = args[1];

    if(strchr(token,92) != NULL){ //Miramos si hay " \ " para substituirla por un espacio
        int i = 0;
        while(i < strlen(token)){
            if(token[i] == 92){
                token[i] = ' ';
            }
            i++;
        }
    }else if(strchr(token,34) != NULL){ //Miramos si hay " " para coger lo de dentro y que se junte en un solo token
        char *token2;
        token2 = args[2];
        if(token[strlen(token) - 1] == 34){ //Si solo hay una palabra dentro de las " "
            int i = 0;
            while(i < strlen(token)){
                if(i == (strlen(token) - 2)){
                    token[i] = '\0';
                }else{
                    token[i] = token[i+1];
                }
                i++;
            }
        }else if(token2 == NULL){ //Si solo hay una "
            fprintf(stderr,ROJO_T "ERROR\n" RESET);
        }else{
            int cont = 3;
            while(strchr(token2,34) == NULL){
                token[strlen(token)] = ' ';
                token2 = args[cont];
                cont++;
               
            }
            token[strlen(token)] = ' ';
            int i = 0;
            while(i < strlen(token)){
                if(i == (strlen(token) - 2)){
                    token[i] = '\0';
                }else{
                    token[i] = token[i+1];
                }
                i++;
            }
        }
           

    }else if(strchr(token,39) != NULL){ //Miramos si hay ' ' para coger lo de dentro y que se junte en un solo token
        char *token2;
        token2 = args[2];
        if(token[strlen(token) - 1] == 39){ //Si solo hay una palabra dentro de las ' '
            int i = 0;
            while(i < strlen(token)){
                if(i == (strlen(token) - 2)){
                    token[i] = '\0';
                }else{
                    token[i] = token[i+1];
                }
                i++;
            }
        }else if(token2 == NULL){ //Si solo hay una '
            fprintf(stderr,ROJO_T "ERROR\n" RESET);
        }else{
            int cont = 3;
            while(strchr(token2,39) == NULL){
                token[strlen(token)] = ' ';
                token2 = args[cont];
                cont++;
               
            }
            token[strlen(token)] = ' ';
            int i = 0;
            while(i < strlen(token)){
                if(i == (strlen(token) - 2)){
                    token[i] = '\0';
                }else{
                    token[i] = token[i+1];
                }
                i++;
            }
        }

    }
}

int internal_cd(char **args){
    
    if(args[1] == NULL){
        if(chdir("/home") != 0){
            perror("chdir()");
        }
    }else {
        cd_avanzado(args);
        if(chdir(args[1]) != 0){
            perror("chdir()");   
        }
    }
    
    char cwd[COMMAND_LINE_SIZE];
    if (getcwd(cwd, COMMAND_LINE_SIZE) != NULL) {
        fprintf(stderr, GRIS_T "[internal_cd()→ PWD: %s\n" RESET, cwd);
    } else {
        perror("getcwd() error\n");
        return FAILURE;
    }
    return SUCCES;
}

int internal_export(char **args)
{
    const char s[2] = "=";
    char *token;
    char *aux = args[1];

    char *nombre = NULL;
    char *valor = NULL;
    int cont = 0;
    token = strtok(aux,s);

    while(token != NULL){
        cont++;
        if (cont == 1){
            nombre = token;
        }else{
            valor = token;
        }
        token = strtok(NULL,s);
    }

    if(valor != NULL && nombre != NULL){
        fprintf(stderr,GRIS_T"[internal_export()→ nombre: %s\n"RESET,nombre);
        fprintf(stderr,GRIS_T"[internal_export()→ valor: %s\n"RESET,valor);
        if (getenv(nombre) != NULL){
            fprintf(stderr,GRIS_T "[internal_export()→ antiguo valor para USER: %s\n"RESET,getenv(nombre));
            setenv(nombre,valor,1);
            fprintf(stderr,GRIS_T "[internal_export()→ nuevo valor para USER: %s\n"RESET,getenv(nombre));
            return SUCCES;
        }else{
            fprintf(stderr,ROJO_T "Error: Nombre no existente\n"RESET);
            return FAILURE;
        }
    }else if(valor != NULL || nombre != NULL){
        fprintf(stderr,GRIS_T"[internal_export()→ nombre: %s\n"RESET,nombre);
        fprintf(stderr,GRIS_T"[internal_export()→ valor: %s\n"RESET,valor);
        fprintf(stderr,ROJO_T "Error de sintaxis. Uso: export Nombre=Valor \n"RESET);
        return FAILURE;
    }else{
        fprintf(stderr,ROJO_T "Error de sintaxis. Uso: export Nombre=Valor \n"RESET);
        return FAILURE;
    }
    
    //fprintf(stderr, GRIS_T "[internal_export()→ EEsta función asignará valores a variables de entorno\n"RESET);
}

int internal_source(char **args)
{

    fprintf(stderr, GRIS_T "[internal_source()→ Esta función ejecutará un fichero de líneas de comandos]\n" RESET);

    return -1;
}

int internal_jobs(char **args)
{

    fprintf(stderr, GRIS_T "[internal_jobs()→ Esta función mostrará el PID de los procesos que no estén en foreground]\n" RESET);

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
