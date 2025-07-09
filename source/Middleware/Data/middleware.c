#include <stdio.h>
#include <stdlib.h>
#include "../../../headers/defs.h"
#include "../../../headers/structs.h"
#include "../../../headers/user.h"
#include "../../../headers/student.h"
#include "../../../headers/event.h"

//events
//#include "./event.c"

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

int loadEventStudentData(EVENTS *eventLoad){
    return 0;
}
