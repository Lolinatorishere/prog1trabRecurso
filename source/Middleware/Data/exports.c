
void exportParticipationReport(int eventId, STUDENTQUEUE queue, const char *filename) {
    FILE *file = fopen(filename, "w");
    USERS user = setUser();
    if (!file) {
        perror("Erro ao abrir o ficheiro de relatório");
        return;
    }
    fprintf(file, "Relatório de Participação - Evento ID: %d\n\n", eventId);
    fprintf(file, "ID Aluno | Nome Aluno | Participou\n");
    fprintf(file, "-------------------------------------\n");
    STUDENTLIST *current = queue.head;
    while (current) {
        const char *studentName = "Desconhecido";
        char participou[64] = {'\0'};
        if(current->participou == 1)
            strcpy(participou, "Sim");
        else
            strcpy(participou,"Nao");
        getUser(&user, current->studentId);
        fprintf(
            file,
            "%-9d | %-20s | %s\n",
            current->studentId,
            user.studentName,
            participou
        );
        current = current->next;
    }
    fclose(file);
    printf("Relatório de participação gerado: %s\n", filename);
}

void exportEventSubscriptions(int eventId, STUDENTQUEUE queue, const char *filename){
    FILE *file = fopen(filename, "w");
    USERS user = setUser();
    EVENTS event = setEvent();
    if (!file) {
        perror("Erro ao criar ficheiro de exportação");
        return;
    }
    getEvent(&event, eventId);
    fprintf(file, "Lista de Inscritos - Evento: %s (ID: %d)\n", event.eventName, event.eventId);
    fprintf(file, "ID Aluno, Nome Aluno, Participou\n");
    STUDENTLIST *curr = queue.head;
    while (curr) {
        getUser(&user, curr->studentId);
        const char *studentName = "Desconhecido";
        char participou[64] = {'\0'};
            if(curr->participou == 1)
                strcpy(participou, "Sim");
            else
                strcpy(participou, "Nao");
        fprintf(file, "%d, %s, %s\n", curr->studentId, user.studentName, participou);
        curr = curr->next;
    }
    fclose(file);
    printf("Lista exportada para: %s\n", filename);
}
