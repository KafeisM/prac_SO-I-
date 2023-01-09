#include <stdio.h>
#include <stdlib.h>
#include "my_lib.h"
#include <pthread.h>
#include <stdbool.h>
#include <string.h>

int NUM_THREADS = 10;
int N = 1000000;
pthread_mutex_t mutex = PTHREAD_MUTEX_INITIALIZER;
struct my_stack *stack_aux;

#define COMMAND_LINE_SIZE 1024
#define ARGV_SIZE 64

void *worker(void *ptr);

int main(int argc,char *argv[]){ 

    pthread_t threads[NUM_THREADS];
    //int or_length,new_length;
    if(argv[1] == NULL){
        fprintf(stderr,"USAGE: ./stack_counters stack_file\n");
        exit(0);
    }

    stack_aux = my_stack_read(argv[1]);

    fprintf(stderr,"Threads: %i, Iterations: %i\n",NUM_THREADS,N);

    int *data = 0;
    
    if (stack_aux == NULL){
        //or_length = 0;
        stack_aux = my_stack_init(4);
        fprintf(stderr,"stack->size: %i \n",stack_aux->size);
        fprintf(stderr,"original stack length: %i \n",my_stack_len(stack_aux));
        for (int i = 0; i < NUM_THREADS; i++){
            data = malloc(4);
            my_stack_push(stack_aux,data);
        }
    }else if(my_stack_len(stack_aux) < NUM_THREADS){
        fprintf(stderr,"stack->size: %i \n",stack_aux->size);
        fprintf(stderr,"original stack length: %i \n",my_stack_len(stack_aux));
        //or_length = my_stack_len(stack_aux);
        for (int i = 0; i < (NUM_THREADS - my_stack_len(stack_aux)); i++){
            data = malloc(4);
            my_stack_push(stack_aux,data);
        }
        
        //my_stack_write(stack_aux,argv[1]);
    }else{
        fprintf(stderr,"stack->size: %i \n",stack_aux->size);
        fprintf(stderr,"original stack length: %i \n",my_stack_len(stack_aux));
    }
    
    fprintf(stderr,"new stack length: %i \n",my_stack_len(stack_aux));

    //my_stack_purge(stack_aux);


    for(int i = 0; i < NUM_THREADS; i++){
        pthread_create(&threads[i], NULL, worker, NULL);
        fprintf(stderr,"%i) Thread %lu created \n",i,threads[i]);
    }
    
    for(int i = 0; i < NUM_THREADS; i++){
        pthread_join(threads[i], NULL);
    }

    int write = my_stack_write(stack_aux,argv[1]);

    if (write == -1){
        fprintf(stderr,"Error escritura en el fichero\n");
    }else{
        fprintf(stderr,"Written elements from stack to file: %i\n",write);
    }

    fprintf(stderr,"Released bytes: %i\n",my_stack_purge(stack_aux));

    pthread_exit(NULL);

    fprintf(stderr,"Bye from main\n");

    return 0;
}

void *worker(void *ptr){

    //stack_aux = my_stack_read(ptr);
    int val_aux;
    for(int i = 0; i < N; i++){

        pthread_mutex_lock(&mutex);

        //fprintf(stderr,"Soy el hilo %lu ejecutando pop \n",pthread_self());
        val_aux = *((int*)my_stack_pop(stack_aux));
        val_aux++;
        //fprintf(stderr,"Soy el hilo %lu ejecutando push \n",pthread_self());
        my_stack_push(stack_aux,&val_aux);
        /*my_stack_pop();
        incrementam amb 1 valor de datos
        my_stack_pop();*/

        pthread_mutex_unlock(&mutex);

    }
    pthread_exit(NULL);

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
        return NULL; 
    }

    //lectura del size y control del error
    er = read(fd,&size_aux,sizeof(int)); 
    if (er == -1) {
        perror("Ha habido un problema al leer el fichero.\n"); 
        return NULL;  
    }
    
    //stack auxiliar e inicialización con size leído antes
    struct my_stack *stack_aux = my_stack_init(size_aux);
    
    //lectura de todos los data
    while (er > 0) {

        //reservar espacio de memoria para cada data y control del error   
        data = malloc(stack_aux->size);
        if (data == NULL) {
            printf("Ha habido un problema al reservar memoria.\n");  
            return NULL;          
        }

        //lectura del data y control del error
        er = read(fd,data,stack_aux->size);
        if (er == -1) {
            perror("Ha habido un problema al leer el fichero.\n");
            return NULL; 
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
        return NULL;       
    }
    
    return stack_aux;
    
}

