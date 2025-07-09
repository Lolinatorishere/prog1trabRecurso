#ifndef EVENT_H
#define EVENT_H
#include "./structs.h"

//EVENTS *searchEventId(EVENTS *eventList, int64_t listSize, int id, int64_t *index);

//EVENTINDEXHELPER *searchEventStudentIndexId(EVENTINDEXHELPER *index, int64_t indexSize, int id, int64_t *indexPos);

//EVENTS *searchEventName(EVENTS *eventList, int64_t listSize, char *eventName, int64_t *index);

//int updateEventData(EVENTS eventList[], int64_t listSize);

//int insertSortedStudentIndex(EVENTINDEXHELPER *index, EVENTINDEXHELPER new, int sizeventId);

//int updateEventStudentIndex(int *eventStudents, int totalStudents, int eventId, int new);

//int loadStudentsFromEventIndex(EVENTS *event);

//int loadEventData(EVENTS *eventList) ;

//int64_t readTotalEvents();

int createEvent(char *eventName, time_t date, char *location, int limit);

int updateEvent(int eventId, char *eventName, time_t *date, char *location, int *limit, int *status);

int deleteEvent(int eventId);

int getEvent(EVENTS *event, int id);

#endif
