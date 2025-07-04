
int createEventsString(char **string, EVENTS *Events, int EventTotal, int eventsPerPage, int page){
    /* should return something with this type of structure
     * / Id:id
     * \ Aluno: id
    */
    *string = malloc(sizeof(char) * (eventsPerPage * (700)));
    int index = 0;
    char buffer[TXT_CONST];
    int ut_id = strlen(" / Id:");
    int ut_name = strlen(" \\ Aluno:");
    strcpy((*string), "\0");
    if(TXT_CONST-ut_name <= ut_name) return -1;
    for(int i = (page * eventsPerPage) ; i < eventsPerPage + (page * eventsPerPage) ; i++){
        if(i >= EventTotal) break;
        EVENTS event = Events[i];
        strcat((*string), " / Id:"); index += ut_id;
        sprintf(buffer, "%i\n", event.eventId);
        strcat((*string), buffer); index += strlen(buffer);
        int name_len = strlen(event.name);
        if(name_len < TXT_CONST-ut_name){
            strcat((*string), " \\ Aluno:"); index += ut_name;
            strcat((*string), event.name); index += name_len;
        }else{
            strcat((*string), " | Aluno:"); index += ut_name;
            for(int j = 0 ; j < name_len ; j++){//needs testing doesnt work needs to test/fix
                (*string)[j+index] = event.name[j];
                if(j%TXT_CONST-ut_name == 0){
                    strcat((*string), " |       "); index += ut_name;
                    continue;
                }
                index++;
            }
        }
        strcat((*string), "\n"); index++;
    }
    (*string)[index+1]='\0';
    return 0;
}

int getAllEvents(char **string, int eventsPerPage, int *page, char *special){
    int64_t eventsTotal = readTotalEvents();
    if(eventsTotal == 0) return -1;
    int maxPages = eventsTotal/eventsPerPage;
    if(eventsTotal%eventsPerPage != 0){
        maxPages++;
    }
    if (*page >= maxPages) *page = maxPages-1;
    if(*page<0)*page = 0;
    EVENTS *events = malloc(sizeof(EVENTS) * (eventsTotal + 1));
    if(!events)return -1;
    if(loadEventData(events) != 0){
        free(events);
        return -1;
    }
    if(createEventsString(string, events, eventsTotal, eventsPerPage, *page) != 0){
        free(events);
        return -1;
    }
    addPageInfo(string, *page, eventsPerPage, eventsTotal, special, "Events");
    free(events);
    return 0;
}

int searchForEvent(char **string, char *search, int Event, int page){
    int64_t eventTotal = readTotalEvents();
    EVENTS *events = malloc(sizeof(EVENTS) * (eventTotal + 1));
    int64_t index = 0;
    if(eventTotal == 0) return -1;
    if(loadEventData(events) != 0){
        free(events);
        return 1;
    }
    EVENTS *event = searchEventName(events, eventTotal, search, &index);
    if(!event){
        //todo work with  instead
        free(events);
        return 1;
    }
    if(createEventsString(string, events, 1, 1, 0) != 0){
        free(events);
        return -1;
    }
    free(events);
    return 0;
}

int searchForEventID(char **string, int search, int Event, int page){
    int64_t eventTotal = readTotalEvents();
    EVENTS *events = malloc(sizeof(EVENTS) * (eventTotal + 1));
    int64_t index = 0;
    if(eventTotal == 0) return -1;
    if(loadEventData(events) != 0){
        free(events);
        return 1;
    }
    EVENTS *event = searchEventId(events, eventTotal, search, &index);
    if(event == NULL){
        (*string) = malloc(sizeof(char) * 64);
        strcpy((*string), "Aluno Nao Existe\n");
    }else{
        if(createEventsString(string, event, 1, 1, 0) != 0){
            free(events);
            return -1;
        }
    }
    free(events);
    return 0;
}

EVENTINDEXHELPER *searchEventIndexId(EVENTINDEXHELPER *index, int64_t indexSize, int id, int64_t *indexPos) {
    int64_t low = 0, high = listSize - 1;
    while (low <= high) {
        int64_t mid = low + (high - low) / 2;
        if (index[mid].eventId == id) {
            *indexPos = mid;
            return &index[mid];
        } else if (index[mid].eventId < id) {
            low = mid + 1;
        } else {
            high = mid - 1;
        }
    }
    return NULL;
}

int loadStudentsFromEventIndex(EVENTS *event){
    int total = 0;
    int indexPos = -1;
    EVENTINDEXHELPER *curentIndex = NULL;
    FILE *fp = fopen(EVENTINDEX, "rb");
    if(!fp)
        return -1;
    fread(&total, sizeof(int), 1, fp);
    EVENTINDEXHELPER *index = malloc(sizeof(EVENTINDEXHELPER) * total + 1);
    if(!index){
        fclose(fp);
        return -1;
    }
    fread(&index, sizeof(EVENTINDEXHELPER), total, fp);
    curentIndex = searchEventIndexId(index, total, event->eventId, &indexPos);
    if(!curentIndex) {
        free(index);
        fclose(fp);
        return -1
    }
    fseek(fp, curentIndex.offset, SEEK_SET);
    int students = curentIndex.howManyStudents;;
    if(students < 1){
        free(index);
        fclose(fp);
        return -1;
    }
    if(curentIndex.howManyStudents > event->limit)
        students = event->limit;
    fread(event->participants, sizeof(int), students, fp);
    free(index);
    fclose(fp);
    return 0;
}

//adds new index in sorted position
int sortIndex(EVENTINDEXHELPER *index, EVENTINDEXHELPER new, int sizeventId){
    int left = 0,
        right = size - 1;
    int result = -1;
    //obtain the index right before 
    while (left <= right) {
        int mid = left + (right - left) / 2;
        if (index[mid].eventId < new.eventId) {
            result = mid;
            left = mid + 1;
        } else {
            right = mid - 1;
        }
    }
    if(result == -1)
        return -1;
    for(int i = size ; i > result; i--){
        index[i] = index[i-1];
    }
    index[result].eventId = new.eventId
    index[result].howManyStudents = new.howManyStudents;
    index[result].offset = 0;
    return result;
}

int updateEventStudentIndex(int *eventStudents, int totalStudents, int eventId, int new){
    //unfortunately we have to load the entire file before updating
    //because we have no idea how the new memory layout will look like
    int total = 0,
        indexPos = -1;
    int64_t indexstudentpos = 0,
            indexwritepos = 0;
    EVENTINDEXHELPER newIndex = {eventId, totalStudents, 0},
                     *curentIndex = NULL;
    FILE *fp = fopen(EVENTINDEX, "rb");
    if(!fp)
        return -1;
    fread(&total, sizeof(int), 1, fp);
    int **studentIds = malloc(sizeof(int*) * total + 1);
    if(!students){
        fclose(fp);
        return -1;
    }
    EVENTINDEXHELPER *index = malloc(sizeof(EVENTINDEXHELPER) * total + 1);
    if(!index){
        free(studentIds);
        fclose(fp);
        return -1;
    }
    if(searchEventIndexId(index, total, eventId, &indexPos) && new == 1){
        free(index);
        free(studentIds);
        fclose(fp);
        return 1;
    }
    fread(&index, sizeof(EVENTINDEXHELPER), total, fp);
    for(int i = 0 ; i < total ; i++){
        int studentIds[i] = malloc(sizeof(int)*index[i].howManyStudents + 1);
        fread(studentIds[i], sizeof(int), index[i].howManyStudents, fp);
    }
    if(new == 1){
        indexPos = sortIndex(index, newIndex, total);
        //loads new students at the end of the malloced array
        studentIds[total] = malloc(sizeof(int)*totalStudents + 1);
        for(int i = 0 ; i < totalStudents ; i++)
            studentIds[total][i] = eventStudents[i];
        total++;
    }else{
        index[indexPos].howManyStudents = totalStudents;
    }
    fclose(fp);
    *fp = fopen(EVENTINDEX, "wb");
    if(!fp){
        free(index);
        for(int i = 0 ; i < total ; i++)
            free(studentIds[i]);
        free(studentIds);
        fclose(fp);
        return -1;
    }
    fwrite(&total, sizeof(int), 1, fp);
    fwrite(index, sizeof(EVENTINDEXHELPER), total, fp);
    //file updating here
    //has double pointer positions to allow for jumping between
    //the index and the data
    int iterator = 0;
    for(int i = 0 ; i < total - new ; i++){
        indexstudentpos = ftell(fp);
        fwrite(studentIds[iterator], sizeof(int), index[i].howManyStudents, fp);
        indexwritepos = ftell(fp);
        /*
         *How im editing the index portion of the file
         *|head: sizof(total) | + |index: sizeof(struct)*i | + |editoffset: current sizeof(eventId) + current (totalStudents)|
         *fwrite now has the correct position in the file to write 
         * */
        fseek(fp, sizeof(int) + (sizeof(EVENTINDEXHELPER) * i) + sizeof(int*2), SEEK_SET);
        //writes index[i].offset;
        fwrite(&indexstudentpos, sizeof(int64_t), 1, fp);
        fseek(fp, indexwritepos, SEEK_SET);
        //because the new item is always written at the end of the array
        //we check when the new index appears
        //and then tell iterator to the end of the new memory block
        if(new != 1){
            iterator = i;
            continue;
        }
        if(indexPos == i+1){
            iterator = total;
            continue;
        }
        if(iterator >= i){
            iterator = i - 1;
            continue;
        }
        iterator = i;
    }
    free(index);
    for(int i = 0 ; i < total ; i++)
        free(studentIds[i]);
    free(studentIds);
    fclose(fp);
    return 0;
}

