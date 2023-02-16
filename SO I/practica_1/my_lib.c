/*PRÀCTICA 1 SISTEMAS OPERATIVOS
-JORDI FLORIT ENSENYAT
-PAU GIRÓN RODRÍGUEZ
-JOSEP GABRIEL FORNÉS REYNÉS*/


#include <stdio.h>
#include <stdlib.h>
#include "my_lib.h"

// FUNCIONES STRINGS -------------------------------------------------------------------------------------

/*---------------------------------------------------------------------------------------------------------
* Devuelve la longitud del string pasado por parámetro  
* Input:    str: puntero a la cadena de texto
* Output:   Valor entero con la lognitud de la cadena 
---------------------------------------------------------------------------------------------------------*/

size_t my_strlen(const char *str){
    
    //variables resultado y contador de caracteres como indice del string
    long res = 0;
    int cont = 0;

    //caracter auxiliar que iremos analizando
    char aux = str[cont];
    while(aux!='\0'){

        //aumentamos el resultado en 1 si el caracter no es un espacio
        if(aux!=' '){
            res++;
        }
        //incrementamos el índice y modificamos el caracter auxiliar
        cont++;
        aux = str[cont];
    }
    return res;
}

/*---------------------------------------------------------------------------------------------------------
* Escanea la cadena apuntada por str buscando la primera ocurrencia del carácter c
* Input:    str: apunta a la cadena a escanear | c : es el carácter buscado.
* Output:   Devuelve el puntero a la primera ocurrencia del carácter c en la cadena str o 
* NULL si el carácter no se encuentra
---------------------------------------------------------------------------------------------------------*/

char *my_strchr(const char *str, int c){

    //recorremos el string con el puntero
    while (*str != '\0'){

        //si encontramos el caracter devolvemos lo que falta de string
        if (*str == c){
            return (char *)str;
        }
        str++;
    }

    return NULL;
}

/*---------------------------------------------------------------------------------------------------------
* Compara dos cadenas de texto pasadas por parámetro
* Input:    str1: apunta a la 1ª cadena a comparar | str2: apunta a la 2ª cadena a comparar.
* Output:   Valor entero que asigna:
*            retun > 0 si str1 > str2
*            retun < 0 si str2 > str1
*            retun = 0 si str1 = str2
---------------------------------------------------------------------------------------------------------*/

int my_strcmp(const char *str1, const char *str2){
    
    //mientras no final de cadena y los elemntos sean iguales, seguir avanzando
  
    while (*str1 && (*str1 == *str2))
    {
        str1++;
        str2++;
    }

    return *str1 - *str2;
}

/*---------------------------------------------------------------------------------------------------------
* Copia la cadena apuntada por src en el espacio de memoria apuntado por dest, después de vaciarlo.
* Input:    dest: puntero a la cadena destino | src: puntero a la cadena origen
* Output:   Puntero a la cadena destino 
---------------------------------------------------------------------------------------------------------*/

char *my_strcpy(char *dest, const char *src){

    //indice para recorrer los conjuntos de char's
    int aux= 0;

    //vaciamos dest
    while(dest[aux] != '\0'){
        dest[aux++] = '\0';
    }

    aux = 0;

    //pasamos todos los caracteres de src a dest
    while (src[aux] != '\0')
    {
        dest[aux] = src[aux];
        aux++;
    }

    return dest;
}

/*---------------------------------------------------------------------------------------------------------
* Copia los caracteres indicados por parámetro de la cadena apuntada por src en la cadena apuntada por dest
* Rellenar con 0's en los carácteres que sobren.
* Input:    dest: puntero a la cadena destino | src: puntero a la cadena origen | n: nº de carácteres que
*           copiamos en dest
* Output:   puntero a la cadena destino 
---------------------------------------------------------------------------------------------------------*/

char *my_strncpy(char *dest, const char *src, size_t n){

    //obtenemos la longitud de src
    int len = my_strlen(src);
    
    //separamos el problema en los dos posibles casos
    if (len < n)
    {
        //para un tamaño mayor que el de *src:

        //ponemos el contenido de *src en *dest y llenamos
        //lo restante de 0's
        for (int i = 0; i < len; i++)
        {
            dest[i] = src[i];
        }
        for (int i = len; i < n; i++)
        {
            dest[i] = '\0';
        }
    }
    else
    {
        //para un tamaño menor o igual que el de *src

        //añadimos los n primeros caracteres de *src en *dest
        for (int i = 0; i < n; i++)
        {
            dest[i] = src[i];
        }
    }
    return dest;
}

/*---------------------------------------------------------------------------------------------------------
* Se pasan 2 cadenas por parametro y las concatena, la cadena src la añadire al final de dest
* Input:    dest: es el puntero a la cadena destino | src: apunta a la cadena a concatenar.
* Output:   puntero a la cadena destino 
---------------------------------------------------------------------------------------------------------*/

char *my_strcat(char *dest, const char *src){

    //variables para la posición string destino y la fuente  
    int posD = 0;
    int posS = 0;
    
    //ajustar posiciones y concatenar strings
    while(dest[posD] != '\0'){
        posD++;
    }

    while(src[posS] != '\0'){
        dest[posD++] = src[posS++];
    }
    
    //añadir final string
    dest[posD] = '\0';

    return dest;
}

// ---------- FUNCIONES GESTOR PILA -------------------------------------------------------------------------

/*---------------------------------------------------------------------------------------------------------
* Reserva espacio para una variable de tipo "struct my_stack", que contendrá el puntero top, que apuntará
* al nodo superior de la pila, y el tamaño de los datos size.
* Input:    size: tamaño de los datos
* Output:   puntero a la pila inicializada
---------------------------------------------------------------------------------------------------------*/

struct my_stack *my_stack_init(int size){

    //stack resultado
    struct my_stack *stack;

    //reserva de memoria e inicialización de sus elementos
    stack = malloc(sizeof(struct my_stack));

    if(stack == NULL){
        printf("NO SE PUEDE RESERVAR MEMORIA\n");
    }

    stack->size = size;
    stack->top = NULL;

    return stack;
}

/*---------------------------------------------------------------------------------------------------------
* Inserta un nuevo nodo a la pila dada.
* Input:    stack: puntero de la pila  | data: puntero a los datos del nodo
* Output:   0 si ha ido bien, o -1 si no.
---------------------------------------------------------------------------------------------------------*/

int my_stack_push (struct my_stack *stack, void *data){

    //comprovación si existe la pila 
    if(stack == NULL){
        printf("PILA NO EXISTENTE\n");
        return -1;
    }

    //nodo que se añadirá a la pila
    struct my_stack_node *node;

    //control de pila vacía
    if(stack == NULL || stack->size < 0){
        return -1;
    }else{

        //reserva de memoria para el nodo (y control error)
        node = malloc(stack->size);

        if(node == NULL){
            printf("ERROR PARA CREACION NUEVO NODO\n");
            return -1;
        }

        //introducción del nuevo nodo y ponemos el data
        node->next = stack->top;
        stack->top = node;
        node->data = data;
        
        return 0;
    }
}

/*---------------------------------------------------------------------------------------------------------
* Elimina el nodo superior de la pila dada y libera la memoria que ocupaba ese nodo
* Input:    stack: puntero de la pila
* Output:   puntero a los datos del elemento eliminado
---------------------------------------------------------------------------------------------------------*/

void *my_stack_pop (struct my_stack *stack){

    //comprovacion pila vacia
    if(stack->top == NULL){
        return NULL;
    }
    
    //nodo auxiliar
    struct my_stack_node *aux = NULL;

    //control de pila vacía y pop del nodo apuntado por top
    if (stack->top == NULL){
        return NULL;
    }else{
        aux = stack->top;
        stack->top = aux->next;
        return aux->data;
    }
    
}

/*---------------------------------------------------------------------------------------------------------
* Mide la longitud de la pila contando los nodos que hay
* Input:    stack: puntero de la pila
* Output:   número de nodos totales que hay en los elementos de la pila
---------------------------------------------------------------------------------------------------------*/

int my_stack_len (struct my_stack *stack){

    //nodo auxiliar y contador para el resultado
    struct my_stack_node *aux;
    int cont = 1;

    //control de pila vacía y bucle para aumentar el contador
    if(stack->top == NULL){
        return 0;
    }else{

        aux = stack->top;
        while(aux->next != NULL){
            cont++;
            aux = aux->next;
           
        }

        return cont;
    }

}

/*---------------------------------------------------------------------------------------------------------
* Libera la memoria que habíamos reservado para cada uno de los datos y la 
* de cada nodo. Además, libera también la memoria que ocupa el "struck my_stack".
* Input:    stack: puntero de la pila
* Output:   número de bytes liberados
---------------------------------------------------------------------------------------------------------*/

int my_stack_purge(struct my_stack *stack){

    //variable para bytes liberados, longitud pila y nodo auxiliar
    int num_mem = sizeof(struct my_stack);
    int len =  my_stack_len(stack);
    struct my_stack_node *aux;

    //control de pila vacía y liberación del stack y la memoria que ocupa
    if(stack->top == NULL){ 
        return num_mem;

    }else{

        //cálculo de bytes liberados e inicialización del nodo auxiliar
        num_mem = (len * sizeof(struct my_stack_node)) + (stack->size * len) + num_mem;   
        aux = stack->top;
        
        //bucle para la liberación de la pila
        while(aux->next != NULL){
            stack->top = aux->next;
            free(aux->data);
            free(aux);
            aux = stack->top;
        }    

        free(stack); 

    }

    return num_mem;
}

/*---------------------------------------------------------------------------------------------------------
* Almacena los datos de la pila en un fichero.
* Input:    stack: puntero de la pila | filname: puntero del fichero al qual tenemos que almacenar los datos
* Output:   número de elementos almacenados o, -1 si hubo error.
---------------------------------------------------------------------------------------------------------*/

int my_stack_write(struct my_stack *stack, char *filename){

    //variables para errores y el resultado
    int fd;
    int er;
    int res = 0;

    //error con pila
    if (stack == NULL) {
        printf("Ha habido un problema con la pila.\n");
        return -1;
    }

    //apertura y control de error
    fd = open(filename,O_WRONLY | O_CREAT | O_TRUNC, S_IRUSR | S_IWUSR);
    if (fd == -1) {
        perror("Ha habido un problema al abrir el fichero.\n");
        return -1;
    }

    //declaración e iniciliciación de un stack y nodo auxiliares
    struct my_stack* stack_aux = my_stack_init(stack->size); 
    struct my_stack_node* node_aux = stack->top;

    //volcado
    while (stack->top != NULL){
        my_stack_push(stack_aux, stack->top->data);
        stack->top = stack->top->next;
    }
    stack->top = node_aux;

    //escritura del size y control de error
    er = write(fd,&(stack->size),sizeof(stack->size));
    if (er == -1) {
        perror("Ha habido un problema al escribir en el fichero.\n");
        return -1;
    }

    //escritura de los data y control de error
    while (stack_aux->top != NULL) {
        er = write(fd,my_stack_pop(stack_aux),stack->size);
        if (er == -1) {
            perror("Ha habido un problema al escribir en el fichero.\n");
            return -1;
        }
        res++;
    }
    
    //cerrar fichero y control de error
    er = close(fd);
    if (er == -1) {
        perror("Ha habido un problema al cerrar el fichero.");
        return -1;
    }

    return res;
}

/*---------------------------------------------------------------------------------------------------------
* Lee los datos almacenados en el fitchero
* Input:    filname: puntero del fichero al qual tenemos que almacenar los datos
* Output:   puntero a la pila creada, y si hubo error, retorna NULL.
---------------------------------------------------------------------------------------------------------*/

struct my_stack *my_stack_read(char *filename){

    //variables para errores y auxiliares
    int fd;
    int er;
    int size_aux = 0;
    void* data;
    
    //apertura fichero y control de error
    fd = open(filename,O_RDONLY);
    if (fd == -1) {
        perror("Ha habido un problema al abrir el fichero.\n");
    }

    //lectura del size y control del error
    er = read(fd,&size_aux,sizeof(int)); 
    if (er == -1) {
        perror("Ha habido un problema al leer el fichero.\n");      
    }
    
    //stack auxiliar e inicialización con size leído antes
    struct my_stack *stack_aux = my_stack_init(size_aux);
    
    //lectura de todos los data
    while (er > 0) {

        //reservar espacio de memoria para cada data y control del error   
        data = malloc(stack_aux->size);
        if (data == NULL) {
            printf("Ha habido un problema al reservar memoria.\n");           
        }

        //lectura del data y control del error
        er = read(fd,data,stack_aux->size);
        if (er == -1) {
            perror("Ha habido un problema al leer el fichero.\n");
        }

        //push del data en la pila
        if (er > 0) {
            my_stack_push(stack_aux, data); 
        }       
    }

    //cerrar fichero y control del error   
    er = close(fd);
    if (er == -1) {
        perror("Error al cerrar el fichero!!!");       
    }
    
    return stack_aux;
    
}


