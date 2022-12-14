/*PRÀCTICA 2 SISTEMAS OPERATIVOS
-JORDI FLORIT ENSENYAT
-PAU GIRÓN RODRÍGUEZ
-JOSEP GABRIEL FORNÉS REYNÉS*/

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

/*El main del programa se lee de manera continuea las líneas de comandos introducidad
para posteriormente ser tratadas (las lineas son troceadas en tokens) y ejecutadas (no implementado en el nivel 1)
mediante el execute_line*/
int main(){ 
    char line[COMMAND_LINE_SIZE];  
    while(true){
        if(read_line(line)){
            execute_line(line);
            fflush(stdout);
        }
    }
}

/*Función empleada para la impresión del prompt en este nivel ya queda implemantado
en su forma final*/
void imprimir_prompt(){
    //obtenemos gracias a llamadas al sistea el USER y el HOME
    user = getenv("USER");
    home = getenv("HOME");

    //anidamos todo correctamente para obtener nuestro propio cwd (current working directory)
    char cwd[COMMAND_LINE_SIZE];
    if(getcwd(cwd,COMMAND_LINE_SIZE)!=NULL){
        printf(BLANCO_T NEGRITA"%s:"RESET,user);
        printf(CYAN_T "~%s" RESET, cwd);
        printf(BLANCO_T"%c ",PROMPT);
    }else{
        perror("getcwd() error");
        
    }
   
}

/*---------------------------------------------------------------------------------------------------------
* Función encargada de la lectura del flujo de entrada de la consola, se implementa
* la salida del minishell mediante CTRL + D.
* Input:   stdin
* Output:  Puntero a la línea leída
---------------------------------------------------------------------------------------------------------*/
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
*           SUCCES (0): ha ido bien
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

int internal_export(char **args){ 

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

/*---------------------------------------------------------------------------------------------------------
* Esta función es la encargada de trasnsformar la línea de comando en un array de tokens y ejecutar la
* instrucción.
* Input:    line: String que contiene la línea introducida por comando 
* Output:   0
---------------------------------------------------------------------------------------------------------*/

int execute_line(char *line){ 

    char *args[ARGS_SIZE];
    int num_tokens;
    int interno;
    num_tokens = parse_args(args, line);
    interno = check_internal(args);
}
