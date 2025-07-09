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
    STUDENTQUEUE* queues = malloc(totalEvents * sizeof(STUDENTQUEUE));
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


int loadEventStudents(STUDENTQUEUE **queues){
    int64_t totalEventStudents = 0,
            totalEvents = readTotalEvents();
    char dir[512] = {'\0'},
         dirId[64] = {'\0'};
    int *studentIds = NULL,
        *eventIds = NULL;
    (*queues) = malloc(sizeof(STUDENTQUEUE)* (totalEvents + 1));
    getAllEventIds(&eventIds);
    FILE *fp = NULL;
    for(int64_t i = 0 ; i < totalEvents ; i++){
        (*queues)[i].eventId = eventIds[i];
        if(studentIds){
            free(studentIds);
            studentIds = NULL;
        }
        STUDENTLIST *head = NULL;
        strcpy(dir, EVENTSUBDIR);
        strcat(dir, "/");
        sprintf(dirId, "%i", eventIds[i]);
        strcat(dir, dirId);
        fp = fopen(dir,"rb");
        if(!fp)
            continue;
        fread(&totalEventStudents, sizeof(int64_t), 1, fp);
        if(totalEventStudents == 0)
            continue;
        (*queues)[i].total = totalEventStudents;
        studentIds = malloc(sizeof(int) * (totalEventStudents + 1));
        if(!studentIds)
            continue;
        fread(studentIds, sizeof(int), totalEventStudents, fp);
        for(int64_t j = 0 ; j < totalEventStudents ; j++){
            insertEnd(&head, studentIds[j], false);
            if(!head)
                continue;
            if(j == 0)
                (*queues)[i].head = head;
                (*queues)[i].tail = head;
            (*queues)[i].tail = head;
        }
    }
    if(studentIds)
        free(studentIds);
    if(eventIds)
        free(eventIds);
    studentIds = NULL;
    if(fp)
        fclose(fp);
    fp = NULL;
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
    struct tm* timestruct = localtime(&timestamp);
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
