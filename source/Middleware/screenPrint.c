#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "../../headers/defs.h"

//prints a line of "space" aka " "
void dynamic_linespace(int text_constant, int txt_indent, int txt_margin, int lines){
    if(lines == 0) return;
    for(int i = 0 ; i < lines ; i++){
        char space[1] = " ";
        printf("|%*c|\n", text_constant + txt_indent + txt_margin, space[0]);
    }
}

void dynamic_begin_text(int text_constant, int txt_indent, int txt_margin){
    char line[1] = "-";
    printf("/");
    for(int i = 0 ; i < text_constant + txt_indent + txt_margin ; i++){
        printf("%c",line[0]);
    }
    printf("\\\n");
}

void dynamic_end_text(int text_constant, int txt_indent, int txt_margin){
    char line[1] = "-";
    printf("\\");
    for(int i = 0 ; i < text_constant + txt_indent + txt_margin ; i++){
        printf("%c",line[0]);
    }
    printf("/\n");
}

void dynamic_line_print(char *string, int text_const, int txt_indent, int txt_margin){
    //have to check for \n chars and replace with pretty much a dymanic line space
    //recieves a string that is read and then it is printed in a certain way
    int arraysize = strlen(string);
    if(arraysize == 0) return;
    if(text_const == 0) text_const = 1;
    char space = ' ';
    int i = 0, j = 0;
    printf("|%*c", txt_indent, space);
    for(i = 0 ; i <= arraysize ; i++){
        if(string[i] == '\n' && i+1 != arraysize){
            printf("%*c|\n", text_const-j + txt_margin, space);
            j = 0;
            printf("|%*c", txt_indent, space);
            continue;
        }
        if(j >= text_const){
            printf("%*c|\n", text_const-j + txt_margin, space);
            printf("|%*c", txt_indent, space);
            i--;
            j = 0;
            continue;
        }
        //causing line cliping on last char of a string if its a single line print
        //fix is to add a sacrificial char to the system, i need to do this because
        //for some godforsaken reason c stores EOF as a char, so file reading dies
        //here if this doesnt exist.
        if(i+1 == arraysize){
            printf("%*c|\n", text_const-j + txt_margin, space);
            continue;
        }
        j++;
        printf("%c",string[i]);
    }
}

int readMenuFile(char *menuSection, char **menuText){
    char *dir = malloc(sizeof(char) * strlen(menuSection) + strlen("./menus/") + 128); //128 to stop segfaults;
    if(!dir)
        return -1;
    strcpy(dir, "./menus/");
    strcat(dir, menuSection);
    strcat(dir,".menu");
    FILE *fp = fopen(dir, "r");
    free(dir);
    if(fp == NULL)
        return -1;
    fseek(fp, 0, SEEK_END);
    int64_t filesize = ftell(fp);
    if(filesize == 0){
        fclose(fp);
        return -1;
    }
    fseek(fp, 0, SEEK_SET);
    *menuText = malloc(sizeof(char) * filesize + 1);
    if(!menuText){
        fclose (fp);
        return -1;
    }
    for(int i = 0 ; i < filesize ; i++){
        fseek(fp, i, SEEK_SET);
        (*menuText)[i] = fgetc(fp);
    }
    (*menuText)[filesize+1] = '\0';
    fclose(fp);
    return 0;
}

void printToScreen(char *input, int padding_top, int padding_bottom){
    syscls;
    dynamic_begin_text(TXT_CONST, INDENT, MARGIN);
    dynamic_linespace(TXT_CONST, INDENT, MARGIN, padding_top);
    dynamic_line_print(input, TXT_CONST, INDENT, MARGIN);
    dynamic_linespace(TXT_CONST, INDENT, MARGIN, padding_bottom);
    dynamic_end_text(TXT_CONST, INDENT, MARGIN);
}

int menuPrint(char *menuSection, int padding_top, int padding_bottom){
    char *menuText = NULL;
    if(readMenuFile(menuSection, &menuText) != 0)
        return -1;
    printToScreen(menuText, padding_top, padding_bottom);
    free(menuText);
    return 0;
}

int advancedPrint(char *input, int padding_top, int padding_bottom){
    if(input == NULL)
        return -1;
    int inlen = strlen(input);
    char *resize = realloc(input, sizeof(char) * inlen + 2);
    input = resize;
    strncpy(input, input, sizeof(input));
    strcat(input, " ");
    printToScreen(input, padding_top, padding_bottom);
    return 0;
}
