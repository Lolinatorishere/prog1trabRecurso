
STUDENTLIST *createStudentNode(int studentId, bool participou){
    STUDENTLIST *newNode = malloc(sizeof(STUDENTLIST));
    if(!newNode)
        return NULL;
    newNode->studentId = studentId;
    newNode->participou = participou;
    newNode->next = NULL;
    newNode->prev = NULL;
    return newNode;
}

void insertEnd(STUDENTLIST **head, int studentId, bool participou){
    STUDENTLIST *newNode = createStudentNode(studentId, participou);
    if(!newNode)
        return;
    if(*head == NULL){
        *head = newNode;
        return;
    }
    STUDENTLIST *temp = *head;
    while(temp->next != NULL)
        temp = temp->next;
    temp->next = newNode;
    newNode->prev = temp;
}

void deleteStudentNode(STUDENTLIST **head, int studentId){
    if(*head == NULL)
        return;
    STUDENTLIST *temp = *head;
    while(temp != NULL && temp->studentId != studentId)
        temp = temp->next;
    if(temp == NULL)
        return;
    if(temp->prev != NULL)
        temp->prev->next = temp->next;
    else
        *head = temp->next;
    if(temp->next != NULL) 
        temp->next->prev = temp->prev;
    free(temp);
}

STUDENTLIST *searchStudentNode(STUDENTLIST *head, int studentId){
    STUDENTLIST *temp = head;
    while(temp != NULL){
        if(temp->studentId == studentId)
            return temp;
        temp = temp->next;
    }
    return NULL;
}

void freeList(STUDENTLIST *head){
    STUDENTLIST *temp;
    while(head != NULL){
        temp = head;
        head = head->next;
        free(temp);
    }
}
