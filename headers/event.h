#ifndef EVENT_H
#define EVENT_H
#include "./structs.h"

EVENTS setEvent();

int64_t readTotalEvents();

int createEvent(EVENTS eventCreate);

int updateEvent(int id, EVENTS *update);

int deleteEvent(int eventId);

int getEvent(EVENTS *event, int id);

int getAllEventIds(int **eventIds);

int getAllEvents(char **string, STUDENTQUEUE *queue, int eventsPerPage, int *page, char *special, int orderBy);

int getNonPlanedEvents(char **string, STUDENTQUEUE *queue, int eventsPerPage, int *page, char *special, int flip);

int searchForEventId(char **string, STUDENTQUEUE *queues, int searchId);

#endif
