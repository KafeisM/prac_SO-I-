#include <stdio.h>
#include <stdlib.h>
#include <string.h>

size_t my_strlen(const char *str);
int my_strcmp(const char *str1, const char *str2);
char *my_strcpy(char *dest, const char *src);
char *my_strncpy(char *dest, const char *src, size_t n);
char *my_strcat(char *dest, const char *src);
char *my_strchr(const char *str, int c);

// PRUEBAS TEMPORALES
void main()
{
    char *str1 = {"mundo"};
    char *str2;
    char aux[15] = {"hola "};
    str2 = aux;

    // test STRCMP
    /* int prueba = my_strcmp(str1,str2);
    if(prueba  < 0){
        printf("El string 2 es mayor al string 1\n");
    }else if(prueba > 0){
        printf("El string 1 es mayor al string 2\n");
    }else{
        printf("Los stirngs son iguales\n");
    } */

    // test my_strcpy

    // test my_strncpy

    // test my_strcat
    str2 = my_strcat(str2, str1);
    printf("%s\n", str2);
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

    int aux = 0;
    while (src[aux] != '\0')
    {
        dest[aux] = src[aux];
        aux++;
    }

    return dest;
}

char *my_strncpy(char *dest, const char *src, size_t n){

    int len;
    for (len = 0; src[len] != '\0'; len++)
    {
    }

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
    for (posD; dest[posD] != '\0'; posD++){
    }

    while(src[posS] != '\0'){
        dest[posD++] = src[posS++];
    }
    
    dest[posD] = '\0';

    return dest;
}
