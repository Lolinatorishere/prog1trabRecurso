
void NewEventMenu(){
    EVENTS new = setEvent();
    char buffer[256] = {'\0'},
         name[256] = {'\0'},
         desc[256] = {'\0'},
         locl[256] = {'\0'};
    int input = 0,
        userdone = 0,
        descdone = 0,
        locldone = 0,
        datedone = 0,
        limitdone = 0,
        day = 0,
        month = 0,
        year = 0;
    while(1){
        int date = 0;
        menuPrint("newEvent", 1, 1);
        if(userdone == 0 && name[0] == '\0'){
            printf("Nome do Evento:");
            fgets(name, 256, stdin);
            trim(name);
            if(strlen(name) == 0 || name[0] == '\n'){
                printf("Nome nao pode ser \"NULL\"\n");
                sleep(1);
                continue;
            }
            strcpy(new.eventName, name);
            userdone = 1;
        }
        if(locldone == 0){
            menuPrint("newEvent", 1, 1);
            printf("Lugar do Evento:");
            fgets(locl, 256, stdin);
            trim(locl);
            if(strlen(locl) == 0 || locl[0] == '\n'){
                printf("Lugar nao pode ser \"NULL\"\n");
                sleep(1);
                continue;
            }
            strcpy(new.location, locl);
            locldone = 1;
        }
        if(descdone == 0){
            menuPrint("newEvent", 1, 1);
            printf("Descricao do Evento:");
            fgets(desc, 256, stdin);
            trim(desc);
            if(strlen(desc) == 0 || desc[0] == '\n'){
                printf("Lugar nao pode ser \"NULL\"\n");
                sleep(1);
                continue;
            }
            strcpy(new.eventDesc, desc);
            descdone = 1;
        }
        if(datedone == 0 ){
            menuPrint("newEvent", 1, 1);
            printf("data do Evento yyyy/mm/dd\n");
            printf("yyyy: ");
            fgets(buffer, 256, stdin);
            year = int64FromString(buffer);
            if (year < 1900 || year > 2100) {
                printf("Erro Tente novamente.\n");
                continue;
            }
            printf("mm: ");
            fgets(buffer, 256, stdin);
            month = int64FromString(buffer);
            if (month < 1 || month > 12) {
                printf("Erro Tente novamente.\n");
                continue;
            }
            printf("dd: ");
            fgets(buffer, 256, stdin);
            day = int64FromString(buffer);
            if (day < 1 || day > 31) {
                printf("Erro Tente novamente.\n");
                continue;
            }
            new.date = convertToTimestamp(day, month, year);
            datedone = 1;
        }
        if(limitdone == 0){
            menuPrint("newEvent", 1, 1);
            printf("Limite:");
            fgets(buffer, 256, stdin);
            input = int64FromString(buffer);
            if(input < 1){
                printf("Limite tem de ser maior que 0");
                sleep(1);
                continue;
            }
            new.limit = input;
            limitdone = 0;
        }
        new.status = 0;
        if(createEvent(new) != 0){
            syscls;
            printf("Houve Um problema a criar o Evento");
            return;
        }
        printf("Evneto %s criado\n", name);
        sleep(1);
        return;
    }
}

void noEventSort(){
    int page = 0;
    char *menuText = NULL;
    char buffer[256] = {'\0'};
    while(buffer[0] != '0'){
        getAllEvents(&menuText, queues, 5, &page, NULL, 0);
        advancedPrint(menuText, 1, 1, 1);
        //handle menuInput
        fgets(buffer, 256, stdin);
        if(buffer[0] == '+')
            page++;
        if(buffer[0] == '-')
            page--;
    }
    free(menuText);
    return;
}

void eventIndexMenu(){
    char buffer[256];
    int64_t input = 0;
    while(1){
        menuPrint("eventIndex", 1, 1);
        fgets(buffer, 256, stdin);
        input = int64FromString(buffer);
        switch(input){
            case 1:
                noEventSort();
                continue;
            case 2:
                //stateEventSort();
                continue;
            case 3:
                //nameEventSort();
                continue;
            case 4:
                //dateEventSort();
                continue;
            case 0:
                return;
            default:
                continue;
        }
    }
}

void eventExportMenu(){

}

void eventAlterMenu(){
    
}

void notifyEventMenu(){

}

void eventDeleteMenu(){

}

void eventAdmin(){
    char buffer[256];
    while(1){
        menuPrint("eventControls", 1, 1);
        printf("input:");
        fgets(buffer, 256, stdin);
        int64_t input = int64FromString(buffer);
        //1 - Criar novo evento
        //2 - Ver eventos
        //3 - Exportar um evento
        //4 - Alterar um evento
        //5 - Notificar alunos de um evento
        //6 - Remover um Evento
        //0 - voltar
        switch(input){
            case 1:
                NewEventMenu();
                continue;
            case 2:
                eventIndexMenu();
                continue;
            case 3:
                eventExportMenu();
                continue;
            case 4:
                eventAlterMenu();
                continue;
            case 5:
                notifyEventMenu();
                continue;
            case 6:
                eventDeleteMenu();
                continue;
            case 0:
                return;
            default:
                continue;
        }
    }
}
