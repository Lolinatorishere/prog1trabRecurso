#ifndef STUDENT_H
#define STUDENT_H
#include "./structs.h"

int64_t readTotalStudents();

int createStudent(char *Student);

int readStudent(STUDENTS *Student, int id);

int updateStudent(int id, char *studentName, int *studentId);

int deleteStudent(int Student);

int getAllStudents(char **string, int Students, int *page, char *special);

int searchForStudent(char **string, char *search, int Student, int page);

int searchForStudentID(char **string, int search, int Student);


#endif
