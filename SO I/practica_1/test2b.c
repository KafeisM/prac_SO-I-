/*
 * This code is a test of the functions implemented in my_lib.c
 * It's required to declare the functions and types in my_lib.h
 */

#include <stdio.h>
#include "my_lib.h"

//#define NODES 100
#define NODES 10

struct my_data {
    int val;
    char name[60];
};


int main() {

    struct my_stack *s1, *s2, *fs1;
    struct my_data *data, *data1, *data2;
    int len1, len2;
    int size1, size2;
    int occupied_bytes=0;

    s1 = my_stack_init(sizeof(struct my_data));
    if (!s1) {
        puts("Error in my_stack_init()");
        exit(EXIT_FAILURE);
    }
    printf("\ns1 initialized, size of data: %lu\n", sizeof(struct my_data));

    for (int i = 0; i < NODES; i++) {
        data = malloc(sizeof(struct my_data)); 
        if (!data) return EXIT_FAILURE;
        data->val = i;
        sprintf(data->name, "Value %d", i);
        if (my_stack_push(s1, data)<0) {
            puts("Error in my_stack_push()");
            exit(EXIT_FAILURE);
        }
        printf("New node in s1: (%d, %s)\n", data->val, data->name);
    }

    len1 = my_stack_len(s1);
    if (NODES != len1) {
        puts("Error in my_stack_len()");
        exit(EXIT_FAILURE);
    }
    printf("s1 length: %d\n", len1);

    // Create a second larger stack
    s2 = my_stack_init(sizeof(struct my_data));
    if (!s2) {
        puts("Error in my_stack_init()");
        exit(EXIT_FAILURE);
    }
    printf("\ns2 initialized, size of data: %lu\n", sizeof(struct my_data));

    for (int i = 0; i < NODES * 3; i++) {
        data = malloc(sizeof(struct my_data)); 
        data->val = i;
        sprintf(data->name, "Value %d", i);
        if (my_stack_push(s2, data)<0) {
            printf("Error in my_stack_push (%d)\n", i);
            exit(EXIT_FAILURE);
        }
        printf("New node in s2: (%d, %s)\n", data->val, data->name);
    }
    len2 = my_stack_len(s2);
    printf("s2 length: %d\n\n", len2);


    // Write the larger stack top
    puts("Writting the larger stack top (s2) in the file \"/tmp/my_stack.data\".");
    if (my_stack_write(s2, "/tmp/my_stack.data") <= 0) {
        puts("Error in my_stack_write (s2)");
        exit(EXIT_FAILURE);
    }

    puts("Writting the smaller stack (s1), it must truncate the file.");
    // Now the smaller stack, it must truncate the file
    if (my_stack_write(s1, "/tmp/my_stack.data") != len1) {
        puts("Error in my_stack_write (s1)");
        exit(EXIT_FAILURE);
    }
    puts("\nReading the file...");
    fs1 = my_stack_read("/tmp/my_stack.data");
    if (!fs1) {
        puts("Error in my_stack_read (fs1)");
        exit(EXIT_FAILURE);
    }

    if (my_stack_len(s1) != my_stack_len(fs1)) {
        puts("Stacks s1 (initial stack 1) and fs1 (retrieved from file) don't have the same length");
        exit(EXIT_FAILURE);
    }
    puts("s1 and the one retrieved from file (fs1) have the same length.");
    puts("Comparing the data with pop...");

    // Test we can free the data and compare stacks s1 and fs1
    while ((data1 = my_stack_pop(s1))) {
        data2 = my_stack_pop(fs1);
        printf("Node of s1: (%d, %s)\t", data1->val, data1->name);
        printf("Node of fs1: (%d, %s)\n", data2->val, data2->name);
        if (!data2 || data1->val != data2->val || my_strcmp(data1->name, data2->name)) {
            printf("Data in s1 and fs1 are not the same.\n (data1->val: %d <> data2->val: %d) o (data1->name: %s <> data2->name: "
                   "%s)\n",
                   data1->val, data2->val, data1->name, data2->name);
            exit(EXIT_FAILURE);
        }
        size1 = sizeof(*data1);
        size2 = sizeof(*data2);
        free(data1);
        free(data2);
    }

    printf("size of data from s1: %d\t", size1);
    printf("size of data from fs1: %d\n", size2);
    len1 = my_stack_len(s1);
    occupied_bytes = len1*(sizeof(struct my_stack_node)+size1) + sizeof (struct my_stack);
    printf ("\nWhole stack space s1: %d bytes\n", occupied_bytes);
    if (occupied_bytes != my_stack_purge(s1)) {
        puts("Error purging s1");
        exit(EXIT_FAILURE);
    }  
    printf("Purging s1: %d bytes released\n", occupied_bytes);

    len2 = my_stack_len(s2);
    occupied_bytes = len2*(sizeof(struct my_stack_node)+size1) + sizeof (struct my_stack);
    printf ("\nWhole stack space s2: %d bytes\n", occupied_bytes);
    if (occupied_bytes != my_stack_purge(s2)) {
        puts("Error purging s2");
        exit(EXIT_FAILURE);
    }
    printf("Purging s2: %d bytes released\n", occupied_bytes);

    len1 = my_stack_len(fs1);
    occupied_bytes = len1*(sizeof(struct my_stack_node)+size2) + sizeof (struct my_stack);
    printf ("\nWhole stack space fs1: %d bytes\n", occupied_bytes);
    if (occupied_bytes != my_stack_purge(fs1)) {
        puts("Error purging fs1");
        exit(EXIT_FAILURE);
    }
    printf("Purging fs1: %d bytes released\n", occupied_bytes);

    puts("\nAll tests passed :-)");
    return EXIT_SUCCESS; // To avoid warning in -Wall
}
