
void userEventIndex(){
    int page = 0;
    char *menuText = NULL;
    char buffer[256] = {'\0'};
    while(buffer[0] != '0'){
        getNonPlanedEvents(&menuText, queues, 5, &page, NULL, 1);
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

void enterEvent(USERS *user) {
    char buffer[256] = {'\0'};
    advancedPrint("Selecionar ID do evento para entrar:", 1, 1, 1);
    fgets(buffer, 256, stdin);
    int eventId = int64FromString(buffer);
    STUDENTQUEUE *queue = {0};
    EVENTS event = setEvent();
    queue = getEventQueue(queues, eventId);
    getEvent(&event, eventId);
    if (!queue || event.eventId == -1){
        printf("Evento não encontrado.\n");
        return;
    }
    STUDENTLIST *current = queue->head;
    while (current) {
        if (current->studentId != user->userId) {
            current = current->next;
            continue;
        }
        printf("ja esta inscrito neste evento.\n");
        return;
    }
    if (queue->total >= event.limit) {
        printf("O evento ja atingiu o maximo de participantes.\n");
        return;
    }
    insertQueue(queue, user->userId, 0);
    printf("Inscrição realizada com sucesso!\n");
    return;
}


void leaveEvent(USERS *user){
    char buffer[256] = {'\0'};
    advancedPrint("Selecionar ID do evento para sair:", 1, 1, 1);
    fgets(buffer, 256, stdin);
    int eventId = int64FromString(buffer);
    STUDENTQUEUE *queue = getEventQueue(queues, eventId);
    EVENTS event = setEvent();
    STUDENTLIST *current = queue->head;
    getEvent(&event, eventId);
    if (!queue || event.eventId == -1) {
        printf("Evento não encontrado.\n");
        return;
    }
    while (current) {
        if (current->studentId != user->userId)
            current = current->next;
        deleteStudentNode(&current, user->userId);
        queue->total--;
        printf("Inscrição removida com sucesso.\n");
        return;
    }
    printf("nao esta inscrito no evento.\n");
    return;
}

void userEventIndexMenu(USERS *user){
    char buffer[256];
    int64_t input = 0;
    while(1){
        menuPrint("userEventIndex", 1, 1);
        fgets(buffer, 256, stdin);
        input = int64FromString(buffer);
        if(input == 0)
            return;
        switch(input){
        case 1:
            userEventIndex(input);
            break;
        case 2:
            enterEvent(user);
            break;
        case 3:
            leaveEvent(user);
            break;
        default:
            break;
        }
    }
}

void listUserEvents(USERS *user){
    int found = 0;
    char string[512] = {'\0'},
         buffer[256] = {'\0'};
    int64_t eventCount = readTotalEvents();
    EVENTS event = setEvent();
    int day = 0,
        month = 0,
        year = 0;
    sprintf(string,"Eventos em que o usuario \"%s\" (ID: %d) esta inscrito:\n ", user->studentName, user->userId);
    advancedPrint(string, 1, 1, 0);
    printf("\n");
    for(int i = 0 ; i < eventCount ; i++) {
        STUDENTLIST *curr = queues[i].head;
        while(curr){
            if (curr->studentId != user->userId){
                curr = curr->next;
                continue;
            }
            getEvent(&event, queues[i].eventId);
            convertFromTimestamp(event.date, &day, &month, &year);
            printf("Evento ID: %d | Nome: %s | Local: %s | Data: %i/%i/%i\n",
                event.eventId,
                event.eventName,
                event.location,
                day, month, year
            );
            found++;
            break;
        }
    }
    if (found == 0)
        printf("nao esta inscrito em eventos.\n");
    printf("\nCarregue uma tecla para sair\n");
    fgets(buffer, 256, stdin);
}
