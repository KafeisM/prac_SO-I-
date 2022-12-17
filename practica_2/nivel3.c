/*PRÀCTICA 2 SISTEMAS OPERATIVOS
-JORDI FLORIT ENSENYAT
-PAU GIRÓN RODRÍGUEZ
-JOSEP GABRIEL FORNÉS REYNÉS*/

//constantes
#define _POSIX_C_SOURCE 200112L
#define COMMAND_LINE_SIZE 1024
#define ARGS_SIZE 64
#define N_JOBS 64

//colores para prompt, errores e includes de librerias
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
#include <sys/types.h>
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

//TABLA INFORMACIÓN DE LOS TRABAJOS
/*Este struct es una estructira de datos que nos sirve para poder tener toda la informacion de los
trabajos actuales del programa. Contiene la siguiente información:
-pid: es el identificador de proceso
-status: nos indica el estado del proceso ‘N’, ’E’, ‘D’, ‘F’ (‘N’: ninguno, ‘E’: Ejecutándose y ‘D’: Detenido, ‘F’: Finalizado) 
-cmd: linea de comando introducida por el usuario*/
struct info_job {
   pid_t pid;
   char status; 
   char cmd[COMMAND_LINE_SIZE]; 
};

//variables para control procesos
static char mi_shell[COMMAND_LINE_SIZE]; 
static struct info_job jobs_list [N_JOBS];

//variables para el prompt
char const PROMPT = '$';
char *user;
char *home;


int main(int argc, char *argv[]){ //jordi

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
        return line;
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

/*---------------------------------------------------------------------------------------------------------
* Función encargada de trocear la línea obtenida en tokens usando como separadores los espacios,
* tabuladores y saltos de línea. Si el primer carácter de un token es '#' obviaremos el resto de elementos.
* Input:    args: array que contendrá la línea escrita por consola dividida por tokens
*           line: línea que contiene la línea escrita por consola
* Output:   Número de tokens creados
---------------------------------------------------------------------------------------------------------*/

int parse_args(char **args,char *line){

    int index = 0;
    char *token;

    //cogemos el primer token
    token = strtok(line, " \t\n\r"); 

    while(token != NULL){
        //añadimos el token al array
        args[index] = token;
        //printf(GRIS_T NEGRITA"Token %i: %s\n",index,args[index]);
        if(args[index][0] != '#'){
            //si el primer carácter del token no es '#' seguimos 
            //cogemos el siguiente token
            token = strtok(NULL," \t\n\r");
            index++;           
        }else{
            //sino acabamos el proceso
            token = NULL;
        }
    }
    //último elemento del array es NULL
    args[index] = NULL;

    return index;

}

/*---------------------------------------------------------------------------------------------------------
* Función que mira si en args[0] hay un comando interno, si es así, nos va a llevar a la función 
* que pertenece dicho token
* Input:    args: array que contiene la línea escrita por consola dividida por tokens
* Output:   Valor entero donde 1 significa que hay comando interno y 0 que no.
---------------------------------------------------------------------------------------------------------*/

int check_internal(char **args){
    if(args[0] == NULL){ //Por si no hay elementos dentro de args[0]
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
    }else if(strcmp(args[0],"exit")== 0){ //En el caso que escribamos exit, va a salir del minishell
        exit(0);
    }else{  
        //printf("No es un comando interno\n");
        return 0;
    }
}

/*---------------------------------------------------------------------------------------------------------
* Ampliación de la función interna_cd, que trata los directorios que van seguidos del comando
* CD cuyos nombres llevan espacios. Estos directorios con espacios deben de ir:
* cd "directorio 1"
* cd 'directorio 2'
* cd directorio\3
* Así que esta función detecta alguno de los tres casos y los trata como un único token
* Input:    args: array que contiene la línea escrita por consola dividida por tokens
* Output:   -
---------------------------------------------------------------------------------------------------------*/

int cd_avanzado(char **args){ 

    char *token = args[1];

    if(strchr(token,92) != NULL){ //Miramos si hay " \ " para substituirla por un espacio
        int i = 0;
        while(i < strlen(token)){
            if(token[i] == 92){ //Si la encontramos la substituimos por un espacio
                token[i] = ' ';
            }
            i++;
        }
    }else if(strchr(token,34) != NULL){ //Miramos si hay " " para coger lo de dentro y que se junte en un solo token
        char *token2;
        token2 = args[2];
        if(token[strlen(token) - 1] == 34){ //Si solo hay una palabra dentro de las " " solo las quitamos
            int i = 0;
            while(i < strlen(token)){
                if(i == (strlen(token) - 2)){
                    token[i] = '\0';
                }else{
                    token[i] = token[i+1];
                }
                i++;
            }
        }else if(token2 == NULL){ //Si solo hay una ", significa que ha habído un error
            fprintf(stderr,ROJO_T "ERROR\n" RESET);
        }else{//Hay más palabras dentro de " ", así que se van passando las palabras hasta encontrar el segundo "
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
        if(token[strlen(token) - 1] == 39){ //Si solo hay una palabra dentro de las ' ' solo las quitamos
            int i = 0;
            while(i < strlen(token)){
                if(i == (strlen(token) - 2)){
                    token[i] = '\0';
                }else{
                    token[i] = token[i+1];
                }
                i++;
            }
        }else if(token2 == NULL){ //Si solo hay una ', significa que ha habido un error
            fprintf(stderr,ROJO_T "ERROR\n" RESET);
        }else{ //Hay más palabras dentro de ' ', así que se van passando las palabras hasta encontrar el segundo '
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

/*---------------------------------------------------------------------------------------------------------
* Función que cambiará al directorio que se introduce seguido del comando. Si el comando
* se introduce solo, se translada al HOME
* Input:    args: array que contiene la línea escrita por consola dividida por tokens
* Output:   FAILURE (-1): ha habído error
            SUCCES (0): ha ido bien
---------------------------------------------------------------------------------------------------------*/

int internal_cd(char **args){
    
    if(args[1] == NULL){    //Miramos si el comando CD va solo, y si es así canviamos el directorio a HOME
        if(chdir("/home") != 0){
            perror("chdir()");
        }
    }else { //sino llamamos al cd avanzado por si hay " ", ' ' o \ y cambiamos el directorio al que hemos indicado
        cd_avanzado(args);
        if(chdir(args[1]) != 0){    //Si no está el directorio indicado salta un error
            perror("chdir()");   
        }
    }
    
    //Imprimimos el prompt
    char cwd[COMMAND_LINE_SIZE];
    if (getcwd(cwd, COMMAND_LINE_SIZE) != NULL) {
        fprintf(stderr, GRIS_T "[internal_cd()→ PWD: %s\n" RESET, cwd);
    } else {
        perror("getcwd() error\n");
        return FAILURE;
    }
    return SUCCES;
}

/*---------------------------------------------------------------------------------------------------------
* Función encargada de asignar un valor a una variable de entorno. Analizamos el segundo token del array
* de argumentos y si se puede realizar la asignación, se hace.
* Input:    args: array que contendrá la línea escrita por consola dividida por tokens
* Output:   confirmación de la operación (SUCCES, FAILURE)
---------------------------------------------------------------------------------------------------------*/

int internal_export(char **args)
{

    //variables para trocear el argumento a analizar
    const char s[2] = "=";
    char *token;
    char *aux = args[1];
    //variables auxiliares
    char *nombre = NULL;
    char *valor = NULL;
    int cont = 0;
    //lectura del primer token
    token = strtok(aux,s);

    //asignación del nombre y del valor.
    while(token != NULL){
        cont++;
        if (cont == 1){
            nombre = token;
        }else{
            valor = token;
        }
        token = strtok(NULL," ");
    }

    //mostramos por pantalla el resultado de la operación
    if(valor != NULL && nombre != NULL){ //si ambas variables están llenas, entonces
        //mostramos el valor de ambas variables
        fprintf(stderr,GRIS_T"[internal_export()→ nombre: %s\n"RESET,nombre);
        fprintf(stderr,GRIS_T"[internal_export()→ valor: %s\n"RESET,valor);
        if (getenv(nombre) != NULL){ //si existe la variable de entorno
            //Se muestra el antiguo valor de este
            fprintf(stderr,GRIS_T "[internal_export()→ antiguo valor para USER: %s\n"RESET,getenv(nombre));
            //Se cambia el valor
            setenv(nombre,valor,1);
            //Se muestra el nuevo valor
            fprintf(stderr,GRIS_T "[internal_export()→ nuevo valor para USER: %s\n"RESET,getenv(nombre));
            return SUCCES;
        }else{ //si no existe la variable de entorno
            fprintf(stderr,ROJO_T "Error: Nombre no existente\n"RESET);
            return FAILURE;
        }
    }else if(valor != NULL || nombre != NULL){//si una de las variables no está llena, entonces
        //mostramos el valor de ambas variables y el uso correcto de la función
        fprintf(stderr,GRIS_T"[internal_export()→ nombre: %s\n"RESET,nombre);
        fprintf(stderr,GRIS_T"[internal_export()→ valor: %s\n"RESET,valor);
        fprintf(stderr,ROJO_T "Error de sintaxis. Uso: export Nombre=Valor \n"RESET);
        return FAILURE;
    }else{ //si ninguna de las variables está llena mostramos el uso correcto de la función
        fprintf(stderr,ROJO_T "Error de sintaxis. Uso: export Nombre=Valor \n"RESET);
        return FAILURE;
    }
}

/*---------------------------------------------------------------------------------------------------------
* Función que provoca la ejecución de los comandos que están escritos dentro de un fichero
* Input:    args: array que contiene la línea escrita por consola dividida por tokens
* Output:   devuelve -1
---------------------------------------------------------------------------------------------------------*/

int internal_source(char **args)
{

    char str[COMMAND_LINE_SIZE];

    FILE *fp = fopen(args[1],"r");//Abrimos modo lectura del fichero pasado por el args[1]. r porq queremos solo leer
    if( fp == NULL ) {  //Control de error
      fprintf(stderr, ROJO_T "Error de sintaxis. Uso: source <nombre fichero>\n"RESET);
      return(-1);
    }

    //Mientras no haya llegado al final del fichero va leyendo linea por linea
    while( fgets (str, COMMAND_LINE_SIZE, fp)!=NULL ) {
        for (size_t i = 0; i < COMMAND_LINE_SIZE; i++){
            if(str[i] == '\n'){ //Substituimos el cambio de linea por \0
                str[i] = '\0';
            }
        }
        fprintf(stderr,"\n");
        fprintf(stderr, GRIS_T "[internal_source()→ LINE: %s]\n"RESET,str);
        fflush(fp);
        execute_line(str); //Enciamos la línea para que se ejecute
      
    }
    fclose(fp); //Cerramos fichero

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

/*---------------------------------------------------------------------------------------------------------
* Esta función es la encargada de trasnsformar la línea de comando en un array de tokens y ejecutar la
* instrucción. Implementamos la ejecución de comandos externos, con la ayuda de un proceso hijo.
* Input:    line: String que contiene la línea introducida por comando 
* Output:   0
---------------------------------------------------------------------------------------------------------*/

int execute_line(char *line){
    
    //línea auxiliar para no modificar la original
    char lineaux[strlen(line)+1];
    strcpy(lineaux,line);

    //estado de los procesos
    int status;

    char *args[ARGS_SIZE];
    int num_tokens;
    int interno;
    num_tokens = parse_args(args, line);
    //fprintf(stderr,"args[0]: %s\n",args[0]);

    if (num_tokens > 0){ //Si exiten argumentos, entonces
        if (check_internal(args) == 1){ //si es un comando interno se ejecuta la correspondiente función
            return 1;
        }else{ //Si es un comando externo, entonces

            //asignamos el comando y el estado al proceso padre
            strcpy(jobs_list[0].cmd,lineaux);
            jobs_list[0].status = 'E';

            //creamos un proceso hijo
            pid_t id = fork();

            if (id > 0){ //si es el padre
                //Mostramos su pid, y actualizamos el pid de jobs.list[0]
                fprintf(stderr,GRIS_T"[execute_line(): PID padre: %d | (%s)]\n"RESET,getpid(),mi_shell);
                jobs_list[0].pid = id;
            }else if(id == 0){ //si es el hijo
                //moatramos su pid y ejecutamos el comando interno controlando el error
                fprintf(stderr,GRIS_T"[execute_line(): PID hijo: %d | (%s)]\n"RESET,getpid(),jobs_list[0].cmd);
                int err = execvp(args[0],args);
                if (err == -1){
                    exit(-1);
                }
            }else{ //si se produce algún error se muestra
                fprintf(stderr,ROJO_T"Error con la creación del hijo\n"RESET);
                exit(-2);
            }

            //esperar a que el hijo finalice su operación
            wait(&status);

            //mostramos la finalización del proceso hijo junto a sus estados
            if (WIFEXITED(status)){
                int statuscode = WEXITSTATUS(status);
                if (statuscode == 0){
                    fprintf(stderr,GRIS_T"[execute_line(): Proceso hijo %d (cmd: %s) finalizado con exit() con estado: %d]\n"RESET,jobs_list[0].pid,jobs_list[0].cmd,status);
                }else{
                    fprintf(stderr,ROJO_T"(%s): no se encontró la orden\n"RESET,jobs_list[0].cmd);
                    fprintf(stderr,GRIS_T"[execute_line(): Proceso hijo %d (cmd: %s) finalizado con exit() con estado: %d]\n"RESET,jobs_list[0].pid,jobs_list[0].cmd,statuscode);
                }
            }
            //reset de los datos del primer proceso
            jobs_list[0].status = 'N';
            strcpy(jobs_list[0].cmd,"");
            jobs_list[0].pid = 0;
        }
    }
    
}