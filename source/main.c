#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "../headers/screenPrint.h"
#include "../headers/defs.h"
#include "../headers/user.h"

int firstTime(){
    int first = 0;
    FILE *in = fopen(USERDATA, "r");
    if(!in){
        first = 1;
        FILE *create = fopen(USERDATA, "w");
        if(!create) return -1;
        fclose(create);
    }else{
        if(readTotalUsers() == 0) first = 1;
        fclose(in);
    }
    return first;
}

int main(){
    menuPrint("initMenu", 1, 1);
    //advancedPrint("hello world", 1, 1);
    //check first time boot:
    switch(firstTime()){
        case 1:
            menuPrint("firstTimeUser", 1, 1);
            break;
        case -1:
            printf("An Error Occured, couldnt create USERDATA file\n");
            break;
        default:
            menuPrint("initMenu", 1, 1);
            break;
    }
    while(1){
        return 0;
    }
    return 0;
}
