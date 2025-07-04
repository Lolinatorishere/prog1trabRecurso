#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdbool.h>
#include <time.h>
#include "../../../headers/structs.h"
#include "../../../headers/stringParse.h"
#include "../../../headers/defs.h"
#include "../../../headers/Menus/menuMiddleware.h"

EVENTS *searchEventId(EVENTS *eventList, int64_t listSize, int id, int64_t *index) {
    int64_t low = 0, high = listSize - 1;
    while (low <= high) {
        int64_t mid = low + (high - low) / 2;
        if (eventList[mid].eventId == id) {
            *index = mid;
            return &eventList[mid];
        } else if (eventList[mid].eventId < id) {
            low = mid + 1;
        } else {
            high = mid - 1;
        }
    }
    return NULL;
}

EVENTINDEXHELPER *searchEventStudentIndexId(EVENTINDEXHELPER *index, int64_t indexSize, int id, int64_t *indexPos) {
    int64_t low = 0, high = indexSize - 1;
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

EVENTS *searchEventName(EVENTS *eventList, int64_t listSize, char *eventName, int64_t *index) { // very slow
    int ulen = strlen(eventName);
    for(int64_t i = 0 ; i < listSize ; i++){
        int check = 0;
        if(ulen != strlen(eventList[i].name))continue;
        for(int j = 0 ; j < ulen; j++){
            if(eventList[i].name[j] != eventName[j])break;
            check++;
        }
        if(check != ulen) continue;
        return &eventList[i];
    }
    return NULL;
}

int updateEventData(EVENTS eventList[], int64_t listSize){
    FILE *fp = fopen(EVENTDATA, "wb");
    if(!fp)
        return -1;
    if(fwrite(&listSize, sizeof(int64_t), 1, fp) != 1 ) return -1;
    for(int i = 0 ; i < listSize ; i++){
        eventList[i].participants = NULL;
        if(fwrite(&eventList[i], sizeof(EVENTS), 1, fp) != 1) return -1;
    }
    fclose(fp);
    return 1;
}

//adds new index in sorted position
int insertSortedStudentIndex(EVENTINDEXHELPER *index, EVENTINDEXHELPER new, int size){
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
    index[result].eventId = new.eventId;
    index[result].howManyStudents = new.howManyStudents;
    index[result].offset = 0;
    return result;
}

int updateEventStudentIndex(int *eventStudents, int totalStudents, int eventId, int new){
    //unfortunately we have to load the entire file before updating
    //because we have no idea how the new memory layout will look like
    int total = 0;
    int64_t indexstudentpos = 0,
            indexwritepos = 0,
            indexPos = -1;
    EVENTINDEXHELPER newIndex = {eventId, totalStudents, 0},
                     *curentIndex = NULL;
    FILE *fp = fopen(EVENTINDEX, "rb");
    if(!fp)
        return -1;
    fread(&total, sizeof(int), 1, fp);
    int **studentIds = malloc(sizeof(int*) * total + 1);
    if(!studentIds){
        fclose(fp);
        return -1;
    }
    EVENTINDEXHELPER *index = malloc(sizeof(EVENTINDEXHELPER) * total + 1);
    if(!index){
        free(studentIds);
        fclose(fp);
        return -1;
    }
    if(searchEventStudentIndexId(index, total, eventId, &indexPos) && new == 1){
        free(index);
        free(studentIds);
        fclose(fp);
        return 1;
    }
    fread(&index, sizeof(EVENTINDEXHELPER), total, fp);
    for(int i = 0 ; i < total ; i++){
        studentIds[i] = malloc(sizeof(int)*index[i].howManyStudents + 1);
        fread(&studentIds[i], sizeof(int), index[i].howManyStudents, fp);
    }
    if(new == 1){
        indexPos = insertSortedStudentIndex(index, newIndex, total);
        //loads new students at the end of the malloc'ed array
        studentIds[total] = malloc(sizeof(int)*totalStudents + 1);
        for(int i = 0 ; i < totalStudents ; i++)
            studentIds[total][i] = eventStudents[i];
        total++;
    }else{
        index[indexPos].howManyStudents = totalStudents;
    }
    fclose(fp);
    fp = fopen(EVENTINDEX, "wb");
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
        fseek(fp, sizeof(int) + (sizeof(EVENTINDEXHELPER) * i) + sizeof(int) * 2, SEEK_SET);
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

int loadStudentsFromEventIndex(EVENTS *event){
    int total = 0;
    int64_t indexPos = -1;
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
    curentIndex = searchEventStudentIndexId(index, total, event->eventId, &indexPos);
    if(!curentIndex) {
        free(index);
        fclose(fp);
        return -1;
    }
    fseek(fp, curentIndex->offset, SEEK_SET);
    int students = curentIndex->howManyStudents;;
    if(students < 1){
        free(index);
        fclose(fp);
        return -1;
    }
    if(curentIndex->howManyStudents > event->limit)
        students = event->limit;
    fread(event->participants, sizeof(int), students, fp);
    free(index);
    fclose(fp);
    return 0;
}


int loadEventData(EVENTS *eventList) {
    FILE *fp = fopen(EVENTDATA, "rb");
    if(!fp) return -1;
    fseek(fp, 0, SEEK_END);
    if(ftell(fp) == 0){
        fclose(fp);
        return 1;
    }
    fseek(fp, 0, SEEK_SET);
    int64_t eventTotal = 0;
    fread(&eventTotal, sizeof(int64_t), 1, fp);
    if(eventTotal == 0){
        fclose(fp);
        return 1;
    }
    for(int i = 0 ; i < eventTotal ; i++){
        fread(&eventList[i], sizeof(EVENTS), 1, fp);
    }
    fclose(fp);
    return 0;
}

int64_t readTotalEvents(){
    int64_t totalEvents = 0;
    FILE *fp = fopen(EVENTDATA, "rb");
    if(!fp){
        //guarantees the existance of EVENTDATA dir file
        FILE *makeFile = fopen(EVENTDATA, "wb");
        fclose(makeFile);
        fp = fopen(EVENTDATA, "rb");
        if(!fp)
            //or not, something wack has happened
            return 0;
    }
    fseek(fp, 0, SEEK_END);
    if(ftell(fp) == 0){
        fclose(fp);
        return 0;
    }
    fseek(fp, 0, SEEK_SET);
    fread(&totalEvents, sizeof(int64_t), 1, fp);
    fclose(fp);
    if(totalEvents == 0) totalEvents = 1;
    return totalEvents;
}

//typedef struct events
//    int eventID;
//    char name[256];
//    time_t date;//unix time, aka seconds since 1970
//    char dateString[32]; //self described
//    char location[256];
//    int limit; //max students
//    int status; //-1 canceled, 0 planned, 1 concluded
//    int *participants;//should only be a place in active memory not part of the struct, to avoid memory disalignment
//EVENTS;

int createEvent(char *eventName, time_t date, char *location, int limit){
    int64_t eventTotal = readTotalEvents();
    EVENTS  *events = malloc(sizeof(EVENTS) * (eventTotal + 1));
    switch(loadEventData(events)){
        case 0:
            break;
        case -1:
            free(events);
            return -1;
        default:
            break;
    }
    EVENTS *newEvent = &events[eventTotal];
    strncpy(newEvent->name, eventName, 255);
    newEvent->name[255] = '\0';
    newEvent->date = date;
    strncpy(newEvent->location, location, 255);
    newEvent->name[255] = '\0';
    newEvent->limit = limit;
    newEvent->status = 0;
    newEvent->participants = NULL;
    eventTotal++;
    if(updateEventData(events, eventTotal) == -1)
        return -1;
    free(events);
    return 1;
}

//typedef struct events
//    int eventID;
//    char name[256];
//    time_t date;//unix time, aka seconds since 1970
//    char dateString[32]; //self described
//    char location[256];
//    int limit; //max students
//    int status; //-1 canceled, 0 planned, 1 concluded
//EVENTS;

int updateEvent(int eventId, char *eventName, time_t *date, char *location, int *limit, int *status){
    if(eventName == NULL)return 1;
    int error = 0;
    int64_t index = 0,
            eventTotal = readTotalEvents();
    EVENTS *events = malloc(sizeof(EVENTS) * (eventTotal+1));
    if(!events){
        return -1;
    }
    switch(loadEventData(events)){
        case -1:
            return -1;
        case 1:
            return 1;
        default:
            if(!searchEventId(events, eventTotal, eventId, &index)){
                error = 2;
                break;
            }
            if(eventName[0] != '\0'){
                eventName[strlen(eventName)] = '\0';
                strcpy(events[index].name, eventName);
            }
            updateEventData(events, eventTotal);
            break;
    }
    free(events);
    return error;
}

int deleteEvent(int eventId){
    int64_t index = 0;
    int64_t eventTotal = readTotalEvents();
    EVENTS *events = malloc(sizeof(EVENTS) * (eventTotal + 1)),
           *check = NULL,
            event;
    if(!events) return -1;
    switch(loadEventData(events)){
        case -1:
            return -1;
        case 1:
            return 1;
        default:
            if(!searchEventId(events, eventTotal, eventId, &index)){
                free(events);
                return 2;
            }
            if(events[index].status == 0)
                return 2;
            // aray[x] replaced with [x+n];
            // because i dont know how to do it better kek;
            if(index != eventTotal-1){
                for(int i = index ; i < eventTotal-1 ; i++){
                    events[i] = events[i+1];
                }
            }
            updateEventData(events, eventTotal-1);
            free(events);
            return 0;
    }
}

int getEvent(EVENTS *event, int id){
    int64_t eventTotal = readTotalEvents();
    int64_t index = 0;
    EVENTS *events = malloc(sizeof(EVENTS) * (eventTotal + 1));
    if(loadEventData(events) != 0){
        free(events);
        return -1;
    }
    event = searchEventId(events, eventTotal, id, &index);
    if(event == NULL){
        free(events);
        return 1;
    }
    return 0;
}

//int getAllEvents(char **string, int eventsPerPage, int *page, char *special){
//    int64_t eventsTotal = readTotalEvents();
//    if(eventsTotal == 0) return -1;
//    int maxPages = eventsTotal/eventsPerPage;
//    if(eventsTotal%eventsPerPage != 0){
//        maxPages++;
//    }
//    if (*page >= maxPages) *page = maxPages-1;
//    if(*page<0)*page = 0;
//    EVENTS *events = malloc(sizeof(EVENTS) * (eventsTotal + 1));
//    if(!events)return -1;
//    if(loadEventData(events) != 0){
//        free(events);
//        return -1;
//    }
//    if(createEventsString(string, events, eventsTotal, eventsPerPage, *page) != 0){
//        free(events);
//        return -1;
//    }
//    addPageInfo(string, *page, eventsPerPage, eventsTotal, special, "Events");
//    free(events);
//    return 0;
//}
//
//int searchForEvent(char **string, char *search, int Event, int page){
//    int64_t eventTotal = readTotalEvents();
//    EVENTS *events = malloc(sizeof(EVENTS) * (eventTotal + 1));
//    int64_t index = 0;
//    if(eventTotal == 0) return -1;
//    if(loadEventData(events) != 0){
//        free(events);
//        return 1;
//    }
//    EVENTS *event = searchEventName(events, eventTotal, search, &index);
//    if(!event){
//        //todo work with  instead
//        free(events);
//        return 1;
//    }
//    if(createEventsString(string, events, 1, 1, 0) != 0){
//        free(events);
//        return -1;
//    }
//    free(events);
//    return 0;
//}
//
//int searchForEventID(char **string, int search, int Event, int page){
//    int64_t eventTotal = readTotalEvents();
//    EVENTS *events = malloc(sizeof(EVENTS) * (eventTotal + 1));
//    int64_t index = 0;
//    if(eventTotal == 0) return -1;
//    if(loadEventData(events) != 0){
//        free(events);
//        return 1;
//    }
//    EVENTS *event = searchEventId(events, eventTotal, search, &index);
//    if(event == NULL){
//        (*string) = malloc(sizeof(char) * 64);
//        strcpy((*string), "Aluno Nao Existe\n");
//    }else{
//        if(createEventsString(string, event, 1, 1, 0) != 0){
//            free(events);
//            return -1;
//        }
//    }
//    free(events);
//    return 0;
//}

//int getAllEvents(char **string, int eventsPerPage, int *page, char *special){
