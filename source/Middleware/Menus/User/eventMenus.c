
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
    insertQueue(queue, user->userId, false);
    saveEventStudents(queue, eventId);
    refreshAllQueues(queues);
    loadEventStudents(queues);
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
        saveEventStudents(queue, eventId);
        refreshAllQueues(queues);
        loadEventStudents(queues);
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

void userInscriptioins(USERS *user){
    
    return;
}
