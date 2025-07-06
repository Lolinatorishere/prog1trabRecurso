#include <stdio.h>
#include <stdlib.h>


int main(){
    int64_t size = 0;
    FILE *fp = fopen("./build/commands.txt", "r");
    if(fp == NULL)
        return 1;
    fseek(fp,0,SEEK_END);
    size = ftell(fp);
    fseek(fp,0,SEEK_SET);
    if(size == 0)
        return 1;
    char *commands = malloc(sizeof(char)*size);
    for(int i = 0 ; i < size ; i++){
        fseek(fp,i,SEEK_SET);
        commands[i] = fgetc(fp);
    }
    system(commands);
    free(commands);
    fclose(fp);
}


