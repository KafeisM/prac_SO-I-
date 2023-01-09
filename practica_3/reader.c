#include <stdio.h>
#include <stdlib.h>
#include "my_lib.h"
#include <pthread.h>
#include <stdbool.h>
#include <string.h>
#include <limits.h>

struct my_stack *stack_aux;
int NUM_THREADS = 10;

int main(int argc,char *argv[]){

    if(argv[1] == NULL){
        fprintf(stderr,"USAGE: ./reader <stack_file> \n");
        return -1;
    }

    stack_aux = my_stack_read(argv[1]);

    if (stack_aux == NULL){
        fprintf(stderr,"Couldn't open stack file: %s",argv[1]);
        return -1;
    }

    int val_aux, items, sum, min, max, average;
    min = INT_MAX;
    max = 0;

    items = my_stack_len(stack_aux);
    
    fprintf(stderr,"Stack length: %i \n",items);

    if(items > NUM_THREADS){
        items = NUM_THREADS;
    }
    
    for (int i = 0; stack_aux->top != NULL && (i<NUM_THREADS); i++){  

        val_aux = *((int*)my_stack_pop(stack_aux));
        fprintf(stderr,"%i \n",val_aux);

        sum += val_aux;
        if (val_aux > max){
            max = val_aux;
        }
        if (val_aux < min){
            min = val_aux;
        }

    }
    average = sum/items;
    fprintf(stderr,"Items: %i | Sum: %i | Min: %i | Max: %i | Average: %i \n",items,sum,min,max,average);
}