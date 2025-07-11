//c libraries
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include<sys/types.h>
#include<sys/stat.h>
#include <unistd.h>
#include <stdbool.h>
//my files
#include "../../../headers/defs.h"
#include "../../../headers/screenPrint.h"
#include "../../../headers/stringParse.h"
#include "../../../headers/user.h"
#include "../../../headers/event.h"
#include "../../../headers/Menus/menuMiddleware.h"
#include "../../../headers/Data/dataMiddleware.h"

//global variables
struct stat st = {0};
STUDENTQUEUE *queues = NULL;

//copy paste menus code legibility.
//may fk with compiler errors
#include "./Admin/userMenus.c"
#include "./Admin/eventMenus.c"

int startUp(int recursion){
    char buffer[256] = {'\0'};
    bool pwderr = false, blink = false;
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
                USERS user = {-1, -1, "admin", '\0', '\0'};
                strcpy(user.password, buffer);
                if(createUser(&user, 100) != 0)
                    continue;
                advancedPrint("user \"admin\" foi criado com sucesso ", 1, 1, 0);
                sleep(1);
                return 0;
            }
            break;
        case -1:
            if(recursion == 1)
                return -1;
            if(stat(DATADIR, &st) == -1)
                mkdir(DATADIR, 0755);
            if(stat(EVENTSUBDIR, &st) == -1)
                mkdir(EVENTSUBDIR, 0755);
            if(startUp(1) == 0)
                return 0;
            printf("An Error Occured, couldnt create USERDATA file\n");
            sleep(1);
            return -1;
        default:
            queues = createAllQueues();
            loadEventStudents(queues);
            return 0;
    }
}

void login(USERS *user){
    int attempts = 0;
    char buffer[256] = {'\0'};
    char username[256] = {'\0'};
    char password[256] = {'\0'};
    while(attempts < 3){
        syscls;
        printf("Nome:");
        fgets(username, 255, stdin);
        if(strlen(username) == 0 || username[0] == '\n'){
            printf("username nao pode ser \"NULL\"\n");
            sleep(1);
            continue;
        }
        for(int i = strlen(username)-1 ; i < 256 ; i++){
            username[i] = '\0';
        }
        printf("Password:");
        fgets(password, 255, stdin);
        if(strlen(password) == 0 || password[0] == '\n'){
            printf("password nao pode ser \"NULL\"\n");
            sleep(1);
            continue;
        }
        for(int i = strlen(password)-1 ; i < 256 ; i++){
            password[i] = '\0';
        }
        trim(username);
        trim(password);
        if(userValidate(username, password, user) == 0){
            printf("Bem vindo %s\n", user->userName);
            sleep(2);
            return;
        }
        printf("username ou password erradas\n");
        sleep(2);
        attempts++;
    }
    returnText("start Menu", 3);
    return;
}

void loggedInAdmin(USERS *user){
    char buffer[256];
    int64_t input = 0;
    while(1){
        menuPrint("Admin", 1, 1);
        printf("input:");
        fgets(buffer, 256, stdin);
        input = int64FromString(buffer);
        switch(input){
            case 1:
                userAdmin((*user));
                continue;
            case 2:
                //administartion of events
                eventAdmin();
                continue;
            case 0:
                printf("Logout de %s, Adeus\n", user->userName);
                returnText("Login Menu", 3);
                *user = setUser();
                return;
            default:
                break;
        }
    }
}

void loggedInUser(USERS *user){
    char buffer[256];
    menuPrint("User", 1, 1);
    printf("input:");
    fgets(buffer, 256, stdin);
    int64_t input = int64FromString(buffer);
    while(1){
        switch(input){
            case 1:
                //1 - Ver Eventos
                break;
            case 2:
                //2 - Ver Inscricoes
                break;
            case 3:
                //3 - Ver notificacoes
                break;
            case 0:
                printf("Logout de %s, Adeus\n", user->userName);
                returnText("Login Menu", 3);
                *user = setUser();
                return;
            default:
                break;
        }
    }
}

void loggedIn(USERS *user){
    if(user->type >= 100){
        loggedInAdmin(user);
    }else{
        loggedInUser(user);
    }
    return;
}

void loginMenu(){
    USERS user = setUser();
    char buffer[256] = {'\0'};
    int input = 0;
    while(1){
        menuPrint("Login", 2, 2);
        printf("input:");
        fgets(buffer, 255, stdin);
        input = int64FromString(buffer);
        switch(input){
            case 1:
                login(&user);
                if(user.userId <= 0) return;
                loggedIn(&user);
                break;
            case 2:
                NewUser(0);
                break;
            case 0:
                return;
            default:
                syscls;
                printf("\nInput Invalido\n");
                sleep(1);
                break;
        }
    }
}

void initMenu(){
    char buffer[256] = {'\0'};
    menuPrint("initMenu", 1, 1);
    while(1){
        menuPrint("initMenu", 1, 1);
        fgets(buffer, 255, stdin);
        switch(int64FromString(buffer)){
        case 0:
            return;
            break;
        case 1:
            loginMenu();
            break;
        default:
            break;
        }
    }
    if(queues)
        free(queues);
}
