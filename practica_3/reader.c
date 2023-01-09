#include <stdio.h>
#include <stdlib.h>
#include "my_lib.h"
#include <pthread.h>
#include <stdbool.h>
#include <string.h>
#include <limits.h>

//pila auxiliar y numero de hilos
struct my_stack *stack_aux;
int NUM_THREADS = 10;

int main(int argc,char *argv[]){

    //control si no se ha especificado ningun fichero
    if(argv[1] == NULL){
        fprintf(stderr,"USAGE: ./reader <stack_file> \n");
        return -1;
    }

    //leemos la pila
    stack_aux = my_stack_read(argv[1]);

    //si la pila no existe controlamos el error
    if (stack_aux == NULL){
        fprintf(stderr,"Couldn't open stack file: %s",argv[1]);
        return -1;
    }

    //variables para los cálculos
    int val_aux, items, sum, min, max, average;
    min = INT_MAX;
    max = 0;

    items = my_stack_len(stack_aux);
    
    fprintf(stderr,"Stack length: %i \n",items);

    //si la longitud de la pila es mayor que el numero de hilos, le asignamos su valor
    if(items > NUM_THREADS){
        items = NUM_THREADS;
    }
    
    //recorremos la pila para extraer cada dato
    for (int i = 0; stack_aux->top != NULL && (i<NUM_THREADS); i++){  

        val_aux = *((int*)my_stack_pop(stack_aux));
        fprintf(stderr,"%i \n",val_aux);

        //calculos del sumatorio, maximo y minimo
        sum += val_aux;
        if (val_aux > max){
            max = val_aux;
        }
        if (val_aux < min){
            min = val_aux;
        }

    }

    //cálculo de la media
    average = sum/items;

    //imprimimos los resultados
    fprintf(stderr,"Items: %i | Sum: %i | Min: %i | Max: %i | Average: %i \n",items,sum,min,max,average);
}