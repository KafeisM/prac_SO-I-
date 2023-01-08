#include <stdio.h>
#include <stdlib.h>
#include "my_lib.h"
#include <pthread.h>
#include <stdbool.h>
#include <string.h>

int NUM_THREADS = 10;
int N = 1000000;
pthread_mutex_t mutex = PTHREAD_MUTEX_INITIALIZER;

#define COMMAND_LINE_SIZE 1024
#define ARGV_SIZE 64

void *worker(void *ptr);

int main(int argc,char *argv[]){ 

    struct my_stack *stack_aux = my_stack_read(argv[1]);
    pthread_t threads[NUM_THREADS];

    if(argv[1] == NULL){
        fprintf(stderr,"USAGE: ./stack_counters stack_file");
        return -1;
    }

    if (stack_aux == NULL){
        stack_aux = my_stack_init(4);
    }else if(my_stack_len(stack_aux) < NUM_THREADS){
        int *data = 0;
        for (int i = 0; i < (NUM_THREADS - my_stack_len(stack_aux)); i++){
            data = malloc(4);
            my_stack_push(stack_aux,data);
        }
        my_stack_write(stack_aux,argv[1]);
    }
    my_stack_purge(stack_aux);


    for(int i = 0; i < NUM_THREADS; i++){
        pthread_create(&threads[i], NULL, worker, NULL);
    }


    return 0;
}

void *worker(void *ptr){

    for(int i = 0; i < N; i++){

        pthread_mutex_lock(&mutex);

        /*my_stack_pop();
        incrementam amb 1 valor de datos
        my_stack_pop();*/

        pthread_mutex_unlock(&mutex);

    }

}

