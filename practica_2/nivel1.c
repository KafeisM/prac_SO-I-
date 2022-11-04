#define COMMAND_LINE_SIZE 1024
#define PROMPT ‘$’

#include <stdlib.h>
#include <stdio.h>
#include <stdbool.h>
#include <string.h>

char *read_line(char *line); 

int main(){
    char line[COMMAND_LINE_SIZE];  
    while(true){
        if(read_line(line)){
            execute_line(line);
        }
    }
}

char *read_line(char *line){
    

}




