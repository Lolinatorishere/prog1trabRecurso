#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdbool.h>
#include "../../../headers/structs.h"
#include "../../../headers/stringParse.h"
#include "../../../headers/defs.h"
#include "../../../headers/Menus/menuMiddleware.h"

USERS setUser(){
    USERS user;
    for(int i = 0 ; i < 256 ; i++){
        user.userName[i] = '\0';
        user.password[i] = '\0';
    }
    user.studentId = -1;
    user.type = -1;
    user.userId = -1;
    return user;
}

void copyUser(USERS *dst, USERS src){
    //doing it like this because of gdb
    //if not done instead of appearing "string" <000> repeats n times
    //it will have "string"<000> random non wiped memory
    for(int i = 0 ; i < strlen(src.userName) ; i++){
        dst[0].userName[i] = src.userName[i];
    }
    for(int i = strlen(src.userName) ; i < 256 ; i++){
        dst[0].userName[i] = '\0';
    }
    for(int i = 0 ; i < strlen(src.password) ; i++){
        dst[0].password[i] = src.password[i];
    }
    for(int i = strlen(src.password) ; i < 256 ; i++){
        dst[0].password[i] = '\0';
    }
    dst[0].type = src.type;
    dst[0].studentId = src.studentId;
    dst[0].userId = src.userId;
}

USERS *searchUsername(USERS *userList, int64_t listSize, char *username, int64_t *index) { // very slow
    int ulen = strlen(username);
    for(int64_t i = 0 ; i < listSize ; i++){
        int check = 0;
        if(ulen != strlen(userList[i].userName))continue;
        for(int j = 0 ; j < ulen; j++){
            if(userList[i].userName[j] != username[j])break;
            check++;
        }
        if(check != ulen) continue;
        return &userList[i];
    }
    return NULL;
}

int searchWithType(USERS *userList, USERS *hits, int64_t listSize, int64_t *hit_size, int type){
    int64_t total = 0;
    for(int i = 0 ; i < listSize ; i++){
        if(userList[i].type != type)continue;
        hits[total] = userList[i];
        total++;
    }
    *hit_size = total;
    return 0;
}

//This is assuming the ids in the List are sequential which they absolutely should be
//binary searches the requested id
USERS *searchUserId(USERS *userList, int64_t listSize, int id, int64_t *index) {
    int64_t low = 0, high = listSize - 1;
    while (low <= high) {
        int64_t mid = low + (high - low) / 2;
        if (userList[mid].userId == id) {
            *index = mid;
            return &userList[mid];
        } else if (userList[mid].userId < id) {
            low = mid + 1;
        } else {
            high = mid - 1;
        }
    }
    return NULL; 
}

USERS *searchUserStudentId(USERS *userList, int64_t listSize, int *studentId, int64_t *index) { // very slow
    for(int64_t i = 0 ; i < listSize ; i++){
        if(userList[i].studentId != *studentId)continue;
        return &userList[i];
    }
    return NULL;
}

int updateUserData(USERS userList[], int64_t listSize){
    FILE *fp = fopen(USERDATA, "wb");
    if(!fp)
        return -1;
    if(fwrite(&listSize, sizeof(int64_t), 1, fp) != 1 ) return -1;
    for(int i = 0 ; i < listSize ; i++){
        if(fwrite(&userList[i], sizeof(USERS), 1, fp) != 1) return -1;
    }
    fclose(fp);
    return 1;
}

int64_t readTotalUsers(){
    int64_t totalUsers = 0;
    FILE *fp = fopen(USERDATA, "rb");
    if(!fp){
        //guarantees the existance of USERDATA dir file
        FILE *makeFile = fopen(USERDATA, "wb");
        fclose(makeFile);
        fp = fopen(USERDATA, "rb");
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
    fread(&totalUsers, sizeof(int64_t), 1, fp);
    fclose(fp);
    if(totalUsers == 0) totalUsers = 1;
    return totalUsers;
}

int loadUserData(USERS *userList) {
    FILE *fp = fopen(USERDATA, "rb");
    if(!fp) return -1;
    fseek(fp, 0, SEEK_END);
    if(ftell(fp) == 0){
        fclose(fp);
        return 1;
    }
    fseek(fp, 0, SEEK_SET);
    int64_t userTotal = 0;
    fread(&userTotal, sizeof(int64_t), 1, fp);
    if(userTotal == 0){
        fclose(fp);
        return 1;
    }
    for(int i = 0 ; i < userTotal ; i++){
        fread(&userList[i], sizeof(USERS), 1, fp);
    }
    fclose(fp);
    return 0;
}

//deprecated
//causes malloc prev vs next courruption errors more frequently for some god forsaken reason
void freeUserData(USERS **userList) {
    if(!userList)
        return;
    free(*userList);
}

//this is seperate because it can be reused for other funcitons, if given other user arrays
//like search for users with type returns multiple users:
//or just a single user like with id serach or uname search

int createUserString(char **string, USERS *users, int userTotal, int usersPerPage, int page){
    //64 for special chars like |,/,\,_,\n name, type, pwd and id
    // 256 * 2 name and pwd, 64, 32 for type and 32 for id all in chars
    // 700 total for memory reasons, dont want buffer overflows
    /* should return something with this type of structure
     * / Id:id
     * | Type: type
     * | Aluno: id
     * | name:
     * \ pwd:
    */
    *string = malloc(sizeof(char) * (usersPerPage * (700)));
    int index = 0;
    char buffer[TXT_CONST];
    int ut_id = strlen(" / alunoId:");
    int ut_type = strlen(" | Tipo:");
    int ut_alun = strlen(" | Aluno:");
    int ut_name = strlen(" | Nome:");
    int ut_pwd = strlen(" \\ Pwd:");
    strcpy((*string), "\0");
    if(TXT_CONST-ut_name <= ut_name)return -1;
    for(int i = (page * usersPerPage); i < usersPerPage + (page * usersPerPage); i++){
        if(i >= userTotal) break;
        USERS user = users[i];
        strcat((*string), " / Id:"); index += ut_id;
        sprintf(buffer, "%i\n", user.userId);
        strcat((*string), buffer); index += strlen(buffer);
        strcat((*string), " | Type:"); index += ut_type;
        sprintf(buffer, "%i\n", user.type);
        strcat((*string), buffer); index += strlen(buffer);
        strcat((*string), " | Aluno:"); index += ut_alun;
        sprintf(buffer, "%i\n", user.studentId);
        strcat((*string), buffer); index += strlen(buffer);
        strcat((*string), " | Name:"); index += ut_name;
        int name_len = strlen(user.userName);
        if(name_len < TXT_CONST-ut_name){
            strcat((*string), user.userName); index += name_len;
        }else{
            for(int j = 0; j < name_len ; j++){//needs testing
                (*string)[j+index] = user.userName[j];
                if(j%TXT_CONST-ut_name == 0){
                    strcat((*string), " |      "); index += ut_name;
                    continue;
                }
                index++;
            }
        }
        strcat((*string), "\n"); index++;
        int pwd_len = strlen(user.password);
        if(pwd_len < TXT_CONST-ut_pwd){
            strcat((*string), " \\ Pwd:"); index += ut_pwd;
            strcat((*string), user.password); index += pwd_len;
        }else{
            strcat((*string), " | Pwd:"); index += ut_pwd;
            for(int j = 0; j < pwd_len; j++){
                (*string)[j+index] = user.password[j];
                if(j%TXT_CONST-ut_name == 0){
                    if(pwd_len - j < TXT_CONST - ut_pwd){
                        strcat((*string), " \\     "); index += ut_pwd;
                    }else{
                        strcat((*string), " |     "); index += ut_pwd;
                    }
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

int createUser(char *username, char *password, int type){
    int64_t userTotal = readTotalUsers();
    USERS *users = malloc(sizeof(USERS) * (userTotal + 1));
    int64_t index = 0;
    switch(loadUserData(users)){
        case 0:
            if(searchUsername(users, userTotal, username, &index) != NULL){
                free(users);
                return 0;
            }
            break;
        case -1:
            free(users);
            return -1;
        default:
            break;
    }
    USERS *newUser = &users[userTotal];
    strncpy(newUser->userName, username, 255);
    newUser->userName[255] = '\0';
    strncpy(newUser->password, password, 255);
    newUser->password[255] = '\0';
    if (userTotal == 0) newUser->type = 100;
    else newUser->type = type;
    if (userTotal == 0) newUser->userId = 1;
    else newUser->userId = users[userTotal - 1].userId + 1;
    newUser->studentId = -1;
    userTotal++;
    if(updateUserData(users, userTotal) == -1)
        return -1;
    free(users);
    return 1;
}

int updateUser(int id, char *username, char *password, int *type, int *studentId){
    if(username[0] == '\0' && password[0] == '\0' && type == NULL && studentId == NULL)return 1;
    int error = 0;
    int64_t index = 0;
    int64_t userTotal = readTotalUsers();
    USERS *users = malloc(sizeof(USERS) * (userTotal+1));
    if(!users){ 
        return -1;
    }
    switch(loadUserData(users)){
        case -1:
            return -1;
        case 1:
            return 1;
        default:
            if(!searchUserId(users, userTotal, id, &index)){
                error = 2;
                break;
            }
            if(username[0] != '\0'){
                int64_t data = 0;
                if(searchUsername(users, userTotal, username, &data) != NULL) {
                    if(users != NULL) free(users);
                    error = 3;
                    break;
                }
                username[strlen(username)] = '\0';
                strcpy(users[index].userName, username);
            }
            if(password[0] != '\0'){
                password[strlen(password)] = '\0';
                strcpy(users[index].password, password);
            }
            if(type != NULL)
                users[index].type = *type;
            if(studentId != NULL)
                users[index].studentId = *studentId;
            updateUserData(users, userTotal);
            break;
    }
    free(users);
    return error;
}

int deleteUser(int id){
    int64_t index = 0;
    USERS *check = NULL;
    USERS user;
    int64_t userTotal = readTotalUsers();
    USERS *users = malloc(sizeof(USERS) * (userTotal + 1));
    if(!users) return -1;
    switch(loadUserData(users)){
        case -1:
            return -1;
        case 1:
            return 1;
        default:
            if(!searchUserId(users, userTotal, id, &index)){
                free(users);
                return 0;
            }
            // aray[x] replaced with [x+n];
            // because i dont know how to do it better kek;
            if(index != userTotal-1){
                for(int i = index ; i < userTotal-1 ; i++){
                    users[i] = users[i+1];
                }
            }
            updateUserData(users, userTotal-1);
            free(users);
            return 0;
    }
}

bool userValidate(char *username,char *password, USERS *user){
    int64_t userTotal = 0;
    int checks = 0;
    userTotal = readTotalUsers();
    USERS *users = malloc(sizeof(USERS) * (userTotal + 1));
    if(!users) return -1;
    *user = setUser();
    if(loadUserData(users) != 0){
        free(users);
        return 1;
    }
    for(int i = 0 ; i < userTotal ; i++){
        int j = 0;
        int uLen = strlen(users[i].userName);
        if(uLen != strlen(username)) continue;
        for(j = 0 ; j < uLen ; j++){
            if(users[i].userName[j] != username[j]) break;
        }
        if(j != uLen) continue;
        int plen = strlen(users[i].password);
        if(plen != strlen(password)) continue;
        for(j = 0 ; j < plen ; j++){
            if(users[i].password[j] != password[j]) break;
        }
        if(j != plen) continue;
        strcpy(user->userName, users[i].userName);
        strcpy(user->password, users[i].password);
        user->type = users[i].type;
        user->userId = users[i].userId;
        user->studentId = users[i].studentId;
        break;
    }
    free(users);
    if(user->type < 0) return -1;
    return 0;
}

int getAllUsers(char **string, int usersPerPage, int *page, char *special){//returns a srting of all users with page addons
    int64_t userTotal = readTotalUsers();
    if(userTotal == 0) return -1;
    int maxPages = userTotal/usersPerPage;
    if(userTotal%usersPerPage != 0){
        maxPages++;
    }
    if (*page >= maxPages) *page = maxPages-1;
    if(*page<0)*page = 0;
    USERS *users = malloc(sizeof(USERS) * (userTotal + 1));
    if(!users)return -1;
    if(loadUserData(users) != 0){
        free(users);
        return -1;
    }
    if(createUserString(string, users, userTotal, usersPerPage, *page) != 0){
        free(users);
        return -1;
    }
    addPageInfo(string, *page, usersPerPage, userTotal, special, "Users");
    free(users);
    return 0;
}

// todo move user pointer to outside so i dont have to repeat the search every fkn time
int searchForUsername(char **string, char *search, int usersPerPage, int page){
    int64_t userTotal = readTotalUsers();
    USERS *users = malloc(sizeof(USERS) * (userTotal + 1));
    int64_t index = 0;
    if(userTotal == 0) return -1;
    if(loadUserData(users) != 0){
        free(users);
        return 1;
    }
    USERS *user = searchUsername(users, userTotal, search, &index);
    if(!user){
        //todo work with deepUsernameSearch instead
        free(users);
        return 1;
    }
    if(createUserString(string, user, 1, 1, 0) != 0){
        free(users);
        return -1;
    }
    free(users);
    return 0;
}

int searchForUserId(char **string, int search, int usersPerPage, int page){
    int64_t userTotal = readTotalUsers();
    USERS *users = malloc(sizeof(USERS) * (userTotal + 1));
    int64_t index = 0;
    if(userTotal == 0) return -1;
    if(loadUserData(users) != 0){
        free(users);
        return 1;
    }
    USERS *user = searchUserId(users, userTotal, search, &index);
    if(user == NULL){
        (*string) = malloc(sizeof(char) * 64);
        strcpy((*string), "Utilizador Nao Existe\n");
    }else{
        if(createUserString(string, user, 1, 1, 0) != 0){
            free(users);
            return -1;
        }
    }
    free(users);
    return 0;
}

int searchForUserType(char **string, USERS **userList, int64_t *totalUsers, int search, int usersPerPage, int *page){
    if((*userList) != NULL){
        int maxPages = *totalUsers/usersPerPage;
        if(*totalUsers%usersPerPage != 0){
            maxPages++;
        }
        if(*page >= maxPages) *page = maxPages-1;
        if(*page<0)*page = 0;
        if(createUserString(string, (*userList), (*totalUsers), usersPerPage, *page) != 0){
            free((*string));
            free((*userList));
            return -1;
        }
        addPageInfo(string, *page, *totalUsers, *totalUsers, NULL, "Users");
        return 0;
    }
    int64_t userTotal = readTotalUsers();
    int64_t hit_size = 0;
    if(userTotal == 0) return -1;
    USERS *users = malloc(sizeof(USERS) * (userTotal + 1));
    if(!users){
        free((*string));
        return -1;
    }
    *userList = malloc(sizeof(USERS) * (userTotal + 1));
    if(!userList){
        free((*string));
        free(users);
        return -1;
    }
    if(loadUserData(users) != 0){
        free((*string));
        free(users);
        return 1;
    }
    searchWithType(users, (*userList), userTotal, &hit_size, search);
    free(users);
    *totalUsers = hit_size;
    if(createUserString(string, (*userList), *totalUsers, usersPerPage, *page) != 0){
        free((*string));
        free(userList);
        return -1;
    }
    addPageInfo(string, *page, usersPerPage, *totalUsers, NULL, "Users");
    return 0;
}

int getUser(USERS *user, int id){
    int64_t userTotal = readTotalUsers();
    int64_t index = 0;
    USERS *temp;
    USERS *users = malloc(sizeof(USERS) * (userTotal + 1));
    if(loadUserData(users) != 0){
        free(users);
        return -1;
    }
    temp = searchUserId(users, userTotal, id, &index);
    if(temp == NULL){free(users); return 1;}
    copyUser(user, (*temp));
    return 0;
}
