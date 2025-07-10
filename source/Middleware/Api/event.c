#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdbool.h>
#include <time.h>
#include "../../../headers/structs.h"
#include "../../../headers/stringParse.h"
#include "../../../headers/defs.h"
#include "../../../headers/Menus/menuMiddleware.h"
#include "../../../headers/Data/dataMiddleware.h"

int64_t TotalEventAmount = 0;

EVENTS setEvent(){
    EVENTS event = {-1, -1, -2, 0, '\0', '\0', '\0' };
    return event;
}

EVENTS *searchEventname(EVENTS *eventList, int64_t listSize, char *eventname, int64_t *index) { // very slow
    if(listSize == 0)
        return NULL;
    int ulen = strlen(eventname);
    for(int64_t i = 0 ; i < listSize ; i++){
        int check = 0;
        if(ulen != strlen(eventList[i].eventName))continue;
        for(int j = 0 ; j < ulen ; j++){
            if(eventList[i].eventName[j] != eventname[j])break;
            check++;
        }
        if(check != ulen) continue;
        return &eventList[i];
    }
    return NULL;
}

//This is assuming the ids in the List are sequential which they absolutely should be
//binary searches the requested id
//
EVENTS *searchEventId(EVENTS *eventList, int64_t listSize, int id, int64_t *index) {
    if(listSize == 0)
        return NULL;
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

int updateEventData(EVENTS *eventList, int64_t listSize){
    FILE *fp = fopen(EVENTDATA, "wb");
    if (!fp)
        return -1;
    if (fwrite(&listSize, sizeof(int64_t), 1, fp) != 1){
        fclose(fp);
        return -1;
    }
    if (listSize > 0 && fwrite(eventList, sizeof(EVENTS), listSize, fp) != listSize){
        fclose(fp);
        return -1;
    }
    fclose(fp);
    return 1;
}

int64_t readTotalEvents(){
    int64_t totalEvents = 0;
    FILE *fp = fopen(EVENTDATA, "rb");
    if(TotalEventAmount == 0){
        if(!fp)
            return 0;
        if(fseek(fp, 0, SEEK_END) != 0)
            goto cleanup;
        if(ftell(fp) == 0)
            goto cleanup;
        if(fseek(fp, 0, SEEK_SET) != 0)
            goto cleanup;
        fread(&totalEvents, sizeof(int64_t), 1, fp);
        TotalEventAmount = totalEvents;
        goto cleanup;
    }
    totalEvents = TotalEventAmount;
    goto cleanup;
cleanup:
    if(fp)
        fclose(fp);
    fp = NULL;
    return totalEvents;
}

int loadEventData(EVENTS *eventList) {
    FILE *fp = fopen(EVENTDATA, "rb");
    int error = 0;
    if(!fp)
        return -1;
    fseek(fp, 0, SEEK_END);
    if(ftell(fp) == 0){
        error = 1;
        goto cleanup;
    }
    fseek(fp, 0, SEEK_SET);
    int64_t eventTotal = 0;
    fread(&eventTotal, sizeof(int64_t), 1, fp);
    if(eventTotal == 0){
        error = 1;
        goto cleanup;
    }
    for(int i = 0 ; i < eventTotal ; i++)
        fread(&eventList[i], sizeof(EVENTS), 1, fp);
    goto cleanup;
cleanup:
    fclose(fp);
    fp = NULL;
    return 0;
}

int createEventString(char **string, EVENTS *events, STUDENTQUEUE *queue, int eventTotal, int eventsPerPage, int page){
    /* should return something with this type of structure
     * / nome:eventName 
     * | Id: eventID
     * | data: date converted
     * | estado: state
     * | total : inscritos
     * \ limit: limit
     */
     if(*string == NULL){
        *string = malloc(sizeof(char) * (eventsPerPage * (256 * 6)));
    } else {
        char *temp = realloc((*string), sizeof(char) * (eventsPerPage * (256 * 6)));
        (*string) = temp;
    }
    char buffer[256];
    int index = 0,
        ut_nome =   strlen(" / Nome: "),
        ut_id =     strlen(" | Id: "),
        ut_estado = strlen(" | Estado: "),
        ut_data =   strlen(" | Data: "),
        ut_total =  strlen(" | Total: "),
        ut_limit =  strlen(" \\ Limite: ");
    strcpy((*string), "\0");
    if(TXT_CONST - ut_nome <= ut_nome)
        return -1;
    for(int i = (page * eventsPerPage) ; i < eventsPerPage + (page * eventsPerPage) ; i++){
        if(i >= eventTotal)
            break;
        EVENTS event = events[i];
        STUDENTQUEUE *eventQueue = getEventQueue(queue, event.eventId);
        int total = eventQueue->total;
        if(!eventQueue)
            total = 0;
        int day = 0,
            month = 0,
            year = 0;
        convertFromTimestamp(event.date, &day, &month, &year);
        strcat((*string), " / Nome:"); index += ut_nome;
        strcat((*string), event.eventName); index += strlen(event.eventName);
        strcat((*string), " | ID:"); index += ut_id;
        sprintf(buffer, "%i\n", event.eventId); strcat((*string), buffer); index += strlen(buffer);
        strcat((*string), " | Estado:"); index += ut_estado;
        if(event.status == 1){
            strncpy(buffer, "Ativo\n", 256);
            strcat((*string), buffer);
            index += strlen(buffer);
        }else if(event.status == 0){
            strncpy(buffer, "Concluido\n", 256);
            strcat((*string), buffer);
            index += strlen(buffer);
        }else{
            strncpy(buffer, "Cancelado\n", 256);
            strcat((*string), buffer);
            index += strlen(buffer);
        }
        strcat((*string), " | Data:"); index += ut_data;
        sprintf(buffer, "%i/%i/%i\n", day, month, year); strcat((*string), buffer); index += strlen(buffer);
        strcat((*string), " | Total:"); index += ut_total;
        sprintf(buffer, "%i\n", total); strcat((*string), buffer); index += strlen(buffer);
        strcat((*string), " \\ Limite:"); index += ut_total;
        sprintf(buffer, "%i\n", event.limit); strcat((*string), buffer); index += strlen(buffer);
    }
    (*string)[index+1]='\0';
    return 0;
}

int createEvent(EVENTS event){
    int64_t eventTotal = readTotalEvents(),
            index = 0;
    EVENTS *events = NULL;
    events = malloc(sizeof(EVENTS) * (eventTotal + 1));
    if(!events)
        return -1;
    int error = loadEventData(events);
    if(error == -1)
        goto cleanup;
    if(eventTotal == 0)
        events[eventTotal].eventId = 1;
    else
        events[eventTotal].eventId = events[eventTotal - 1].eventId + 1;
    events[eventTotal].limit = event.limit;
    events[eventTotal].status = event.status;
    events[eventTotal].date = event.date;
    strncpy(events[eventTotal].location, event.location, 255);
    strncpy(events[eventTotal].eventName, event.eventName, 255);
    strncpy(events[eventTotal].eventDesc, event.eventDesc, 255);
    eventTotal++;
    if(updateEventData(events, eventTotal) == -1){
        error = -1;
        goto cleanup;
    }
    TotalEventAmount = eventTotal;
    goto cleanup;
cleanup:
    free(events);
    return error;
}

int updateEvent(int id, EVENTS *update){
    if(!update)
        return 1;
    int error = 0;
    int64_t index = 0,
            eventTotal = readTotalEvents(),
            data = 0;
    EVENTS *events = NULL;
    events = malloc(sizeof(EVENTS) * (eventTotal+1));
    if(!events)
        return -1;
    error = loadEventData(events);
    if(error != 0)
        goto cleanup;
    if(!searchEventId(events, eventTotal, id, &index)){
        error = 2;
        goto cleanup;
    }
//int eventId;
//  int limit; //max students
//  int status; //-1 canceled, 0 planned, 1 concluded
//  time_t date;//unix time, aka seconds since 1970
//  char location[256];
//  char eventName[256];
//  char eventDesc[256];
    if(update->limit >= 0)
        events[index].limit = update->limit;
    if(update->status != -2)
        events[index].status = update->status;
    if(update->date != 0)
        events[index].date = update->date;
    if(update->location[0] != '\0')
        strcpy(events[index].location, update->location);
    if(update->eventName[0] != '\0')
        strcpy(events[index].eventName, update->eventName);
    if(update->eventDesc[0] != '\0')
        strcpy(events[index].eventDesc, update->eventDesc);
    updateEventData(events, eventTotal);
    goto cleanup;
cleanup:
    free(events);
    return error;
}

int deleteEvent(int id){
    int64_t index = 0,
            eventTotal = readTotalEvents();
    EVENTS *check = NULL,
          *events = malloc(sizeof(EVENTS) * (eventTotal + 1)),
          event;
    if(!events)
        return -1;
    int error = loadEventData(events);
    if(error != 0)
        goto cleanup;
    if(!searchEventId(events, eventTotal, id, &index)){
        error = -1;
        goto cleanup;
    }
    // aray[x] replaced with [x+n];
    // because i dont know how to do it better kek;
    if(index < eventTotal-1)
        for(int i = index ; i < eventTotal-1 ; i++)
            events[i] = events[i+1];
    eventTotal--;
    updateEventData(events, eventTotal);
    if(TotalEventAmount != eventTotal)
        TotalEventAmount = eventTotal;
    goto cleanup;
cleanup:
    free(events);
    return error;
}

int getEvent(EVENTS *event, int id){
    int64_t eventTotal = readTotalEvents();
    int64_t index = 0;
    EVENTS *events = NULL;
    events = malloc(sizeof(EVENTS) * (eventTotal + 1));
    if(!events)
        return -1;
    int error = loadEventData(events);
    if(error != 0)
        goto cleanup;
    event = searchEventId(events, eventTotal, id, &index);
    goto cleanup;
cleanup:
    free (events);
    return error;
}

int getAllEvents(char **string, STUDENTQUEUE *queue, int eventsPerPage, int *page, char *special, int orderBy){
    int64_t eventTotal = readTotalEvents();
    EVENTS *events = malloc(sizeof(EVENTS) * (eventTotal + 1));
    int maxPages = eventTotal/eventsPerPage,
        error = 0;
    if(!events)
        return -1;
    if(eventTotal%eventsPerPage != 0)
        maxPages++;
    if (*page >= maxPages)
        *page = maxPages-1;
    if(*page < 0)
        *page = 0;
    error = loadEventData(events);
    if(error != 0)
        goto cleanup;
    switch(orderBy){
        case 1://name
            qsort(events, eventTotal, sizeof(EVENTS), compareEventsByName);
            break;
        case 2://status
            qsort(events, eventTotal, sizeof(EVENTS), compareEventsByStatus);
            break;
        case 3://date
            qsort(events, eventTotal, sizeof(EVENTS), compareEventsByDate);
            break;
        default:
            break;
    }
    error = createEventString(string, events, queue, eventTotal, eventsPerPage, *page);
    if(error != 0)
        goto cleanup;
    addPageInfo(string, *page, eventsPerPage, eventTotal, special, "Users");
    goto cleanup;
cleanup:
    free(events);
    return error;
    return 0;
}

int getAllEventIds(int **eventIds){
    int64_t eventTotal = readTotalEvents();
    int64_t index = 0;
    EVENTS *events = NULL;
    events = malloc(sizeof(EVENTS) * (eventTotal + 1));
    *eventIds = malloc(sizeof(int) * (eventTotal + 1));
    if(!events)
        return -1;
    int error = loadEventData(events);
    if(error != 0)
        goto cleanup;
    for(int64_t i = 0 ; i < eventTotal ; i++)
        (*eventIds)[i] = events[i].eventId;
    goto cleanup;
cleanup:
    free (events);
    return error;
}

