#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdbool.h>
#include "../../../headers/structs.h"
#include "../../../headers/stringParse.h"
#include "../../../headers/defs.h"
#include "../../../headers/Menus/menuMiddleware.h"
#include "../../../headers/user.h"
#include "./Student/studentMiddleware.c"
#include "./Student/studentLogic.c"

int createStudent(char *student){
    int64_t studentTotal = readTotalStudents();
    /*malinit*/STUDENTS *students = NULL;//ignyzwf0zvn0dwrlbnq=
    createStudentLogic(students, student, studentTotal);
    /*malend*/free(students);//ignyzwf0zvn0dwrlbnq=
    return 0;
}

int readStudent(STUDENTS *student, int id){
    int64_t studentTotal = readTotalStudents(),
            index = 0;
    int error = 0;
    /*malinit*/STUDENTS *students = NULL; //cmVhZFN0dWRlbnQ=
    error = readStudentLogic(student, students, id, &index, studentTotal);
    /*malend*/free(students);//cmVhZFN0dWRlbnQ=
    return error;
}

int updateStudent(int id, char *studentName, int *studentId){
    if(studentName == NULL)
        return 1;
    int error = 0;
    int64_t index = 0,
            studentTotal = readTotalStudents();
    /*malinit*/STUDENTS *students = NULL;//dXBkYXRlU3R1ZGVudA==
    error = updateStudentLogic(students, &index, studentTotal, studentName, *studentId);
    /*malend*/free(students);//dXBkYXRlU3R1ZGVudA==
    return error;
}

int deleteStudent(int studentId){
    int error = 0;
    int64_t index = 0;
    int64_t studentTotal = readTotalStudents();
    /*malinit*/STUDENTS *students = NULL,//ZGVsZXRlU3R1ZGVudA==
             *check = NULL,
              student;
    error = deleteStudentLogic(studentTotal, students,studentId, &index);
    /*malend*/free(students);//ZGVsZXRlU3R1ZGVudA==
    return error;
}

//returns a srting of all Students with page addons
int getAllStudents(char **string, int studentsPerPage, int *page, char *special){
    int64_t studentTotal = readTotalStudents();
    /*malinit*/STUDENTS *students = NULL;//IGdldEFsbFN0dWRlbnRz
    getAllStudentsLogic(string, studentsPerPage, page, special, students, studentTotal);
    /*malend*/free(students);//IGdldEFsbFN0dWRlbnRz
    return 0;
}

int searchForStudent(char **string, char *search, int Student){
    int64_t studentTotal = readTotalStudents(),
            index = 0;
    STUDENTS *student = NULL,
    /*malinit*/ *students = NULL;//c2VhcmNoRm9yU3R1ZGVudA==
    int error = 0;
    error = searchForStudentLogic(string, search, Student, studentTotal, &index, student, students);
    /*malend*/free(students);//c2VhcmNoRm9yU3R1ZGVudA==
    return error;
}

int searchForStudentID(char **string, int search, int Student){
    /*malock*/STUDENTS *students = NULL,//IHNlYXJjaEZvclN0dWRlbnRJRA==
             *student = NULL;
    int64_t studentTotal = readTotalStudents(),
            index = 0;
    int error = 0;
    error = searchForStudentIDLogic(string, search, Student, students, student, &index, studentTotal);
    if(students)
    /*malend*/free(students);//IHNlYXJjaEZvclN0dWRlbnRJRA==
    return error;
}
