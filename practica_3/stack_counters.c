#include <stdio.h>
#include <stdlib.h>
#include "my_lib.h"
#include <pthread.h>
#include <stdbool.h>
#include <string.h>

int NUM_THREADS = 3;
int N = 5;
pthread_mutex_t mutex = PTHREAD_MUTEX_INITIALIZER;
struct my_stack *stack_aux;

#define COMMAND_LINE_SIZE 1024
#define ARGV_SIZE 64

void *worker(void *ptr);

int main(int argc,char *argv[]){ 

    stack_aux = my_stack_read(argv[1]);
    pthread_t threads[NUM_THREADS];
    //int or_length,new_length;
    if(argv[1] == NULL){
        fprintf(stderr,"USAGE: ./stack_counters stack_file");
        return -1;
    }

    fprintf(stderr,"Threads: %i, Iterations: %i",NUM_THREADS,N);

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
        fprintf(stderr,"Error escritura en el fichero");
    }else{
        fprintf(stderr,"Written elements from stack to file: %i",write);
    }

    fprintf(stderr,"Released bytes: %i",my_stack_purge(stack_aux));

    pthread_exit(NULL);

    fprintf(stderr,"Bye from main");

    return 0;
}

void *worker(void *ptr){

    //stack_aux = my_stack_read(ptr);
    int val_aux;
    for(int i = 0; i < N; i++){

        pthread_mutex_lock(&mutex);

        fprintf(stderr,"Soy el hilo %lu ejecutando pop \n",pthread_self());
        val_aux = *((int*)my_stack_pop(stack_aux));
        val_aux++;
        fprintf(stderr,"Soy el hilo %lu ejecutando push \n",pthread_self());
        my_stack_push(stack_aux,&val_aux);
        /*my_stack_pop();
        incrementam amb 1 valor de datos
        my_stack_pop();*/

        pthread_mutex_unlock(&mutex);

    }
    pthread_exit(NULL);

}

