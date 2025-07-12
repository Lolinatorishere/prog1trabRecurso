
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
        new.status = 1;
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

void eventIndexSort(int orderby){
    int page = 0;
    char *menuText = NULL;
    char buffer[256] = {'\0'};
    while(buffer[0] != '0'){
        getAllEvents(&menuText, queues, 5, &page, NULL, orderby);
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
        if(input == 0)
            return;
        eventIndexSort(input);
    }
}

void eventExportMenu(){
    char buffer[256] = {'\0'},
         dir[512] = {'\0'},
         buffer2[256] = {'\0'};
    EVENTS event = setEvent();
    STUDENTQUEUE *queue = {0};
    int64_t input = 0;
    while(1){
        menuPrint("eventExport", 1, 1);
        fgets(buffer, 256, stdin);
        switch(int64FromString(buffer)){
            case 0:
                return;
            case 1:
                advancedPrint("Evento para a exportar ", 1, 1, 0);
                fgets(buffer, 256, stdin);
                input = int64FromString(buffer);
                if(getEvent(&event, int64FromString(buffer)) != 0){
                    menuPrint("eventNonExists", 1, 1);
                    continue;
                }
                sprintf(dir, "participacaoEvento%ld.txt", input);
                queue = getEventQueue(queues, input);
                exportParticipationReport(input, *queue, dir);
                break;
            case 2:
                advancedPrint("Evento para a exportar ", 1, 1, 0);
                fgets(buffer, 256, stdin);
                input = int64FromString(buffer);
                if(getEvent(&event, input) != 0){
                    menuPrint("eventNonExists", 1, 1);
                    continue;
                }
                sprintf(dir, "AlunosEvento%ld.csv", input);
                queue = getEventQueue(queues, input);
                exportEventSubscriptions(input, *queue, dir);
                break;
            default:
                continue;
        }
    }
}


//    int limit; //max students
//    int status; //-1 canceled, 0 concluded, 1 planed
//    time_t date;//unix time, aka seconds since 1970
//    char location[256];
//    char eventName[256];
//    char eventDesc[256];


int editEventStudents(int eventId) {
    syscls;
    int input = 0,
        i = 0,
        studentId = 0;
    char buffer[256] = {'\0'},
         studentBuffer[512] = {'\0'},
         dir[512],
         *string = NULL;
    USERS user = setUser();
    STUDENTQUEUE *queue = NULL;
    STUDENTLIST *current = NULL,
                *node = NULL;
    LISTHELPER *fileupdate = NULL;
    queue = getEventQueue(queues, eventId);
    string = calloc((queue->total + 1) * (256 * 3), sizeof(char));
    if(!string )
        goto cleanup;
    while(1){
        strcpy(string, " ");
        current = queue->head;
        i = 0;
        while (current && i < queue->total) {
            getUser(&user, current->studentId);
            sprintf(studentBuffer, "ID: %d / nome: %s / Participou: %s\n",
                    current->studentId,
                    user.studentName ? user.studentName : "N/A",  // defensive
                    current->participou ? "Sim" : "Não");
            strcat(string, studentBuffer);
            current = current->next;
            i++;
        }
        if(queue->total == 0)
            addPageInfo(&string, 0, 1, 1, "\n1 - Adicionar Estudante\n2 - Remover Estudante\n3 - Alternar Participação\n0 - Voltar\n","Estudantes");
        else
            addPageInfo(&string, 0, queue->total, queue->total, "\n1 - Adicionar Estudante\n2 - Remover Estudante\n3 - Alternar Participação\n0 - Voltar\n","Estudantes");
        advancedPrint(string, 1, 1, 0);
        fgets(buffer, sizeof(buffer), stdin);
        switch(int64FromString(buffer)){
            case 1:
                printf("ID do novo estudante: ");
                fgets(buffer, sizeof(buffer), stdin);
                studentId = int64FromString(buffer);
                if(studentId < 1){
                    printf("\nid invalido\n");
                    sleep(1);
                    continue;
                }
                node = queue->head;
                while(node){
                    if(node->studentId == studentId){
                        printf("Estudante já existe.\n");
                        sleep(1);
                        break;
                    }
                    node = node->next;
                }
                if(node)
                    break;
                insertEnd(&queue->head, studentId, false);
                queue->tail = getLast(queue->head);
                queue->total++;
                printf("Estudante adicionado.\n");
                saveEventStudents(queue, eventId);
                refreshAllQueues(queues);
                loadEventStudents(queues);
                sleep(1);
                break;
            case 2:
                printf("ID do estudante a remover: ");
                fgets(buffer, sizeof(buffer), stdin);
                studentId = int64FromString(buffer);
                if(deleteStudentNode(&queue->head, studentId)){
                    queue->tail = getLast(queue->head);
                    queue->total--;
                    printf("Estudante removido.\n");
                } else {
                    printf("Estudante não encontrado.\n");
                }
                saveEventStudents(queue, eventId);
                refreshAllQueues(queues);
                loadEventStudents(queues);
                sleep(1);
                break;
            case 3:
                printf("ID do estudante a alterar participação: ");
                fgets(buffer, sizeof(buffer), stdin);
                studentId = int64FromString(buffer);
                node = queue->head;
                while (node) {
                    if (node->studentId == studentId) {
                        node->participou = !node->participou;
                        printf("Participação atualizada.\n");
                        break;
                    }
                    node = node->next;
                }
                if(!node)
                    printf("Estudante não encontrado.\n");
                sleep(1);
                break;
            case 0:
                goto cleanup;
            default:
                printf("Opção inválida.\n");
                sleep(1);
        }
    }
    goto cleanup;
cleanup:
    if(string)
        free(string);
    return 0;
}

int editingEvent(char **menuText, int page, int selectedID){
    EVENTS event = setEvent(),
           editer = setEvent();
    char editFunc[] = "\n1 - Nome\n2 - Descricao\n3 - Local\n4 - Limite\n5 - Estado\n6 - Data\n7 - Estudantes\n0 - Voltar\n",
         buffer[256];
    int input = 0,
        input1 = 0;
    if (getEvent(&event, selectedID) < 0 || event.eventId == -1) {
        menuPrint("eventNonExists", 1, 1);
        sleep(1);
        return -1;
    }
    while(1){
        strcpy((*menuText), "\n");
        editer = event;  // start with current event state
        searchForEventId(menuText, queues, selectedID);
        size_t newSize = strlen(*menuText) + strlen(editFunc) + 2;
        char *temp = realloc(*menuText, newSize);
        if (!temp) {
            printf("Erro de memória.\n");
            return -1;
        }
        *menuText = temp;
        strcat(*menuText, editFunc);
        advancedPrint(*menuText, 1, 1, 1);
        fgets(buffer, 256, stdin);
        if (buffer[0] == '0')
            break;
        input = int64FromString(buffer);
        if (buffer[0] == '\n' || buffer[0] == '\0')
            continue;
        switch(input){
            case 1:
                printf("\nNovo Nome: ");
                fgets(editer.eventName, sizeof(editer.eventName), stdin);
                trim(editer.eventName);
                if (strlen(editer.eventName) < 1)
                    continue;
                break;
            case 2:
                printf("\nNova Descrição: ");
                fgets(editer.eventDesc, sizeof(editer.eventDesc), stdin);
                trim(editer.eventDesc);
                if (strlen(editer.eventDesc) < 1) continue;
                break;
            case 3:
                printf("\nNovo Local: ");
                fgets(editer.location, sizeof(editer.location), stdin);
                trim(editer.location);
                if (strlen(editer.location) < 1)
                    continue;
                break;
            case 4:
                printf("\nNovo Limite de Estudantes: ");
                fgets(buffer, 256, stdin);
                input1 = int64FromString(buffer);
                if(input1 > 0)
                    editer.limit = input1;
                else
                    printf("Input Invalido");
                break;
            case 5:
                printf("Novo Estado\n1:planeado\n0:concluido\n2:cancelado\ninput:");
                fgets(buffer, 256, stdin);
                input1 = int64FromString(buffer);
                if(input1 > 0 && input < 2)
                    editer.status = input1;
                else if(input1 == 2)
                    editer.status = -1;
                else
                    printf("\nEstado Invalido\n");
                break;
            case 6:
                int day = 0,
                    month = 0,
                    year = 0;
                printf("\nNova Data (YYYY-MM-DD): ");
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
                editer.date = convertToTimestamp(day, month, year);
                break;
            case 7:
                editEventStudents(event.eventId);
                continue;
            default:
                printf("Opção inválida.\n");
                sleep(1);
                continue;
        }
        if(updateEvent(event.eventId, &editer)){
            printf("Erro ao atualizar evento.\n");
            sleep(1);
            return -1;
        }
        printf("Evento atualizado com sucesso.\n");
        event = editer;
        sleep(1);
        break;
    }
    return 0;
}

void eventAlterMenu(){
    char extras[256] = "sel \"ID\" para editar Evento\n";
    int page = 0;
    int input = 0;
    char buffer[256] = {'\0'};
    char *menuText = NULL;
    while(buffer[0] != '0'){
        input = 0;
        getAllEvents(&menuText, queues, 5, &page, extras, 0);
        advancedPrint(menuText, 1, 1, 1);
        fgets(buffer, 256, stdin);
        if(buffer[0] == '+')
            page++;
        if(buffer[0] == '-')
            page--;
        if(buffer[0] == 's' && buffer[1] == 'e' && buffer[2] == 'l')
            editingEvent(&menuText, page, int64FromString(buffer));
    }
    if(menuText)
        free(menuText);
    return;
}

void notifyEventMenu(){
    return;
}

void eventDeleteMenu(){
    EVENTS event = setEvent();
    STUDENTQUEUE *queue;
    int page = 0,
        selectedID = 0,
        str_cmp = 0;
    char confirm[256] = "C E R T E Z A",
         extras[256] = "sel \"ID\" para remover Evento\n",
         *menuText = NULL,
         toPrint[1024] = {'\0'},
         buffer[256] = {'\0'};
    while(buffer[0] != '0'){
        event = setEvent();
        getNonPlanedEvents(&menuText, queues, 5, &page, extras);
        advancedPrint(menuText, 1, 1, 1);
        fgets(buffer, 256, stdin);
        if(buffer[0] == '+')
            page++;
        if(buffer[0] == '-')
            page--;
        if(buffer[0] == 's' && buffer[1] == 'e' && buffer[2] == 'l'){
            selectedID = int64FromString(buffer);
            if(getEvent(&event, selectedID) != 0){
                advancedPrint("Nao existe Evento ", 1, 1, 0);
                sleep(1);
                goto cleanup;
            }
            if(event.status == 1){
                advancedPrint("Nao se pode apagar um evento Ativo\n ", 1, 1, 0);
                sleep(1);
                continue;
            }
            sprintf(toPrint, "Tem A certeza que quer apagar\nEvento:\"%s\"\nId:%i\nDescricao:%s\n", event.eventName, event.eventId, event.eventDesc);
            advancedPrint(toPrint, 1, 1, 0);
            printf("escreve: \"C E R T E Z A\", para confirmar");
            fgets(buffer, 256, stdin);
            if(str_cmp != strlen(confirm))
                continue;
            for(int i = 0 ; i < strlen(buffer); i++){
                if(buffer[i] != confirm[i])
                    break;
                str_cmp++;
            }
            if(deleteEvent(selectedID) != 0){
                menuPrint("Error", 4, 4);
                sleep(1);
                goto cleanup;
            }
            queue = getEventQueue(queues, selectedID);
            if(queue->total > 0){
                freeList((queue->head));
                queue->tail = NULL;
            }
            refreshAllQueues(queues);
            loadEventStudents(queues);
        }
    }
    goto cleanup;
cleanup:
    free(menuText);
    return;
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
