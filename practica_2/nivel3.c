#define _POSIX_C_SOURCE 200112L
#define COMMAND_LINE_SIZE 1024
#define ARGS_SIZE 64
#define N_JOBS 64

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

#include <stdlib.h>
#include <stdio.h>
#include <stdbool.h>
#include <string.h>
#include <unistd.h>

char *read_line(char *line); 
int execute_line(char *line);
int parse_args(char **args,char *line);
int internal_cd(char **args);
int internal_export(char **args);
int internal_source(char **args);
int internal_jobs(char **args);
int internal_fg(char **args);
int internal_bg(char **args);
void imprimir_prompt();

//tabla datos de los procesos 
struct info_job {
   pid_t pid;
   char status; // ‘N’, ’E’, ‘D’, ‘F’ (‘N’: ninguno, ‘E’: Ejecutándose y ‘D’: Detenido, ‘F’: Finalizado) 
   char cmd[COMMAND_LINE_SIZE]; // línea de comando asociada
};

//variable para control procesos
static char mi_shell[COMMAND_LINE_SIZE]; 
static struct info_job jobs_list [N_JOBS];

//variables para el prompt
char const PROMPT = '$';
char *user;
char *home;


int main(int argc, char *argv[]){

    char line[COMMAND_LINE_SIZE];  

    //inicializar datos tabla
    jobs_list[0].pid = 0;
    jobs_list[0].status = 'N';
    memset(jobs_list[0].cmd,'\0',COMMAND_LINE_SIZE);

    //obtener comando de ejecucción del minishell
    strcpy(mi_shell,argv[0]);

    //bucle principal
    while(true){
        if(read_line(line)){
            execute_line(line);
            fflush(stdout);
        }
    }
}

void imprimir_prompt(){
    //queda implementar el PWD en niveles posteriores
    user = getenv("USER");
    home = getenv("HOME");

    printf(BLANCO_T NEGRITA"%s:"RESET,user);
    printf(CYAN_T "~%s" RESET, home);
    printf(BLANCO_T"%c ",PROMPT);

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

    
    
    if(args[1] == NULL){
        if(chdir("/home") != 0){
            perror("chdir(): ");
        }
    }else{        

        if(chdir(args[1]) != 0){
            perror("chdir(): ");
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

    fprintf(stderr, GRIS_T "[internal_export()→ EEsta función asignará valores a variables de entorno\n" RESET);
    return 1;
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