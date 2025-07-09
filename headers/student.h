#ifndef STUDENT_H
#define STUDENT_H
#include "./structs.h"

int64_t readTotalStudents();

int createStudent(char *studentName);

int getStudent(STUDENTS *student, int id);

//int getStudents(STUDENTS *student, int id);

int updateStudent(int id, char *studentName, int userId);

int deleteStudent(int studentId);

#endif
