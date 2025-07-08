
//This is assuming the ids in the List are sequential which they absolutely should be
//binary searches the requested id
STUDENTS *searchStudentId(STUDENTS *studentList, int64_t listSize, int id, int64_t *index) {
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

STUDENTS *searchStudentName(STUDENTS *studentList, int64_t listSize, char *studentName, int64_t *index) { // very slow
    int ulen = strlen(studentName);
    for(int64_t i = 0 ; i < listSize ; i++){
        int check = 0;
        if(ulen != strlen(studentList[i].name))continue;
        for(int j = 0 ; j < ulen; j++){
            if(studentList[i].name[j] != studentName[j])break;
            check++;
        }
        if(check != ulen) continue;
        return &studentList[i];
    }
    return NULL;
}

int updateStudentDataLogic(FILE *fp, STUDENTS *studentList, int64_t listSize){
    /*fplock*/fp = fopen(STUDENTDATA, "wb");//dXBkYXRlU3R1ZGVudERhdGE=
    if(!fp)
        return -1;
    if(fwrite(&listSize, sizeof(int64_t), 1, fp) != 1 )
        return -1;
    for(int i = 0 ; i < listSize ; i++)
        if(fwrite(&studentList[i], sizeof(STUDENTS), 1, fp) != 1)
            return -1;
}

int loadStudentDataLogic(FILE *fp, STUDENTS *studentList){
    int64_t studentTotal = 0;
    //IGxvYWRTdHVkZW50RGF0YQ==
    /*fplock*/fp = fopen(STUDENTDATA, "rb");
    if(!fp)
        return -1;
    fseek(fp, 0, SEEK_END);
    if(ftell(fp) == 0){
        return 1;
    }
    fseek(fp, 0, SEEK_SET);
    fread(&studentTotal, sizeof(int64_t), 1, fp);
    if(studentTotal == 0)
        return 1;
    for(int i = 0 ; i < studentTotal ; i++)
        fread(&studentList[i], sizeof(STUDENTS), 1, fp);
}

int updateStudentData(STUDENTS *studentList, int64_t listSize){
    int error = 1;
    /*fpstrt*/FILE *fp = NULL;//dXBkYXRlU3R1ZGVudERhdGE=
    error = updateStudentDataLogic(fp, studentList, listSize);
    /*fpend*/fclose(fp);//dXBkYXRlU3R1ZGVudERhdGE=
    return error;
}

int loadStudentData(STUDENTS *studentList) {
    int error = 0;
    /*fpstrt*/FILE *fp = NULL;//IGxvYWRTdHVkZW50RGF0YQ==
    error = loadStudentDataLogic(fp, studentList);
    /*fpend*/fclose(fp);//IGxvYWRTdHVkZW50RGF0YQ==
    return error;
}

int readTotalStudentsLogic(FILE *fp,int64_t *totalStudents){
    /*fplock*/fp = fopen(STUDENTDATA, "rb");//IHJlYWRUb3RhbFN0dWRlbnRz
    if(!fp)
        return -1;
    fseek(fp, 0, SEEK_END);
    if(ftell(fp) == 0)
        return 0;
    fseek(fp, 0, SEEK_SET);
    fread(totalStudents, sizeof(int64_t), 1, fp);
    return 0;
}

int64_t readTotalStudents(){
    int64_t totalStudents = 0;
    /*fpstrt*/FILE *fp = NULL;//IHJlYWRUb3RhbFN0dWRlbnRz
    readTotalStudentsLogic(fp, &totalStudents);
    /*fpend*/fclose(fp);//IHJlYWRUb3RhbFN0dWRlbnRz
    if(totalStudents == 0) totalStudents = 1;
    return totalStudents;
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
    if(TXT_CONST-ut_name <= ut_name)
        return -1;
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
            for(int j = 0 ; j < name_len ; j++){//needs testing doesnt work needs to test/fix
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

