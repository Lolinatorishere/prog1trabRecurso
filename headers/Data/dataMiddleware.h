#ifndef DATAMIDDLEWARE_H
#define DATAMIDDLEWARE_H
#include "../structs.h"

int associateStudentUser(int userId, int alunoId);

int DissasociateStudentUser(int userId, int alunoId);

int loadEventStudentData(EVENTS *eventList);

#endif
