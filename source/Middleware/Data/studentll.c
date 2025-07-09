
// Function to create a new node
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

// Function to delete a node by studentId
void deleteStudentNode(STUDENTLIST **head, int studentId){
    if(*head == NULL)
        return;
    STUDENTLIST* temp = *head;
    while(temp != NULL && temp->studentId != studentId)
        temp = temp->next;
    if(temp == NULL)
        return; // Node not found
    if(temp->prev != NULL) 
        temp->prev->next = temp->next;
    else
        *head = temp->next; // Deleting the head node
    if(temp->next != NULL) 
        temp->next->prev = temp->prev;
    free(temp);
}

// Function to search for a node by studentId
STUDENTLIST *searchStudentNode(STUDENTLIST *head, int studentId){
    STUDENTLIST *temp = head;
    while(temp != NULL){
        if(temp->studentId == studentId)
            return temp;
        temp = temp->next;
    }
    return NULL; // Node not found
}

// Function to display the list
void displayList(STUDENTLIST *head){
    STUDENTLIST *temp = head;
    while(temp != NULL){
        printf("Student ID: %d, Participou: %s\n", temp->studentId, temp->participou ? "true" : "false");
        temp = temp->next;
    }
}

// Function to free the entire list
void freeList(STUDENTLIST *head){
    STUDENTLIST *temp;
    while(head != NULL){
        temp = head;
        head = head->next;
        free(temp);
    }
}
