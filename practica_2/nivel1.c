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

char const PROMPT = '$';
char *user;
char *home;

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

//Función empleada para la impresión del prompt
void imprimir_prompt(){ 
    //queda implementar el CWD en niveles posteriores
    user = getenv("USER");
    home = getenv("HOME");

    printf(BLANCO_T NEGRITA"%s:"RESET,user);
    printf(CYAN_T "~%s" RESET, home);
    printf(BLANCO_T"%c ",PROMPT);

}

/*---------------------------------------------------------------------------------------------------------
* Función encargada de la lectura del flujo de entrada de la consola, se implementa
* la salida del minishell mediante CTRL + D.
* Input:   stdin
* Output:  Puntero a la línea leída
---------------------------------------------------------------------------------------------------------*/
char *read_line(char *line){ //jordi

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
        printf(GRIS_T NEGRITA"Token %i: %s\n",index,args[index]);
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
    printf(GRIS_T NEGRITA"Token %i: %s\n",index,args[index]);
    //printf(GRIS_T NEGRITA"Numero total de tokens: %i\n",index);

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
* Estas funciones que contienen internal, de momento solo imprime lo que va a hacer en un futuro, por
* eso no se hace nada dentro. Más adelante veremos como estas funciones se van implementando
* Input:    args: array que contiene la línea escrita por consola dividida por tokens
* Output:   De momento devuenve un entero (1)
---------------------------------------------------------------------------------------------------------*/

int internal_cd(char **args){
    fprintf(stderr, GRIS_T "[internal_cd()→ Esta función cambiará de directorio]\n" RESET);
    return 1;
}

int internal_export(char **args)
{

    fprintf(stderr, GRIS_T "[internal_export()→ Esta función asignará valores a variables de entorno\n" RESET);
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

/*---------------------------------------------------------------------------------------------------------
* Esta función es la encargada de trasnsformar la línea de comando en un array de tokens y ejecutar la
* instrucción.
* Input:    line: String que contiene la línea introducida por comando 
* Output:   0
---------------------------------------------------------------------------------------------------------*/

int execute_line(char *line){ //pau
    char *args[ARGS_SIZE];
    int num_tokens;
    int interno;
    num_tokens = parse_args(args, line);
    interno = check_internal(args);
    return 0;
}
