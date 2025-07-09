#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdbool.h>
#include <unistd.h>
#include"../../../headers/defs.h"
#include"../../../headers/stringParse.h"
#include"../../../headers/user.h"

int addPageInfo(char **string, int page, int itemsPerPage, int itemTotal, char *specialCtrls, char *itemType){
    char *pageExtras = malloc(sizeof(char)*256),
         pageInfo[256] = {'\0'},
         pageCur[256] = {'\0'},
         *move = NULL,
         *tmpctrls = NULL,
         pagetotal[1024] = {'\0'};
    int maxItemPrint = 0,
        maxPages = itemTotal/itemsPerPage,
        error = 0;
    strcpy(pageExtras, "\n+ pagina seguinte \n- pagina anterior\n");
    if(specialCtrls != NULL){
        tmpctrls = realloc(pageExtras, sizeof(char) * (256 + strlen(specialCtrls)));
        if(!tmpctrls){
            error = -1;
            goto cleanup;
        }
        pageExtras = tmpctrls;
        strcat(pageExtras, specialCtrls);
    }
    strcat(pageExtras, "0 sair\n");
    if(itemTotal%itemsPerPage != 0) maxPages++;
    if((page+1) * itemsPerPage > itemTotal){
        maxItemPrint = itemTotal;
    } else {
        maxItemPrint = itemsPerPage*(page+1);
    }
    sprintf(pageCur,"pagina %i de %i", (page+1), maxPages);
    sprintf(pageInfo,"%s %i a %i", itemType, (page*itemsPerPage)+1, maxItemPrint);
    if(strlen(pageCur) + strlen(pageInfo) < TXT_CONST){
        centerString(TXT_CONST/2, pageCur);
        centerString(TXT_CONST/2, pageInfo);
        strcat(pagetotal, pageCur);
        pagetotal[strlen(pageCur)] = '|';
        strcat(pagetotal, pageInfo);
    }else{
        pageCur[strlen(pageCur)+1] = '\n';
        pageCur[strlen(pageCur)+2] = '\0';
        strcat(pagetotal, pageCur);
        pageCur[strlen(pageInfo)+1] = '\n';
        pageCur[strlen(pageInfo)+2] = '\0';
        strcat(pagetotal, pageInfo);
    }
    strcat(pagetotal, "\n");
    strcat(pagetotal, "\n");
    //32 for safety reasons;
    move = malloc(sizeof(char) * strlen((*string)));
    if(!move){
        error = -1;
        goto cleanup;
    }
    strcpy(move, (*string));
    char *tmppage = realloc((*string), sizeof(char) * (strlen((*string)) + strlen(pagetotal) + strlen(pageExtras) + 32));
    if(!tmppage){
        error = -1;
        goto cleanup;
    }
    (*string) = tmppage ;
    strcpy((*string), pagetotal);
    strcat((*string), move);
    strcat((*string), pageExtras);
    goto cleanup;
cleanup:
    if(pageExtras)
        free(pageExtras);
    if(move)
        free(move);
    return error;
}

void returnText(char *where, int HowLong){
    printf("voltar para %s em %i\n", where, HowLong);
    sleep(1);
    for(int i = 1 ; i < HowLong ; i++){
        printf("%i\n",HowLong - i);
        sleep(1);
    }
    return;
}

int firstTimeLogic(FILE *in, FILE *create, int *first){
    in = fileopen(STUDENTDATA, "r");
    if(!in){
        create = fileopen(STUDENTDATA, "w");
        if(!create)
            return -1;
        fileclose(create);
    }
    in = fileopen(EVENTDATA, "r");
    if(!in){
        create = fileopen(EVENTDATA, "w");
        if(!create)
            return -1;
        fileclose(create);
    }
    in = fileopen(EVENTINDEX, "r");
    if(!in){
        create = fileopen(EVENTINDEX, "w");
        if(!create)
            return -1;
        fileclose(create);
    }
    in = fileopen(USERDATA, "r");
    if(!in){
        *first = 1;
        FILE *create = fileopen(USERDATA, "w");
        if(!create)
            return -1;
        fileclose(create);
    }
    if(readTotalUsers() == 0)
        *first = 1;
}

int firstTime(){
    int first = 0;
    FILE *in = NULL;
    FILE *create = NULL;
    firstTimeLogic(in, create, &first);
    if(in)
        fileclose(in);
    in = NULL;
    create = NULL;
    return first;
}
