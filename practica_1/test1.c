/*
 * This code is a test of the functions implemented in my_lib.c
 * It's required to declare the functions and types in my_lib.h
 */

#include <stdio.h>
#include <string.h>
#include <math.h>

#include "my_lib.h"

void red () {
  printf("\033[1;31m");
}

void green () {
  printf("\033[1;32m");
}

void reset () {
  printf("\033[0m");
}

int test_strlen(char *strs[], int n) { // char **strs
    int i;
    reset();printf("\n***************************************\n");
    printf("Testeando my_strlen() frente a strlen()\n");
    printf("\n***************************************\n");
    for (i = 0; i < n; i++) {
        printf("*str: \"%s\"\n", strs[i]);
        printf("strlen(str) = %lu\n", strlen(strs[i]));
        printf("my_strlen(str) = %lu\n\n", my_strlen(strs[i]));

        if (strlen(strs[i]) != my_strlen(strs[i])) {
            red();printf("\n%s failed :-(\n", __func__);
            return -1;
        }
    }

    green();printf("\n%s passed :-)\n", __func__);
    return 0;
}

int sign(int x) { return (x > 0) - (x < 0); }

int test_strcmp(char *strs[], int n) {
    int i, j;
    reset();printf("\n***************************************\n");
    printf("Testeando my_strcmp() frente a strcmp()\n");
    printf("\n***************************************\n");
    for (i = 0; i < n; i++) {
        for (j = n - 1; j >= 0; j--) {
            printf("*str1: \"%s\", *str2: \"%s\"\n", strs[i], strs[j]);
            printf("strcmp(str1, str2) = %d\n", strcmp(strs[i], strs[j]));
            printf("my_strcmp(str1, str2) = %d\n\n", my_strcmp(strs[i], strs[j]));
            if (sign(strcmp(strs[i], strs[j])) != sign(my_strcmp(strs[i], strs[j]))) {
                red();printf("\n%s failed :-(\n", __func__);
                return -1;
            }
        }
    }

    green();printf("\n%s passed :-)\n", __func__);
    return 0;
}

int test_strcpy(char **strs, int n) {
    int i;
    char src[1024];
    char dest1[1024];
    char dest2[1024];

    reset();printf("\n***************************************\n");
    printf("Testeando my_strcpy() frente a strcpy()\n");
    printf("\n***************************************\n");
    for (i = 0; i < n; i++) {

        memset(dest1, 0, sizeof(dest1));
        memset(dest2, 0, sizeof(dest2));
        strcpy(dest1, strs[i]);
        strcpy(dest2, strs[i]);
        strcpy(src, strs[(i+1)%n]);

        printf("*dest: \"%s\", *src: \"%s\"\n", dest1, src);
        printf("strcpy(dest, src) = %s\n", strcpy(dest1, src));
        printf("my_strcpy(dest, src) = %s\n\n", my_strcpy(dest2, src));


        if (strcmp(dest2, dest1) != 0) {
            red();printf("\n%s failed :-(\n", __func__);
            return -1;
        }
    }

    green();printf("\n%s passed :-)\n", __func__);
    return 0;
}

int test_strncpy(char *dest, char *src, int num_test) {

    int len;
    char dest1[1024];
    char dest2[1024];

    reset();printf("\n*************************************************\n");
    printf("Testeando my_strncpy() frente a strncpy(). Test %d\n", num_test);
    printf("\n*************************************************\n");

   
    for (len = 1; len < 12; len+=3) {
        memset(dest1, 0, sizeof(dest1));
        memset(dest2, 0, sizeof(dest2));
        strcpy(dest1, dest);
        strcpy(dest2, dest);

        printf("*dest: \"%s\", *src: \"%s\", n = %d\n", dest1, src, len);
        printf("strncpy(dest, src, %d) = %s\n",len, strncpy(dest1, src, len));
        printf("my_strncpy(dest, src, %d) = %s\n\n",len, my_strncpy(dest2, src, len));


        if (strcmp(dest1, dest2) != 0) {
            red();printf("%s failed for len %d :-(\n", __func__, len);
            printf("*dest with strncpy: %s\t", dest1);
            printf("*dest with my_strncpy: %s\n", dest2);
            return -1;
        }
    }
    
    green();printf("%s passed :-)\n", __func__);
    return 0;
}

int test_strcat(char **strs, int n) {
    int i;
    char dest1[1024];
    char dest2[1024];
    char src[1024];

    reset();printf("\n***************************************\n");
    printf("Testeando my_strcat() frente a strcat()\n");
    printf("\n***************************************\n");
    for (i = 0; i < n; i++) {
        memset(dest1, 0, sizeof(dest1));
        memset(dest2, 0, sizeof(dest2));
        strcpy(dest1, strs[i]);
        strcpy(dest2, strs[i]);
        strcpy(src, strs[(i+1)%n]);

        printf("*dest: \"%s\", *src: \"%s\"\n", dest1, src);
        printf("strcat(dest, src) = %s\n", strcat(dest1, src));
        printf("my_strcat(dest, src) = %s\n\n", my_strcat(dest2, src));
    }

    if (strcmp(dest1, dest2) != 0) {
        red();printf("\n%s failed :-(\n", __func__);
        return -1;
    }

    green();printf("\n%s passed :-)\n", __func__);

    return 0;
}

int test_strchr(char **strs, int n, char c) {
    int i;
    char str1[1024];
    char str2[1024];

    reset();printf("\n***************************************\n");
    printf("Testeando my_strchr() frente a strchr()\n");
    printf("\n***************************************\n");
    for (i = 0; i < n; i++) {
        memset(str1, 0, sizeof(str1));
        memset(str2, 0, sizeof(str2));
        strcpy(str1, strs[i]);
        strcpy(str2, strs[i]);

        printf("*srt: \"%s\", char: \'%c\'\n", str1, c);
        printf("strchr(str, char) = %s\n", strchr(str1, c));
        printf("my_strchr(str, char) = %s\n\n", my_strchr(str2, c));
    }

    if (strcmp(str1, str2) != 0) {
        red();printf("\n%s failed :-(\n", __func__);
        return -1;
    }

    green();printf("\n%s passed :-)\n", __func__);

    return 0;
}

int main() {
    const int n = 3;
    char *strs[3] = {"programa", "compilador", "depurador"};

    // Init the array of char * with several strings and NULL at the end
    /*
    strs[0]="programa";   // longitud 8
    strs[1]="compilador"; // longitud 10 
    strs[2]="depurador";  // longitud 9 
    */

    test_strlen(strs, n);
    test_strcmp(strs, n);
    test_strcpy(strs, n);
    test_strncpy(strs[0], strs[1], 1);
    test_strncpy(strs[1], strs[0], 2);
    test_strcat(strs, n);
    test_strchr(strs, n,'m');

    return 0; // To avoid warning in -Wall
}
