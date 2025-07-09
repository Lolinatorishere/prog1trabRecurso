#ifndef EVENT_H
#define EVENT_H
#include "./structs.h"

int64_t readTotalEvents();

int createEvent(char *eventName, time_t date, char *location, int limit);

int updateEvent(int eventId, char *eventName, time_t *date, char *location, int *limit, int *status);

int deleteEvent(int eventId);

int getEvent(EVENTS *event, int id);

#endif
