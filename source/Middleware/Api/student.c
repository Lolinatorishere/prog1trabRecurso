#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdbool.h>
#include "../../../headers/structs.h"
#include "../../../headers/stringParse.h"
#include "../../../headers/defs.h"
#include "../../../headers/Menus/menuMiddleware.h"
#include "../../../headers/student.h"

int64_t TotalStudentAmount = 0;

STUDENTS setStudent(){
    STUDENTS student = {'\0', -1, -1};
    return student;
}

void copyStudent(STUDENTS *dst, STUDENTS src){
    //doing it like this because of gdb
    //if not done instead of appearing "string" <000> repeats n times
    //it will have "string"<000> random non wiped memory
    for(int i = 0 ; i < strlen(src.studentName) ; i++){
        dst[0].studentName[i] = src.studentName[i];
    }
    for(int i = strlen(src.studentName) ; i < 256 ; i++){
        dst[0].studentName[i] = '\0';
    }
    dst[0].studentId = src.studentId;
    dst[0].userId = src.userId;
}

STUDENTS *searchStudentname(STUDENTS *studentList, int64_t listSize, char *studentname, int64_t *index) { // very slow
    if(listSize == 0)
        return NULL;
    int ulen = strlen(studentname);
    for(int64_t i = 0 ; i < listSize ; i++){
        int check = 0;
        if(ulen != strlen(studentList[i].studentName))continue;
        for(int j = 0 ; j < ulen ; j++){
            if(studentList[i].studentName[j] != studentname[j])break;
            check++;
        }
        if(check != ulen) continue;
        return &studentList[i];
    }
    return NULL;
}

//This is assuming the ids in the List are sequential which they absolutely should be
//binary searches the requested id
STUDENTS *searchStudentId(STUDENTS *studentList, int64_t listSize, int id, int64_t *index) {
    if(listSize == 0)
        return NULL;
    int64_t low = 0, high = listSize - 1;
    while (low <= high) {
        int64_t mid = low + (high - low) / 2;
        if (studentList[mid].studentId == id) {
            *index = mid;
            return &studentList[mid];
        } else if (studentList[mid].studentId < id) {
            low = mid + 1;
        } else {
            high = mid - 1;
        }
    }
    return NULL; 
}

STUDENTS *searchStudentUserId(STUDENTS *studentList, int64_t listSize, int *userId, int64_t *index) { // very slow
    if(listSize == 0)
        return NULL;
    for(int64_t i = 0 ; i < listSize ; i++){
        if(studentList[i].userId != *userId)
            continue;
        return &studentList[i];
    }
    return NULL;
}

int updateStudentData(STUDENTS *studentList, int64_t listSize){
    FILE *fp = fopen(USERDATA, "wb");
    if(!fp)
        return -1;
    if(fwrite(&listSize, sizeof(int64_t), 1, fp) != 1 ) 
        return -1;
    for(int i = 0 ; i < listSize ; i++){
        if(fwrite(&studentList[i], sizeof(STUDENTS), 1, fp) != 1) 
            return -1;
    }
    fclose(fp);
    fp = NULL;
    return 1;
}

int64_t readTotalStudents(){
    int64_t totalStudents = 0;
    FILE *fp = fopen(USERDATA, "rb");
    if(TotalStudentAmount == 0){
        if(!fp)
            return 0;
        if(fseek(fp, 0, SEEK_END) != 0)
            goto cleanup;
        if(ftell(fp) == 0)
            goto cleanup;
        if(fseek(fp, 0, SEEK_SET) != 0)
            goto cleanup;
        fread(&totalStudents, sizeof(int64_t), 1, fp);
        TotalStudentAmount = totalStudents;
        goto cleanup;
    }
    totalStudents = TotalStudentAmount;
    goto cleanup;
cleanup:
    if(fp)
        fclose(fp);
    fp = NULL;
    return totalStudents;
}

int loadStudentData(STUDENTS *studentList) {
    FILE *fp = fopen(USERDATA, "rb");
    int error = 0;
    if(!fp)
        return -1;
    fseek(fp, 0, SEEK_END);
    if(ftell(fp) == 0){
        error = 1;
        goto cleanup;
    }
    fseek(fp, 0, SEEK_SET);
    int64_t studentTotal = 0;
    fread(&studentTotal, sizeof(int64_t), 1, fp);
    if(studentTotal == 0){
        error = 1;
        goto cleanup;
    }
    for(int i = 0 ; i < studentTotal ; i++)
        fread(&studentList[i], sizeof(STUDENTS), 1, fp);
    goto cleanup;
cleanup:
    fclose(fp);
    fp = NULL;
    return 0;
}

int createStudent(char *studentname){
    int64_t studentTotal = readTotalStudents(),
            index = 0;
    STUDENTS *students = NULL;
    students = malloc(sizeof(STUDENTS) * (studentTotal + 1));
    if(!students)
        return -1;
    int error = loadStudentData(students);
    if(error == -1)
        goto cleanup;
    if(searchStudentname(students, studentTotal, studentname, &index)){
        error = 1;
        goto cleanup;
    }
    STUDENTS *newStudent = &students[studentTotal];
    strncpy(newStudent->studentName, studentname, 255);
    newStudent->studentName[255] = '\0';
    if (studentTotal == 0) newStudent->studentId = 1;
    else newStudent->studentId = students[studentTotal - 1].studentId + 1;
    newStudent->userId = -1;
    studentTotal++;
    if(updateStudentData(students, studentTotal) == -1){
        error = -1;
        goto cleanup;
    }
    if(TotalStudentAmount != studentTotal)
        TotalStudentAmount = studentTotal;
    goto cleanup;
cleanup:
    free(students);
    return error;
}

int updateStudent(int id, char *studentName, int userId){
    if(studentName[0] == '\0' && userId == -1);
        return 1;
    int error = 0;
    int64_t index = 0,
            studentTotal = readTotalStudents(),
            data = 0;
    STUDENTS *students = NULL;
    students = malloc(sizeof(STUDENTS) * (studentTotal+1));
    if(!students)
        return -1;
    error = loadStudentData(students);
    if(error != 0)
        goto cleanup;
    if(!searchStudentId(students, studentTotal, id, &index)){
        error = 2;
        goto cleanup;
    }
    if(studentName[0] != '\0'){
        studentName[strlen(studentName)] = '\0';
        strcpy(students[index].studentName, studentName);
    }
    if(userId != -1)
        students[index].userId = userId;
    updateStudentData(students, studentTotal);
    goto cleanup;
cleanup:
    free(students);
    return error;
}

int deleteStudent(int id){
    int64_t index = 0,
            studentTotal = readTotalStudents();
    STUDENTS *check = NULL,
          *students = malloc(sizeof(STUDENTS) * (studentTotal + 1)),
          student;
    if(!students)
        return -1;
    int error = loadStudentData(students);
    if(error != 0)
        goto cleanup;
    if(!searchStudentId(students, studentTotal, id, &index)){
        error = -1;
        goto cleanup;
    }
    // aray[x] replaced with [x+n];
    // because i dont know how to do it better kek;
    if(index < studentTotal-1)
        for(int i = index ; i < studentTotal-1 ; i++)
            students[i] = students[i+1];
    studentTotal--;
    updateStudentData(students, studentTotal);
    if(TotalStudentAmount != studentTotal)
        TotalStudentAmount = studentTotal;
    goto cleanup;
cleanup:
    free(students);
    return error;
}

int getStudent(STUDENTS *student, int id){
    int64_t studentTotal = readTotalStudents();
    int64_t index = 0;
    STUDENTS *temp = NULL,
          *students = NULL;
    students = malloc(sizeof(STUDENTS) * (studentTotal + 1));
    if(!students)
        return -1;
    int error = loadStudentData(students);
    if(error != 0)
        goto cleanup;
    temp = searchStudentId(students, studentTotal, id, &index);
    if(!temp){
        error = 1;
        goto cleanup;
    }
    copyStudent(student, (*temp));
    goto cleanup;
cleanup:
    free (students);
    return error;
}

//int getStudents(STUDENTS *hits, int *ids, int studentAmount){
//    int64_t studentTotal = readTotalStudents(),
//            index = 0;
//    STUDENTS *students = malloc(sizeof(STUDENTS) * (studentTotal + 1));
//    if(!students)
//        return -1;
//    int error = loadStudentData(students);
//    if(error != 0)
//        goto cleanup;
//    for(int i = 0 ; i < studentAmount ; i++){
//        STUDENTS *student = searchStudentId(students, studentTotal, ids[i], &index);
//        copyStudent(&hits[i], *student);
//        if(&hits[i] == NULL)
//            hits[i] = setStudent();
//    }
//    goto cleanup;
//cleanup:
//    free(students);
//    return error;
//}
