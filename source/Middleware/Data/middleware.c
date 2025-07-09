#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdbool.h>
#include "../../../headers/defs.h"
#include "../../../headers/structs.h"
#include "../../../headers/user.h"
#include "../../../headers/student.h"
#include "../../../headers/event.h"
//events
#include "./studentll.c"

int associateStudentUser(int userId, int studentId){
    STUDENTS *student;
    USERS *user;
    getStudent(student, studentId);
    if(student->userId != -1)
        return -1;
    getUser(user, studentId);
    if(user->studentId != -1)
        return -1;
    updateUser(userId, "\0", "\0", -1, studentId);
    updateStudent(studentId, "\0", userId);
    return 0;
}

int DissasociateStudentUser(int userId, int studentId){
    STUDENTS *student;
    USERS *user;
    getStudent(student, studentId);
    if(student->userId == -1)
        return -1;
    getUser(user, studentId);
    if(user->studentId == -1)
        return -1;
    updateUser(userId, "\0", "\0", -1, studentId);
    updateStudent(studentId, "\0", userId);
    return 0;
}

int doesEventExist(STUDENTQUEUE *queues, int eventId) {
    int64_t totalQueues = readTotalEvents();
    if(!queues || totalQueues <= 0)
        return 0;
    for(int64_t i = 0; i < totalQueues; i++) 
        if(queues[i].eventId == eventId)
            return 1;
    return 0;
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

int loadEventStudents(STUDENTQUEUE *eventStudents, int *eventIds, STUDENTQUEUE **queues){
    int64_t totalEventStudents = 0,
            totalEvents = readTotalEvents();
    char dir[512] = {'\0'},
         dirId[64] = {'\0'};
    int *studentIds = NULL;
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
    studentIds = NULL;
    if(fp)
        fclose(fp);
    fp = NULL;
    return 0;
}
