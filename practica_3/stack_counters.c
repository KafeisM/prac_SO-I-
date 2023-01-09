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
void imprimir_stack(struct my_stack *stack);

int main(int argc,char *argv[]){ 

    pthread_t threads[NUM_THREADS];
    //int or_length,new_length;
    if(argv[1] == NULL){
        fprintf(stderr,"USAGE: ./stack_counters <stack_file>\n");
        exit(0);
    }

    stack_aux = my_stack_read(argv[1]);

    fprintf(stderr,"Threads: %i, Iterations: %i\n",NUM_THREADS,N);
    
    int *data = 0;
    
    if (stack_aux == NULL){
        //si la pila no exista, creamos pila N elementos a 0

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

        fprintf(stderr,"stack content for treatment:\n");
        imprimir_stack(stack_aux);
        fprintf(stderr,"new stack length: %i \n",my_stack_len(stack_aux));
    
    }else{ //sino significa que esta completa
        fprintf(stderr,"stack->size: %i \n",stack_aux->size);
        fprintf(stderr,"original stack:\n");
        imprimir_stack(stack_aux);
        fprintf(stderr,"original stack length: %i \n",my_stack_len(stack_aux));

    }
    
    //CREACIÓN HILOS

    for(int i = 0; i < NUM_THREADS; i++){
        pthread_create(&threads[i], NULL, worker, NULL);
        fprintf(stderr,"%i) Thread %lu created \n",i,threads[i]);
    }
    
    for(int i = 0; i < NUM_THREADS; i++){
        pthread_join(threads[i], NULL);
    }

    fprintf(stderr,"\nstack content after threads iterations\n");
    imprimir_stack(stack_aux);
    fprintf(stderr,"stack length: %i \n",my_stack_len(stack_aux));

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

    int *val_aux;
    for(int i = 0; i < N; i++){

       /*
       pthread_mutex_lock(&mutex);

        fprintf(stderr,"Soy el hilo %lu ejecutando pop \n",pthread_self());
        val_aux = *((int*)my_stack_pop(stack_aux));
        fprintf(stderr,"valor aux: %d por el hilo %lu\n",val_aux, pthread_self());
        val_aux++;
        fprintf(stderr,"Soy el hilo %lu ejecutando push \n",pthread_self());
        my_stack_push(stack_aux,&val_aux);
        my_stack_pop();
        incrementam amb 1 valor de datos
        my_stack_pop();
        pthread_mutex_unlock(&mutex);
        */

        //worker 2
        pthread_mutex_lock(&mutex);
        val_aux = my_stack_pop(stack_aux);
        pthread_mutex_unlock(&mutex);

        (*val_aux)++;
        

        pthread_mutex_lock(&mutex);
        my_stack_push(stack_aux,val_aux);
        pthread_mutex_unlock(&mutex);

    }
    pthread_exit(NULL);

}

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

