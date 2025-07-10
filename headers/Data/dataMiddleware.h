#ifndef DATAMIDDLEWARE_H
#define DATAMIDDLEWARE_H
#include "../structs.h"

int compareEventsByName(const void *a, const void *b);

int compareEventsByStatus(const void *a, const void *b);

int compareEventsByDate(const void *a, const void *b);

int compareStudentQueuesByTotal(const void *a, const void *b);

STUDENTQUEUE *getEventQueue(STUDENTQUEUE *queues, int eventId);

int loadEventStudents(STUDENTQUEUE **queues);

time_t convertToTimestamp(int day, int month, int year);

void convertFromTimestamp(time_t timestamp, int* day, int* month, int* year);

time_t notificationTimestamp(time_t timestamp);

#endif
