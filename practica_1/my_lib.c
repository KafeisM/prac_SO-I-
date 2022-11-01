#include <stdio.h>
#include <stdlib.h>
#include "my_lib.h"

//FUNCIONS STRINGS

size_t my_strlen(const char *str){
  
    long res = 0;
    int cont = 0;

    char aux = str[cont];
    while(aux!='\0'){
        if(aux!=' '){
            res++;
        }
        cont++;
        aux = str[cont];
    }
    return res;
}

char *my_strchr(const char *str, int c){

    while (*str != '\0'){
      if (*str == c){
        return (char *)str;
      }
      str++;
    }

    return NULL;
}

int my_strcmp(const char *str1, const char *str2)
{

    /*mentre no final de cadena i els elements siguin igual, seguir avançant
    si 2 elements ja no son iguals, mirar quin es major
    retun > 0 si str1 > str2
    retun < 0 si str2 > str1
    retun = 0 si str1 = str2*/

    while (*str1 && (*str1 == *str2))
    {
        str1++;
        str2++;
    }

    return *str1 - *str2;
}

char *my_strcpy(char *dest, const char *src){

    int aux= 0;
    int aux2= 0;

    while(dest[aux2] != '\0'){
        dest[aux2++] = '\0';
    }

    while (src[aux] != '\0')
    {
        dest[aux] = src[aux];
        aux++;
    }

    return dest;
}

char *my_strncpy(char *dest, const char *src, size_t n){

    int len = my_strlen(src);
    

    if (len < n)
    {
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
        for (int i = 0; i < n; i++)
        {
            dest[i] = src[i];
        }
    }
    return dest;
}

char *my_strcat(char *dest, const char *src){
    /*Cadena de src acabada amb '\0' afegir-ho a dest
    primer caracter de src sobreescriu el darrer de dest
    retorna punter dest*/

    int posD = 0;
    int posS = 0;
    

    while(dest[posD] != '\0'){
        posD++;
    }

    while(src[posS] != '\0'){
        dest[posD++] = src[posS++];
    }
    
    dest[posD] = '\0';

    return dest;
}

//FUNCIONS GESTOR PILA

struct my_stack *my_stack_init(int size){
    struct my_stack *stack;

    stack = malloc(size);
    //mirar error
    stack->size = size;
    stack->top = NULL;

    return stack;
}

int my_stack_push (struct my_stack *stack, void *data){
    struct my_stack_node *node;

    if(stack == NULL || stack->size < 0){
        return -1;
    }else{
        node = malloc(stack->size);
        //mirar error
        node->next = stack->top;
        stack->top = node;
        node->data = data;
        
        return 0;
    }
}

int my_stack_purge(struct my_stack *stack){
    int num_mem = sizeof(struct my_stack);
    int len =  my_stack_len(stack);
    struct my_stack_node *aux;

    if(stack->top == NULL){ 
        return num_mem;

    }else{
        num_mem = (len * sizeof(struct my_stack_node)) + (stack->size * len) + num_mem;   
        aux = stack->top;
        
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



int my_stack_len (struct my_stack *stack){
    struct my_stack_node *aux;
    int cont = 1;

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

void *my_stack_pop (struct my_stack *stack){
    
    struct my_stack_node *aux = NULL;

    if (stack->top == NULL){
        return NULL;
    }else{
        aux = stack->top;
        stack->top = aux->next;
        return aux->data;
    }
    
}



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


