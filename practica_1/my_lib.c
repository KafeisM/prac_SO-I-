#include <stdio.h>


size_t my_strlen(const char *str);
int my_strcmp(const char *str1, const char *str2);
char *my_strcpy(char *dest, const char *src);
char *my_strncpy(char *dest, const char *src, size_t n);
char *my_strcat(char *dest, const char *src);
char *my_strchr(const char *str, int c);

//PRUEBAS TEMPORALES
void main(){
    char *str1 = {"hola"};
    char *str2 = {"HOLA"};

    //test STRCMP
    int prueba = my_strcmp(str1,str2);
    if(prueba  < 0){
        printf("El string 2 es mayor al string 1\n");
    }else if(prueba > 0){
        printf("El string 1 es mayor al string 2\n");
    }else{
        printf("Los stirngs son iguales\n");
    }
    

}

int my_strcmp(const char *str1, const char *str2){
    
    /*mentre no final de cadena i els elements siguin igual, seguir avançant
    si 2 elements ja no son iguals, mirar quin es major
    retun > 0 si str1 > str2
    retun < 0 si str2 > str1
    retun = 0 si str1 = str2*/

    while(*str1 && (*str1 == *str2)){
        str1++;
        str2++;
    }

    return *str1 - *str2;
}



