#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdbool.h>
#include "../../../headers/defs.h"
#include "../../../headers/structs.h"
#include "../../../headers/user.h"
#include "../../../headers/event.h"
//events
#include "./studentll.c"
#include "./sorts.c"
#include "./exports.c"

STUDENTQUEUE *getEventQueue(STUDENTQUEUE *queues, int eventId){
    int64_t totalQueues = readTotalEvents();
    if(!queues || totalQueues <= 0)
        return 0;
    for(int64_t i = 0; i < totalQueues; i++) 
        if(queues[i].eventId == eventId)
            return &queues[i];
    return NULL;
}

STUDENTQUEUE *createAllQueues(){
    int64_t totalEvents = readTotalEvents();
    if(totalEvents <= 0)
        return NULL;
    STUDENTQUEUE *queues = calloc(sizeof(STUDENTQUEUE), totalEvents+1);
    if(!queues)
        return NULL;
    for(int64_t i = 0 ; i < totalEvents ; i++) {
        queues[i].eventId = -1;
        queues[i].total = -1;
        queues[i].head = NULL;
        queues[i].tail = NULL;
    }
    return queues;
}

void refreshAllQueues(STUDENTQUEUE *queues) {
    int64_t totalEvents = readTotalEvents();
    if (!queues || totalEvents <= 0)
        return;
    STUDENTQUEUE *temp = (STUDENTQUEUE*) realloc(queues, sizeof(STUDENTQUEUE) * (totalEvents + 1));
    if(!temp)
        return;
    for (int64_t i = 0 ; i < totalEvents ; i++) {
        temp[i].eventId = -1;
        temp[i].total = -1;
        // Free existing linked list if any
        STUDENTLIST *current = temp[i].head;
        freeList(current);
        temp[i].head = NULL;
        temp[i].tail = NULL;
    }
    queues = temp;
}

int loadEventStudents(STUDENTQUEUE *queues){
    if (!queues)
        return -1;
    LISTHELPER *studentIds = NULL;
    int64_t totalEventStudents = 0;
    int64_t totalEvents = readTotalEvents();
    char dir[512] = {'\0'};
    char dirId[64] = {'\0'};
    int *eventIds = NULL;
    getAllEventIds(&eventIds);
    FILE *fp = NULL;
    for (int64_t i = 0; i < totalEvents; i++) {
        queues[i].eventId = eventIds[i];
        STUDENTLIST *cur = queues[i].head;
        while(cur){
            STUDENTLIST *next = cur->next;
            free(cur);
            cur = next;
        }
        queues[i].head = NULL;
        queues[i].tail = NULL;
        if(studentIds){
            free(studentIds);
            studentIds = NULL;
        }
        STUDENTLIST *head = NULL;
        snprintf(dir, sizeof(dir), "%s/%d", EVENTSUBDIR, eventIds[i]);
        fp = fopen(dir, "rb");
        if(!fp){
            queues[i].total = 0;
            continue;
        }
        fread(&totalEventStudents, sizeof(int64_t), 1, fp);
        queues[i].total = totalEventStudents;
        studentIds = (LISTHELPER *)malloc(sizeof(LISTHELPER) * (totalEventStudents + 1));
        if(!studentIds){
            fclose(fp);
            continue;
        }
        fread(studentIds, sizeof(int), totalEventStudents, fp);
        for(int64_t j = 0; j < totalEventStudents; j++){
            insertEnd(&head, studentIds[j].studentId, studentIds[j].participou);
            if (!head)
                continue;
            if (j == 0) {
                queues[i].head = head;
                queues[i].tail = head;
            }
            queues[i].tail = head;
        }
        fclose(fp);
        fp = NULL;
    }
    if(studentIds)
        free(studentIds);
    if(eventIds)
        free(eventIds);
    return 0;
}

int saveAllStudents(STUDENTQUEUE *queues){
    char dir[512] = {'\0'};
    int64_t eventCount = readTotalEvents();
    EVENTS event = setEvent();
    STUDENTQUEUE *queue = {0};
    FILE *fp = NULL;
    for(int i = 0 ; i < eventCount ; i++){
        queue = &queues[i];
        getEvent(&event, queue->eventId);
        if(queue->total <= 0)
            continue;
        int64_t total = queue->total;
        snprintf(dir, sizeof(dir), "%s/%d", EVENTSUBDIR, event.eventId);
        fp = fopen(dir, "wb");
        if (!fp)
            continue;
        if(!queue)
            continue;
        fwrite(&total, sizeof(int64_t), 1, fp);
        STUDENTLIST *curr = queue->head;
        for (int i = 0 ; i < total && curr != NULL ; i++) {
            fwrite(&curr->studentId, sizeof(int), 1, fp);
            fwrite(&curr->participou, sizeof(int), 1, fp);
            curr = curr->next;
        }
        fclose(fp);
        fp = NULL;
    }
    return 0;
}

time_t convertToTimestamp(int day, int month, int year){
    struct tm timestruct = {0};
    timestruct.tm_mday = day;
    timestruct.tm_mon = month - 1;
    timestruct.tm_year = year - 1900;
    return mktime(&timestruct);
}

void convertFromTimestamp(time_t timestamp, int* day, int* month, int* year) {
    struct tm *timestruct = localtime(&timestamp);
    *day = timestruct->tm_mday;
    *month = timestruct->tm_mon + 1;  // tm_mon is 0-based
    *year = timestruct->tm_year + 1900;  // tm_year is years since 1900
}

time_t notificationTimestamp(time_t timestamp){
    int day = 0,
        month = 0,
        year = 0;
    convertFromTimestamp(timestamp, &day, &month, &year);
    day += 7;
    return convertToTimestamp(day, month, year);
}
