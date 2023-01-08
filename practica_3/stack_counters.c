#include <stdio.h>
#include <stdlib.h>
#include "my_lib.h"
#include <pthread.h>
#include <stdbool.h>
#include <string.h>

int NUM_THREADS = 10;
int N = 1000000;

#define COMMAND_LINE_SIZE 1024
#define ARGV_SIZE 64

int main(int argc,char *argv[]){ 

    struct my_stack *stack_aux = my_stack_read(argv[1]);

    if(argv[1] == NULL){
        fprintf(stderr,"USAGE: ./stack_counters stack_file");
        return -1;
    }

    if (stack_aux == NULL){
        stack_aux = my_stack_init(4);
    }else if(my_stack_len(stack_aux) < NUM_THREADS){
        int *data = 0;
        for (int i = 0; i < (NUM_THREADS - my_stack_len(stack_aux)); i++){
            my_stack_push(stack_aux,data);
        }
    }
    return 0;
}