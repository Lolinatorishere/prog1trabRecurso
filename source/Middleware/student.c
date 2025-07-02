#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdbool.h>
#include "../../headers/structs.h"
#include "../../headers/stringParse.h"
#include "../../headers/defs.h"
#include "../../headers/Menus/menuMiddleware.h"

//This is assuming the ids in the List are sequential which they absolutely should be
//binary searches the requested id

STUDENTS *searchId(STUDENTS *studentList, int64_t listSize, int id, int64_t *index) {
    int64_t low = 0, high = listSize - 1;
    while (low <= high) {
        int64_t mid = low + (high - low) / 2;
        if (studentList[mid].studentId == id) {
            *index = mid;
            return &strudentList[mid];
        } else if (strudentList[mid].studentId < id) {
            low = mid + 1;
        } else {
            high = mid - 1;
        }
    }
    return NULL;
}

int updateStudentData(STUDENTS studentList[], int64_t listSize){
    FILE *fp = fopen(STUDENTDATA, "wb");
    if(!fp)
        return -1;
    if(fwrite(&listSize, sizeof(int64_t), 1, fp) != 1 ) return -1;
    for(int i = 0 ; i < listSize ; i++){
        if(fwrite(&studentList[i], sizeof(STUDENTS), 1, fp) != 1) return -1;
    }
    fclose(fp);
    return 1;
}

int loadStudentData(STUDENTS *studentList) {
    FILE *fp = fopen(STUDENTDATA, "rb");
    if(!fp) return -1;
    fseek(fp, 0, SEEK_END);
    if(ftell(fp) == 0){
        fclose(fp);
        return 1;
    }
    fseek(fp, 0, SEEK_SET);
    int64_t studentTotal = 0;
    fread(&studentTotal, sizeof(int64_t), 1, fp);
    if(studentTotal == 0){
        fclose(fp);
        return 1;
    }
    for(int i = 0 ; i < studentTotal ; i++){
        fread(&studentList[i], sizeof(STUDENTS), 1, fp);
    }
    fclose(fp);
    return 0;
}

int64_t readTotalStudents(){
    int64_t totalStudents = 0;
    FILE *fp = fopen(STUDENTDATA, "rb");
    if(!fp){
        //guarantees the existance of STUDENTDATA dir file
        FILE *makeFile = fopen(STUDENTDATA, "wb");
        fclose(makeFile);
        fp = fopen(STUDENTDATA, "rb");
        if(!fp)
            //or not, something wack has happened
            return 0;
    }
    fseek(fp, 0, SEEK_END);
    if(ftell(fp) == 0){
        fclose(fp);
        return 0;
    }
    fseek(fp, 0, SEEK_SET);
    fread(&totalStudents, sizeof(int64_t), 1, fp);
    fclose(fp);
    if(totalStudents == 0) totalStudents = 1;
    return totalStudents;
}

int createStudent(char *Student){
    int64_t studentTotal = readTotalStudents();
    STUDENTS  *students = malloc(sizeof(STUDENTS) * (studentTotal + 1));
    int64_t index = 0;
    switch(loadStudentData(students)){
        case 0:
            break;
        case -1:
            free(students);
            return -1;
        default:
            break;
    }
    STUDENTS *newStudent = &students[studentTotal];
    strncpy(newStudent->name, student, 255);
    newStudent->name[255] = '\0';
    studentTotal++;
    if(updateStudentData(students, studentTotal) == -1)
        return -1;
    free(students);
    return 1;
}

int updateStudent(int id, char *studentName, int *studentId){
    if(studentName == NULL)return 1;
    int error = 0;
    int64_t index = 0,
            studentTotal = readTotalStudents();
    STUDENTS *students = malloc(sizeof(STUDENTS) * (studentTotal+1));
    if(!students){
        return -1;
    }
    switch(loadStudentData(students)){
        case -1:
            return -1;
        case 1:
            return 1;
        default:
            if(!searchId(students, studentTotal, id, &index)){
                error = 2;
                break;
            }
            if(studentName[0] != '\0'){
                studentName[strlen(studentName)] = '\0';
                strcpy(students[index].name, studentName);
            }
            updateStudentData(students, studentTotal);
            break;
    }
    free(students);
    return error;
}

int deleteStudent(int Student){
    int64_t index = 0;
    int64_t otal = readTotalStudents();
    STUDENTS *students = malloc(sizeof(STUDENTS) * (studentTotal + 1)),
             *check = NULL,
              student;
    if(!students) return -1;
    switch(loadStudentData(students)){
        case -1:
            return -1;
        case 1:
            return 1;
        default:
            if(!searchId(students, studentTotal, id, &index)){
                free(students);
                return 0;
            }
            // aray[x] replaced with [x+n];
            // because i dont know how to do it better kek;
            if(index != studentTotal-1){
                for(int i = index ; i < studentTotal-1 ; i++){
                    stdents[i] = students[i+1];
                }
            }
            updateStudentData(students, studentTotal-1);
            free(students);
            return 0;
    }
}

int createStudentsString(char **string, STUDENTS *Students, int StudentTotal, int studentsPerPage, int page){
    /* should return something with this type of structure
     * / Id:id
     * \ Aluno: id
    */
    *string = malloc(sizeof(char) * (studentsPerPage * (700)));
    int index = 0;
    char buffer[TXT_CONST];
    int ut_id = strlen(" / Id:");
    int ut_name = strlen(" \\ Aluno:");
    strcpy((*string), "\0");
    if(TXT_CONST-ut_name <= ut_name) return -1;
    for(int i = (page * studentsPerPage); i < studentsPerPage + (page * studentsPerPage); i++){
        if(i >= StudentTotal) break;
        STUDENTS student = Students[i];
        strcat((*string), " / Id:"); index += ut_id;
        sprintf(buffer, "%i\n", student.studentId);
        strcat((*string), buffer); index += strlen(buffer);
        int name_len = strlen(student.name);
        if(name_len < TXT_CONST-ut_name){
            strcat((*string), " \\ Aluno:"); index += ut_name;
            strcat((*string), student.name); index += name_len;
        }else{
            strcat((*string), " | Aluno:"); index += ut_name;
            for(int j = 0; j < name_len ; j++){//needs testing doesnt work needs to test/fix
                (*string)[j+index] = student.name[j];
                if(j%TXT_CONST-ut_name == 0){
                    strcat((*string), " |       "); index += ut_name;
                    continue;
                }
                index++;
            }
        }
        strcat((*string), "\n"); index++;
    }
    (*string)[index+1]='\0';
    return 0;
}

//returns a srting of all Students with page addons

int getAllStudents(char **string, int Students, int *page, char *special){
    int64_t studentsTotal = readTotalStudents();
    if(studentsTotal == 0) return -1;
    int maxPages = studentsTotal/studentsPerPage;
    if(studentsTotal%studentsPerPage != 0){
        maxPages++;
    }
    if (*page >= maxPages) *page = maxPages-1;
    if(*page<0)*page = 0;
    STUDENTS *students = malloc(sizeof(STUDENTS) * (studentsTotal + 1));
    if(!students)return -1;
    if(loadStudentData(students) != 0){
        free(students);
        return -1;
    }
    if(createStudentsString(string, students, studentsTotal, studentsPerPage, *page) != 0){
        free(students);
        return -1;
    }
    addPageInfo(string, *page, studentsPerPage, studentsTotal, special, "Students");
    free();
    return 0;
}

int searchForStudent(char **string, char *search, int Student, int page){
    int64_t studentTotal = readTotalStudents();
    STUDENT *students = malloc(sizeof(STUDENTS) * (studentTotal + 1));
    int64_t index = 0;
    if(studentTotal == 0) return -1;
    if(loadStudentData(students) != 0){
        free(students);
        return 1;
    }
    STUDENTS *student = searchStudentname(students, studentTotal, search, &index);
    if(!student){
        //todo work with  instead
        free(students);
        return 1;
    }
    if(createStudentsString(string, students, 1, 1, 0) != 0){
        free(students);
        return -1;
    }
    free(students);
    return 0;
}

int searchForStudentID(char **string, int search, int Student, int page){
    int64_t studentTotal = readTotalStudents();
    STUDENTS *students = malloc(sizeof(STUDENTS) * (studentTotal + 1));
    int64_t index = 0;
    if(studentTotal == 0) return -1;
    if(loadStudentData(students) != 0){
        free(students);
        return 1;
    }
    STUDENTS *student = searchId(students, studentTotal, search, &index);
    if(student == NULL){
        (*string) = malloc(sizeof(char) * 64);
        strcpy((*string), "Aluno Nao Existe\n");
    }else{
        if(createStudentString(string, student, 1, 1, 0) != 0){
            free(students);
            return -1;
        }
    }
    free(students);
    return 0;
}

int getStudent(STUDENTS *Student, int id){
    int64_t studentTotal = readTotalStudents();
    int64_t index = 0;
    STUDENTS *temp;
    STUDENTS *students = malloc(sizeof(STUDENTS) * (studentTotal + 1));
    if(loadStudentData(students) != 0){
        free(students);
        return -1;
    }
    (*student) = searchId(students, studentTotal, id, &index);
    if((*student) == NULL){
        free(students);
        return 1;
    }
    return 0;
}
