

int compareEventsByName(const void *a, const void *b){
    const EVENTS *eventA = (const EVENTS *)a;
    const EVENTS *eventB = (const EVENTS *)b;
    return strcmp(eventA->eventName, eventB->eventName);
}

int compareEventsByStatus(const void *a, const void *b){
    const EVENTS *eventA = (const EVENTS *)a;
    const EVENTS *eventB = (const EVENTS *)b;
    return eventA->status - eventB->status;
}

int compareEventsByDate(const void *a, const void *b){
    const EVENTS *eventA = (const EVENTS *)a;
    const EVENTS *eventB = (const EVENTS *)b;
    if (eventA->date < eventB->date)
        return -1;
    else if (eventA->date > eventB->date)
        return 1;
    else
        return 0;
}

int compareStudentQueuesByTotal(const void *a, const void *b) {
    const STUDENTQUEUE *queueA = (const STUDENTQUEUE *)a;
    const STUDENTQUEUE *queueB = (const STUDENTQUEUE *)b;
    return queueA->total - queueB->total;
}

int compareEventsById(const void *a, const void *b){
    const EVENTS *eventA = (const EVENTS *)a;
    const EVENTS *eventB = (const EVENTS *)b;
    if(eventA->eventId < eventB->eventId)
        return -1;
    else if(eventA->eventId > eventB->eventId)
        return 1;
    else
        return 0;
}
