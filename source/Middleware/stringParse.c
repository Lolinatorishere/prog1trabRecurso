#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "../../headers/defs.h"

int isWhitespace(char c) {
    return (c == ' ' || c == '\t' || c == '\n' || c == '\r' || c == '\v' || c == '\f');
}

int stringReplace(char *input, char *filter, char *replace){
    if(input == NULL)
        return 1;
    if(filter[0] == '\0') 
        return 1;
    int i = 0;
    int replaced = 0;
    char *buffer = (char*) malloc(sizeof(char)*strlen(input));
    if(strlen(input) == 0){
        strcpy(input, "\0");
        return 0;
    }
    for(i = 0 ; i < strlen(input) ; i++){
        if(input[i] != filter[0] && replace[0] != '\0'){
            buffer[i - replaced] = input[i];
            continue;
        }else{
            buffer[i] = replace[0];
        }
        if(replace[0] == '\0') replaced++;
    }
    strcpy(input, buffer);
    free(buffer);
    return 0;
}

void trim(char *str) {
    char *start = str;
    char *end;
    while (isWhitespace(*start)) {
        start++;
    }
    if (*start == '\0') {
        str[0] = '\0';
        return;
    }
    end = start + strlen(start) - 1;
    while (end > start && isWhitespace(*end)) {
        end--;
    }
    *(end + 1) = '\0';
    // Shift the trimmed string to the beginning of the original buffer
    memmove(str, start, end - start + 2);
}

//int intNumberInput(char *input, int *output);
int64_t int64FromString(char *input){
    if(input == NULL)
        return -1;
    int inlen = strlen(input),
        j = 0;
    char seperateNums[256] = {'\0'};
    if(inlen > 255)
       inlen = 255;
    if(strlen(input) == 0)
        return -1;
    for(int i = 0 ; i < inlen ; i++){
        isasciinum(input[i]){
            seperateNums[j] = input[i];
            j++;
        }
    }
    return atoll(seperateNums);
}

//int floatNumberInput(char *input, float *output);

double doubleFromString(char *input){
    if(input == NULL)
        return -1;
    if(strlen(input) == 0)
        return -1;
    return atof(input);
}

int centerString(int size, char *text){
    int textsize = strlen(text);
    int index = 0, i = 0;
    int margins = (size - textsize) / 2;
    if(size < textsize)
        return -1;
    if(!text)
        return -1;
    if(textsize > TXT_CONST)
        return -1;
    char *buffer = calloc(TXT_CONST, sizeof(char));
    for(i = 0 ; i < margins ; i++)
        buffer[i] = ' ';
    index += i;
    strcat(buffer, text);
    index += textsize;
    for(i = 0 ; i < margins ; i++)
        buffer[i+index] = ' ';
    index += i;
    buffer[index] = '\0';
    strcpy(text, buffer);
    free(buffer);
    return 0;
}
