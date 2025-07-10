#ifndef EVENT_H
#define EVENT_H
#include "./structs.h"

EVENTS setEvent();

int64_t readTotalEvents();

int createEvent(EVENTS eventCreate);

int updateEvent(int eventId, char *eventName, time_t *date, char *location, int *limit, int *status);

int deleteEvent(int eventId);

int getEvent(EVENTS *event, int id);

int getAllEventIds(int **eventIds);

int getAllEvents(char **string, STUDENTQUEUE *queue, int eventsPerPage, int *page, char *special, int orderBy);

#endif
