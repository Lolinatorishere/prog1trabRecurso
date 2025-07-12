#ifndef DATAMIDDLEWARE_H
#define DATAMIDDLEWARE_H
#include "../structs.h"

bool deleteStudentNode(STUDENTLIST **head, int studentId);

int compareEventsByName(const void *a, const void *b);

int compareEventsByStatus(const void *a, const void *b);

int compareEventsByDate(const void *a, const void *b);

int compareStudentQueuesByTotal(const void *a, const void *b);

int loadEventStudents(STUDENTQUEUE *queues);

int saveEventStudents(STUDENTQUEUE *queue, int eventId);

STUDENTQUEUE *createAllQueues();

STUDENTQUEUE *getEventQueue(STUDENTQUEUE *queues, int eventId);

STUDENTLIST *searchStudentNode(STUDENTLIST *head, int studentId);

STUDENTLIST *getLast(STUDENTLIST *head);

void refreshAllQueues(STUDENTQUEUE *queues);

void insertEnd(STUDENTLIST **head, int studentId, bool participou);

void freeList(STUDENTLIST *head);

void convertFromTimestamp(time_t timestamp, int* day, int* month, int* year);

void exportParticipationReport(int eventId, STUDENTQUEUE queue, const char *filename);

void exportEventSubscriptions(int eventId, STUDENTQUEUE queue, const char *filename);

time_t notificationTimestamp(time_t timestamp);

time_t convertToTimestamp(int day, int month, int year);

#endif
