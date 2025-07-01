#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <stdbool.h>
#include "../headers/screenPrint.h"
#include "../headers/stringParse.h"
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
    char buffer[256] = {'\0'};
    bool pwderr = false, blink = false;
    USERS user = setUser();
    menuPrint("initMenu", 1, 1);
    //advancedPrint("hello world", 1, 1);
    //check first time boot:
    switch(firstTime()){
        case 1:
            while(1){
                menuPrint("firstTimeUser", 1, 1);
                if(pwderr){
                    if(!blink){
                        printf("Password nao pode ser \033[31m\"NULL\"\033[0m\n");
                        blink = true;
                    }else{
                        printf("Password nao pode ser \"NULL\"\n");
                        blink = false;
                    }
                } 
                printf("password:");
                fgets(buffer, 255, stdin);
                isNonValid(buffer[0]){
                    pwderr = true;
                    continue;
                }
                trim(buffer);
                if(createUser("admin", buffer, 100) != 1) continue;
                advancedPrint("user Admin foi criado com sucesso", 1, 1);
                sleep(1);
                break;
            }
            break;
        case -1:
            printf("An Error Occured, couldnt create USERDATA file\n");
            break;
        default:
            break;
    }
    while(1){
        menuPrint("initMenu", 1, 1);
        fgets(buffer, 255, stdin);
        if(int64FromString(buffer) == 0){
            return 0;
        }
    }
}
