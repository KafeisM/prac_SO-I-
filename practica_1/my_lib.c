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

    if (stack == NULL) {
        printf("Error! La pila no existe.");
        return -1;
    }

    int fd;
    int errorControl;
    fd = open(filename, O_WRONLY | O_CREAT | O_TRUNC, 0666); //Abrir fichero

    if (fd == -1) {
        perror("Error al abrir el fichero!!!");
        return -1;
    }

    //Pila Original -> 2 1 0 (valores a modo de ejemplo)

    //Pila auxiliar -> 0 1 2

    //Escritura de la pila auxiliar en fichero -> 0 1 2

    //Lectura de fichero para crear pila -> 2 1 0

    struct my_stack* aux = my_stack_init(stack->size); //Creamos pila auxiliar

    //Guardamos valores de pila original en pila auxiliar
    struct my_stack_node* top = stack->top;
    while (stack->top != NULL)
    {
        my_stack_push(aux, stack->top->data);
        stack->top = stack->top->next;
       
    }
    stack->top = top;


    size_t writtenElements = 0;

    //Escribimos en el fichero
    errorControl = write(fd, &(stack->size), sizeof(stack->size));
   
    if (errorControl == -1) {
            perror("Error al escribir en el fichero!!!");
            return -1;
    }

    while (aux->top != NULL) {
        errorControl = write(fd, my_stack_pop(aux), (stack->size));
        //fd = fichero
        //my_stack_pop(aux) = data (a escribir);
        //(stack->size) = tamaño de la estructura en bytes <-> sizeof(struct my_data)
        if (errorControl == -1) {
            perror("Error al escribir en el fichero!!!");
            return -1;
        }
        writtenElements++;
    }
    


    //Cerramos fichero
    errorControl = close(fd);
    
    if (errorControl == -1) {
        perror("Error al cerrar el fichero!!!");
        return -1;
    }


    return writtenElements; //Devolvemos número de elementos escritos
}

struct my_stack *my_stack_read(char *filename){
    int fd;
    int fe;
    int size;

    fd = open(filename,O_RDONLY); //Abrir fichero
    if (fd == -1) {
        perror("Error al abrir el fichero!!!");
    }
    
    fe = read(fd, &size, sizeof(int)); 
    
    if (fe == -1) {
        perror("Error al leer el fichero!!!");      
    }
    
    //Creamos pila
    struct my_stack *s = my_stack_init(size);
    void* data;

    //Leemos  
    while (fe > 0) {
           

        data = malloc(s->size);

        if (data == NULL) {
            printf("No se pudo reservar memoria.\n");
            
        }

        fe = read(fd, data, s->size);

        if (fe == -1) {
            perror("Error al leer el fichero!!!");
        }


        if (fe > 0) {
            my_stack_push(s, data);
        }
            
     }
       
    close(fd);
    return s;
    
}


