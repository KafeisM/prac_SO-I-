#include <stdio.h>
#include <stdlib.h>
#include "my_lib.h"

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
    

    while(dest[posD] != '\0'){
        posD++;
    }

    while(src[posS] != '\0'){
        dest[posD++] = src[posS++];
    }
    
    dest[posD] = '\0';

    return dest;
}
