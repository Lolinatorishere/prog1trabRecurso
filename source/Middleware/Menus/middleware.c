
int addPageInfo(char **string, int page, int itemsPerPage, int itemTotal, char *specialCtrls, char *itemType){
    char *pageExtras = malloc(sizeof(char)*256);
    char pageInfo[256] = {'\0'};
    char pageCur[256] = {'\0'};
    char pagetotal[1024] = {'\0'};
    int maxUserPrint = 0;
    int maxPages = itemTotal/itemsPerPage;
    strcpy(pageExtras, "\n+ pagina seguinte \n- pagina anterior\n");
    if(specialCtrls != NULL){
        char *tmpctrls = realloc(pageExtras, sizeof(char) * (256 + strlen(specialCtrls)));
        if(!tmpctrls){
            free(pageExtras);
            return -1;
        }
        pageExtras = tmpctrls;
        strcat(pageExtras, specialCtrls);
    }
    strcat(pageExtras, "0 sair\n");
    if(itemTotal%itemsPerPage != 0) maxPages++;
    if((page+1) * itemsPerPage > itemTotal){
        maxUserPrint = itemTotal;
    }else{
        maxUserPrint = itemsPerPage*(page+1);
    }
    sprintf(pageCur,"pagina %i de %i", (page+1), maxPages);
    sprintf(pageInfo,"%s %i a %i", itemType, (page*itemsPerPage)+1, maxUserPrint);
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
    char *move = malloc(sizeof(char)*strlen((*string)));
    if(!move)
        return -1;
    strcpy(move, (*string));
    char *tmppage = realloc((*string), sizeof(char) * (strlen((*string)) + strlen(pagetotal) + strlen(pageExtras) + 32));
    if(!tmppage){
        free(pageExtras);
        return -1;
    }
    (*string) = tmppage ;
    strcpy((*string), pagetotal);
    strcat((*string), move);
    strcat((*string), pageExtras);
    free(pageExtras);
    free(move);
    return 0;
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
