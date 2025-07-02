#include"../../../headers/structs.h"
#include"../../../headers/user.h"
#include"../../../headers/students.h"

int associateStudentUser(int userId, int alunoId){
    STUDENTS *student;
    STUDENTS *user;
    getStudent(student, alunoId);
    if(student->userId != -1) return -1;
    getUser(student, alunoId);
    if(user->alunoId != -1) return -1;
    updateUser(userId, "\0", "\0", NULL, alunoId);
    updateStudent(alunoId, "\0", userId);
    return 0;
}

int DissasociateStudentUser(int userId, int alunoId){
    STUDENTS *student;
    STUDENTS *user;
    getStudent(student, alunoId);
    if(student->userId == -1) return -1;
    getUser(student, alunoId);
    if(user->alunoId == -1) return -1;
    updateUser(userId, "\0", "\0", NULL, -1);
    updateStudent(alunoId, "\0", -1);
    return 0;
}
