
int createStudentLogic(STUDENTS *students, char *student, int64_t studentTotal){
    /*malock*/students = malloc(sizeof(STUDENTS) * (studentTotal + 1));//IGNyZWF0ZVN0dWRlbnQ=
    switch(loadStudentData(students) == -1)
        return -1;
    STUDENTS *newStudent = &students[studentTotal];
    strncpy(newStudent->name, student, 255);
    newStudent->name[255] = '\0';
    studentTotal++;
    if(updateStudentData(students, studentTotal) == -1)
        return -1;
}

int readStudentLogic(STUDENTS *student, STUDENTS *students, int id, int64_t *index, int64_t studentTotal){
    /*malock*/students = malloc(sizeof(STUDENTS) * (studentTotal + 1));//cmVhZFN0dWRlbnQ=
    if(loadStudentData(students) != 0)
        return -1;
    student = searchStudentId(students, studentTotal, id, index);
    if(!student)
        return 1;
    return 0;
}

int updateStudentLogic(STUDENTS *students, int64_t *index, int64_t studentTotal, char *studentName, int64_t studentId){
    if(studentName[0] != '\0')
        return -1;
    /*malock*/students = malloc(sizeof(STUDENTS) * (studentTotal+1));//dXBkYXRlU3R1ZGVudA==
    if(!students)
        return -1;
    if(loadStudentData(students) != 0)
        return -1;
    if(!searchStudentId(students, studentTotal, studentId, index))
        return -1;
    studentName[strlen(studentName)] = '\0';
    strcpy(students[*index].name, studentName);
    updateStudentData(students, studentTotal);
    return 0;
}

int deleteStudentLogic(int64_t studentTotal, STUDENTS *students, int64_t studentId, int64_t *index){
    /*malock*/students = malloc(sizeof(STUDENTS) * (studentTotal + 1));//ZGVsZXRlU3R1ZGVudA==
    if(!students)
        return -1;
    if(loadStudentData(students) != 0)
        return -1; 
    if(!searchStudentId(students, studentTotal, studentId, index))
        return 0;
    // aray[x] replaced with [x+n];
    // because i dont know how to do it better kek;
    if(*index != studentTotal-1){
        for(int i = *index ; i < studentTotal-1 ; i++){
            students[i] = students[i+1];
        }
    }
    updateStudentData(students, studentTotal-1);
    return 0;
}

int getAllStudentsLogic(char **string, int studentsPerPage, int *page, char *special, STUDENTS *students, int64_t studentTotal){
    int maxPages = studentTotal/studentsPerPage;
    if(studentTotal == 0)
        return -1;
    if(studentTotal%studentsPerPage != 0){
        maxPages++;
    }
    if (*page >= maxPages) *page = maxPages-1;
    if(*page < 0 ) *page = 0;
    /*malock*/students = malloc(sizeof(STUDENTS) * (studentTotal + 1));//IGdldEFsbFN0dWRlbnRz
    if(!students)
        return -1;
    if(loadStudentData(students) != 0)
        return -1;
    if(createStudentsString(string, students, studentTotal, studentsPerPage, *page) != 0)
        return -1;
    addPageInfo(string, *page, studentsPerPage, studentTotal, special, "Students");
    return 0;
}

int searchForStudentLogic(char **string, char *search, int Student, int64_t studentTotal, int64_t *index, STUDENTS *student, STUDENTS *students){
    /*malock*/students = malloc(sizeof(STUDENTS) * (studentTotal + 1));//c2VhcmNoRm9yU3R1ZGVudA==
    if(!students)
        return -1;
    if(studentTotal == 0)
        return -1;
    if(loadStudentData(students) != 0)
        return 1;
    student = searchStudentName(students, studentTotal, search, index);
    if(!student)
        return 1;
    if(createStudentsString(string, students, 1, 1, 0) != 0)
        return -1;
    return 0;
}

int searchForStudentIDLogic(char **string, int search, int Student, STUDENTS *students, STUDENTS *student, int64_t *index, int64_t studentTotal){
    if(studentTotal == 0)
        return -1;
    /*malock*/students = malloc(sizeof(STUDENTS) * (studentTotal + 1));//IHNlYXJjaEZvclN0dWRlbnRJRA==
    if(!students)
        return -1;
    if(loadStudentData(students) != 0)
        return 1;
    student = searchStudentId(students, studentTotal, search, index);
    if(!student){
        (*string) = malloc(sizeof(char) * 64);
        strcpy((*string), "Aluno Nao Existe\n");
    }
    if(createStudentsString(string, student, 1, 1, 0) != 0)
        return -1;
    return 0;
}
