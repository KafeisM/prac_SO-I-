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

#include <signal.h>
#include <sys/types.h>
#include <stdlib.h>
#include <stdio.h>
#include <stdbool.h>
#include <string.h>
#include <unistd.h>
#include <sys/wait.h>

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
void ctrlc(int signum);
void ctrlz(int signum);
void reaper(int signum);
int jobs_list_find(pid_t pid);
int  jobs_list_remove(int pos);
int jobs_list_add(pid_t pid,char status, char *cmd);


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

//cantidad de trabajos (en background y/o detenidos) que hay en jobs_list[ ]
int n_pids = 0;


int main(int argc, char *argv[]){

    char line[COMMAND_LINE_SIZE];  

    //associar señales con sus funciones
    signal(SIGCHLD,reaper);
    signal(SIGINT,ctrlc);
    signal (SIGTSTP, ctrlz);

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

int is_background(char **args){
    for (int i=0; i < ARGS_SIZE; i++){
        if (args[i] == "&"){
            args[i] = NULL;
            return 1;
        }
    }
    return 0;
}

void reaper(int signum){
    signal(SIGCHLD,reaper);
    pid_t ended;
    int status;

    fprintf(stderr,GRIS_T"[reaper()→ Recibida señal %d (SIGCHLD)]\n"RESET, signum); // la señal 17 es SIGCHILD
          
    while ((ended=waitpid(-1, &status , WNOHANG))>0) {
        //if ended es el pid del hijo en primer plano
        if (ended == jobs_list[0].pid){ //foreground
            fprintf(stderr,GRIS_T"[reaper()→ Proceso hijo %d en foreground (%s) finalizado por la señal %d]\n"RESET,ended,jobs_list[0].cmd,status);
            jobs_list[0].pid = 0;
            jobs_list[0].status = 'F';
            memset(jobs_list[0].cmd,'\0',COMMAND_LINE_SIZE);
        }else{ //background
            int pos = jobs_list_find(ended);
            fprintf(stderr,GRIS_T"[reaper()→ Proceso hijo %d en background (%s) finalizado por la señal %d]\n"RESET,ended,jobs_list[0].cmd,status);
            jobs_list_remove(pos);
        }

    }  


}

void ctrlc(int signum){
    signal(SIGINT, ctrlc);
    printf("\n");
    fprintf(stderr,GRIS_T"[ctrlc()--> soy el proceso con PID %d (%s) "RESET,getpid(),mi_shell);
    if(jobs_list[0].pid > 0){
        if(strcmp(jobs_list[0].cmd,"./nivel4") != 0){
           fprintf(stderr,GRIS_T"el proceso foreground es %d (%s) \n"RESET,jobs_list[0].pid,jobs_list[0].cmd);
            fprintf(stderr,GRIS_T"[ctrlc()→ recibida señal 2 (SIGINT)]");
            kill(jobs_list[0].pid,SIGTERM);
            fprintf(stderr,GRIS_T"[ctrlc()--> Señal 15 enviada a %d (%s) por %d (%s)"RESET,jobs_list[0].pid,jobs_list[0].cmd,getpid(),mi_shell);
        }else{
            fprintf(stderr,GRIS_T"[ctrlc()--> Señal 15 NO enviada a %d (%s) debido a que su proceso en foreground es el shell"RESET,getpid(),mi_shell);
        }
    }else{
        fprintf(stderr,GRIS_T"\n[ctrlc()--> Señal 15 NO enviada por %d (%s) debido a que no hay proceso en foreground"RESET,getpid(),mi_shell);
    }
    printf("\n");
    fflush(stdout);
}

void ctrlz(int signum){
    signal (SIGTSTP, ctrlz);
    printf("\n");
    fprintf(stderr,GRIS_T"[ctrlz()--> soy el proceso con PID %d (%s) "RESET,getpid(),mi_shell);
    if(jobs_list[0].pid > 0){
        if(strcmp(jobs_list[0].cmd,"./nivel4") != 0){
            fprintf(stderr,GRIS_T"el proceso foreground es %d (%s)] \n"RESET,jobs_list[0].pid,jobs_list[0].cmd);
            fprintf(stderr,GRIS_T"[ctrlz()→ recibida señal 20 (SIGTSTP)]");
            kill(jobs_list[0].pid,SIGSTOP);
            jobs_list[jobs_list_find(jobs_list[0].pid)].status = 'D';
            jobs_list_add(jobs_list[0].pid,jobs_list[0].status,jobs_list[0].cmd);
            jobs_list[0].pid = 0;
            jobs_list[0].status = 'N';
            memset(jobs_list[0].cmd,'\0',COMMAND_LINE_SIZE);
            fprintf(stderr,GRIS_T"[ctrlz()--> Señal 19 (SIGSTOP) enviada a %d (%s) por %d (%s)"RESET,jobs_list[0].pid,jobs_list[0].cmd,getpid(),mi_shell);
        }else{
            fprintf(stderr,GRIS_T"[ctrlz()--> Señal SIGSTOP NO enviada a %d (%s) debido a que su proceso en foreground es el shell"RESET,getpid(),mi_shell);
        }
    }else{ 
        fprintf(stderr,GRIS_T"\n[ctrlz()--> Señal SIGSTOP NO enviada por %d (%s) debido a que no hay proceso en foreground"RESET,getpid(),mi_shell);
    }
    printf("\n");
    fflush(stdout);


}

int jobs_list_add(pid_t pid,char status, char *cmd){
    n_pids++;

    if(n_pids < N_JOBS){
        jobs_list[n_pids].status = status;
        jobs_list[n_pids].pid = pid;
        strcpy(jobs_list[n_pids].cmd,cmd);
    }
}

int jobs_list_find(pid_t pid){
    int final;
    bool trobat = false;
    for (size_t i = 0; (!final) && (i < n_pids); i++){
        if(jobs_list[i].pid == pid){
            final = i;
            trobat = true;
        }
    }
    
    return final;
}

int  jobs_list_remove(int pos){
    jobs_list[pos] = jobs_list[n_pids - 1];
    jobs_list[n_pids-1].pid = 0;
    jobs_list[n_pids-1].status = '\0';
    memset(jobs_list[n_pids-1].cmd,'\0',COMMAND_LINE_SIZE);
    n_pids--;
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
    sleep(0.4);
    fflush(stdout);
   
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
    return line; 
}

int parse_args(char **args,char *line){

    int res = 0;
    char *token;
    char *aux;
    token = strtok(line, " \t\n\r");

    while(token != NULL){
        args[res] = token;
       // printf("ARGS %i : %s\n",res,args[res]);
        if(args[res][0] != '#'){
            token = strtok(NULL," \t\n\r");
            res++;           
        }else{
            token = NULL;
        }
       // printf("ARGS %i: %s\n",res,args[res]);
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
    }else if(strcmp(args[0],"^C")==0){
        return 0;
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
        fprintf(stderr,"[internal_export()→ nombre: %s\n"RESET,nombre);
        fprintf(stderr,"[internal_export()→ valor: %s\n"RESET,valor);
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
        fprintf(stderr,"[internal_export()→ nombre: %s\n"RESET,nombre);
        fprintf(stderr,"[internal_export()→ valor: %s\n"RESET,valor);
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
    char *str[COMMAND_LINE_SIZE];

    FILE *fp = fopen(args[1],"r");//r porq queremos solo leer
    if( fp == NULL ) {
      fprintf(stderr, ROJO_T "Error de sintaxis. Uso: source <nombre fichero>\n"RESET);
      return(-1);
    }

    while( fgets (*str, COMMAND_LINE_SIZE, fp)!=NULL ) {
      for (size_t i = 0; i < COMMAND_LINE_SIZE; i++){
        if(*str[i] == '\n'){
            *str[i] = '\0';
        }
      }
      fflush(fp);
      execute_line(*str);
      
    }
    fclose(fp);

    return -1;
}

int internal_jobs(char **args)
{

    for(int i = 1; i<= n_pids; i++){
        printf("[%d] %d     %c      %s\n",i,jobs_list[i].pid,jobs_list[i].status,jobs_list[i].cmd);
    }

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

    char lineaux[strlen(line)+1];
    strcpy(lineaux,line);
    int status;
    char *args[ARGS_SIZE];
    int num_tokens;
    int interno;
    num_tokens = parse_args(args, line);
    interno = check_internal(args);
    fprintf("interno: %i\n",interno);

    if (num_tokens > 0){
        if (interno == 0){
        bool isbg = is_background(args);
        if(isbg){
            strcpy(jobs_list[0].cmd,lineaux);
            jobs_list[0].status = 'E';
        }
        pid_t id = fork();
        if (id > 0){
            if(isbg){
                signal(SIGINT,ctrlc);
                fprintf(stderr,GRIS_T"[execute_line(): PID padre: %d | (%s)]\n"RESET,getpid(),mi_shell);
                //fprintf(stderr,"args[0]: %s\n",args[0]);
                jobs_list[0].pid = id;
            }else{
                jobs_list_add(getpid(),'E',lineaux);
            } 
        }else if(id == 0){
            signal(SIGCHLD,SIG_DFL);
            signal(SIGINT,SIG_IGN);
            signal(SIGTSTP, SIG_IGN);
            fprintf(stderr,GRIS_T"[execute_line(): PID hijo: %d | (%s)]\n"RESET,getpid(),jobs_list[0].cmd);
            int err = execvp(args[0],args);
            if (err == -1){
                exit(-1);
            }
        }else{
            fprintf(stderr,ROJO_T"Error con la creación del hijo\n"RESET);
            exit(-1);
        }

        while(jobs_list[0].pid > 0){
            pause();
        }
        
    }
    }
    
}