#ifndef STRUCTS_H
#define STRUCTS_H
#include <time.h>
#include <stdint.h>
#include <stdbool.h>

typedef struct users{
    int userId;
    int type;
    int studentId;
    char userName[256];
    char password[256];
}USERS;

typedef struct students{
    int studentId;
    int userId;
    char studentName[256];
}STUDENTS;

typedef struct studentlist{
    int studentId;
    bool participou;
    struct studentlist *next;
    struct studentlist *prev;
}STUDENTLIST;

typedef struct studentQueue{
    int eventId;
    int total;
    STUDENTLIST *head;
    STUDENTLIST *tail;
}STUDENTQUEUE;

//ID ´unico, nome do evento, data, local, n´umero m´aximo de participantes, descricao e estado (ativo, cancelado, concluido).
typedef struct events{
    int eventId;
    int limit; //max students
    int status; //-1 canceled, 0 planned, 1 concluded
    time_t date;//unix time, aka seconds since 1970
    char location[256];
    char eventName[256];
    char eventDesc[256];
}EVENTS;

typedef struct notif{
    int eventId;
    char msg[256];
}NOTIF;

#endif
