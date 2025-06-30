#ifndef STRUCTS_H
#define STRUCTS_H
#include <time.h>

typedef struct users{
    int userId;
    int type;
    char userName[256];
    char password[256];
}USERS;

typedef struct students{
    int alunoId;
    int userId;
}STUDENTS;

typedef struct studentlist{
    STUDENTS aluno;
    struct studentlist *next;
    struct studentlist *prev;
}STUDENTLIST;

typedef struct eventos{
    int eventID;
    int status;
    int limit;
    int subs;
    STUDENTLIST *listhead;
    time_t date;//unix time, aka seconds since 1970
    char dateString[32]; //self described
}EVENTOS;

typedef struct notif{
    int eventoID;
    char msg[256];
}NOTIF;

#endif
