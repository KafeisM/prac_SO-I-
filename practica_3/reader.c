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