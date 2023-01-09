#include <stdio.h>
#include <stdlib.h>
#include "my_lib.h"
#include <pthread.h>
#include <stdbool.h>
#include <string.h>

 //numero de hilos y accesos a la pila
int NUM_THREADS = 10;
int N = 1000000;

//semáforo y pila auxiliar
pthread_mutex_t mutex = PTHREAD_MUTEX_INITIALIZER;
struct my_stack *stack_aux;

//tamaño de línea y argv
#define COMMAND_LINE_SIZE 1024
#define ARGV_SIZE 64

void *worker(void *ptr);
void imprimir_stack(struct my_stack *stack);

int main(int argc,char *argv[]){ 

    //array de hilos (sus identificadores)
    pthread_t threads[NUM_THREADS];

    //control si no se ha especificado ningun fichero
    if(argv[1] == NULL){
        fprintf(stderr,"USAGE: ./stack_counters <stack_file>\n");
        exit(0);
    }

    //leemos la pila
    stack_aux = my_stack_read(argv[1]);

    fprintf(stderr,"Threads: %i, Iterations: %i\n",NUM_THREADS,N);
    
    //variable para el dato que extraemos de la pila
    int *data = 0;
    
    if (stack_aux == NULL){
        //si la pila no existe, creamos pila N elementos a 0

        stack_aux = my_stack_init(4);
        fprintf(stderr,"stack->size: %i \n",stack_aux->size);
        fprintf(stderr,"Initial stack content:\n");

        for (int i = 0; i < NUM_THREADS; i++){
            data = malloc(4);
            my_stack_push(stack_aux,data);
        }

        //imprimir nueva pila
        fprintf(stderr,"stack content for treatment:\n");
        imprimir_stack(stack_aux);
        fprintf(stderr,"new stack length: %i \n",my_stack_len(stack_aux));

    }else if(my_stack_len(stack_aux) < NUM_THREADS){
        //si la pila existe pero elementos < 10, la completamos con 0

        fprintf(stderr,"Initial stack content:\n");
        imprimir_stack(stack_aux);

        fprintf(stderr,"stack->size: %i \n",stack_aux->size);
        fprintf(stderr,"original stack length: %i \n",my_stack_len(stack_aux));
        int len = my_stack_len(stack_aux);
        for (int i = 0; i < (NUM_THREADS - len); i++){
            data = malloc(4);
            my_stack_push(stack_aux,data);
        }

        //imprimir la nueva pila
        fprintf(stderr,"stack content for treatment:\n");
        imprimir_stack(stack_aux);
        fprintf(stderr,"new stack length: %i \n",my_stack_len(stack_aux));
    
    }else{ //sino significa que ya está completa
        fprintf(stderr,"stack->size: %i \n",stack_aux->size);
        fprintf(stderr,"original stack:\n");

        //imprimir la nueva pila
        imprimir_stack(stack_aux);
        fprintf(stderr,"original stack length: %i \n",my_stack_len(stack_aux));

    }
    
    //CREACIÓN HILOS

    //bucle para crear los hijos
    for(int i = 0; i < NUM_THREADS; i++){
        pthread_create(&threads[i], NULL, worker, NULL);
        fprintf(stderr,"%i) Thread %lu created \n",i,threads[i]);
    }
    
    //bucle donde el hilo principal espera a que acaben los otros
    for(int i = 0; i < NUM_THREADS; i++){
        pthread_join(threads[i], NULL);
    }

    //imprimir nuevo stack
    fprintf(stderr,"\nstack content after threads iterations\n");
    imprimir_stack(stack_aux);
    fprintf(stderr,"stack length: %i \n",my_stack_len(stack_aux));

    //escribimos la pila en el fichero
    int write = my_stack_write(stack_aux,argv[1]);

    //control del error en la escritura
    if (write == -1){
        fprintf(stderr,"Error escritura en el fichero\n");
    }else{
        fprintf(stderr,"Written elements from stack to file: %i\n",write);
    }

    //liberamos la pila y mostramos los bytes liberados
    fprintf(stderr,"Released bytes: %i\n",my_stack_purge(stack_aux));

    pthread_exit(NULL);

    fprintf(stderr,"Bye from main\n");

    return 0;
}

void *worker(void *ptr){

    int *val_aux;
    for(int i = 0; i < N; i++){

        //para cada acceso acemos un pop, modificamos el valor y push

        //protegemos el valor en el pop y en el push

        pthread_mutex_lock(&mutex);
        val_aux = my_stack_pop(stack_aux);
        pthread_mutex_unlock(&mutex);

        //incrementamos el valor
        (*val_aux)++;

        pthread_mutex_lock(&mutex);
        my_stack_push(stack_aux,val_aux);
        pthread_mutex_unlock(&mutex);

    }
    pthread_exit(NULL);

}

/*---------------------------------------------------------------------------------------------------------
* Imprime una pila
* Input:    stack: puntero a la pila que queremos imprimir
* Output:   -
---------------------------------------------------------------------------------------------------------*/

void imprimir_stack(struct my_stack *stack){
    //nodo auxiliar y contador para el resultado
    struct my_stack_node *aux;
    int num;

    //control de pila vacía y bucle para aumentar el contador
    if(stack->top == NULL){
        fprintf(stderr,"\n");
    }else{
        aux = stack->top;
        do{
            num = *((int*)aux->data);
            fprintf(stderr,"%d\n",num);
            aux = aux->next;
           
        }while(aux != NULL);
    }
}

