#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdbool.h>
#include "../../../headers/structs.h"
#include "../../../headers/stringParse.h"
#include "../../../headers/defs.h"
#include "../../../headers/Menus/menuMiddleware.h"
int64_t TotalUserAmount = 0;

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
    if(listSize == 0)
        return NULL;
    int ulen = strlen(username);
    for(int64_t i = 0 ; i < listSize ; i++){
        int check = 0;
        if(ulen != strlen(userList[i].userName))continue;
        for(int j = 0 ; j < ulen ; j++){
            if(userList[i].userName[j] != username[j])break;
            check++;
        }
        if(check != ulen) continue;
        return &userList[i];
    }
    return NULL;
}

int searchWithType(USERS *userList, USERS *hits, int64_t listSize, int64_t *hit_size, int type){
    if(listSize == 0)
        return -1;
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
    if(listSize == 0)
        return NULL;
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
    if(listSize == 0)
        return NULL;
    for(int64_t i = 0 ; i < listSize ; i++){
        if(userList[i].studentId != *studentId)continue;
        return &userList[i];
    }
    return NULL;
}

int updateUserData(USERS *userList, int64_t listSize){
    FILE *fp = fopen(USERDATA, "wb");
    if(!fp)
        return -1;
    if(fwrite(&listSize, sizeof(int64_t), 1, fp) != 1 ) 
        return -1;
    for(int i = 0 ; i < listSize ; i++){
        if(fwrite(&userList[i], sizeof(USERS), 1, fp) != 1) 
            return -1;
    }
    fclose(fp);
    fp = NULL;
    return 1;
}

int64_t readTotalUsers(){
    int64_t totalUsers = 0;
    FILE *fp = fopen(USERDATA, "rb");
    if(TotalUserAmount == 0){
        if(!fp)
            return 0;
        if(fseek(fp, 0, SEEK_END) != 0)
            goto cleanup;
        if(ftell(fp) == 0)
            goto cleanup;
        if(fseek(fp, 0, SEEK_SET) != 0)
            goto cleanup;
        fread(&totalUsers, sizeof(int64_t), 1, fp);
        TotalUserAmount = totalUsers;
        goto cleanup;
    }
    totalUsers = TotalUserAmount;
    goto cleanup;
cleanup:
    if(fp)
        fclose(fp);
    fp = NULL;
    return totalUsers;
}

int loadUserData(USERS *userList) {
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
    int64_t userTotal = 0;
    fread(&userTotal, sizeof(int64_t), 1, fp);
    if(userTotal == 0){
        error = 1;
        goto cleanup;
    }
    for(int i = 0 ; i < userTotal ; i++)
        fread(&userList[i], sizeof(USERS), 1, fp);
    goto cleanup;
cleanup:
    fclose(fp);
    fp = NULL;
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
    if(*string == NULL){
        *string = malloc(sizeof(char) * (usersPerPage * (700)));
    } else {
        char *temp = realloc(*string, sizeof(char) * (usersPerPage * (700)));
        *string = temp;
    }
    int index = 0;
    char buffer[TXT_CONST];
    int ut_id = strlen(" / alunoId:");
    int ut_type = strlen(" | Tipo:");
    int ut_alun = strlen(" | Aluno:");
    int ut_name = strlen(" | Nome:");
    int ut_pwd = strlen(" \\ Pwd:");
    strcpy((*string), "\0");
    if(TXT_CONST-ut_name <= ut_name)
        return -1;
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
            for(int j = 0 ; j < name_len ; j++){//needs testing
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
            for(int j = 0 ; j < pwd_len ; j++){
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
    int64_t userTotal = readTotalUsers(),
            index = 0;
    USERS *users = NULL;
    users = malloc(sizeof(USERS) * (userTotal + 1));
    if(!users)
        return -1;
    int error = loadUserData(users);
    if(error == -1)
        goto cleanup;
    if(searchUsername(users, userTotal, username, &index)){
        error = 1;
        goto cleanup;
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
    if(updateUserData(users, userTotal) == -1){
        error = -1;
        goto cleanup;
    }
    if(TotalUserAmount != userTotal)
        TotalUserAmount = userTotal;
    goto cleanup;
cleanup:
    free(users);
    return error;
}

int updateUser(int id, char *username, char *password, int *type, int *studentId){
    if(username[0] == '\0' && password[0] == '\0' && type == NULL && studentId == NULL)
        return 1;
    int error = 0;
    int64_t index = 0,
            userTotal = readTotalUsers(),
            data = 0;
    USERS *users = NULL;
    users = malloc(sizeof(USERS) * (userTotal+1));
    if(!users)
        return -1;
    error = loadUserData(users);
    if(error != 0)
        goto cleanup;
    if(!searchUserId(users, userTotal, id, &index)){
        error = 2;
        goto cleanup;
    }
    if(username[0] != '\0'){
        if(searchUsername(users, userTotal, username, &data)){
            error = 3;
            goto cleanup;
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
    goto cleanup;
cleanup:
    free(users);
    return error;
}

int deleteUser(int id){
    int64_t index = 0,
            userTotal = readTotalUsers();
    USERS *check = NULL,
          *users = malloc(sizeof(USERS) * (userTotal + 1)),
          user;
    if(!users)
        return -1;
    int error = loadUserData(users);
    if(error != 0)
        goto cleanup;
    if(!searchUserId(users, userTotal, id, &index)){
        error = -1;
        goto cleanup;
    }
    // aray[x] replaced with [x+n];
    // because i dont know how to do it better kek;
    if(index < userTotal-1)
        for(int i = index ; i < userTotal-1 ; i++)
            users[i] = users[i+1];
    updateUserData(users, userTotal-1);
    goto cleanup;
cleanup:
    free(users);
    return error;
}

bool userValidate(char *username,char *password, USERS *user){
    int64_t userTotal = readTotalUsers();;
    int checks = 0,
        error = 0,
        uCheck = strlen(username),
        pCheck = strlen(password);
    USERS *users = malloc(sizeof(USERS) * (userTotal + 1));
    if(!users)
        return -1;
    *user = setUser();
    error = loadUserData(users);
    if(error != 0)
        goto cleanup;
    for(int i = 0 ; i < userTotal ; i++){
        int j = 0;
        int uLen = strlen(users[i].userName);
        if(uLen != uCheck)
            continue;
        for(j = 0 ; j < uLen ; j++)
            if(users[i].userName[j] != username[j])
                break;
        if(j != uLen)
            continue;
        int plen = strlen(users[i].password);
        if(plen != pCheck)
            break;
        for(j = 0 ; j < plen ; j++)
            if(users[i].password[j] != password[j])
                break;
        if(j != plen)
            continue;
        strcpy(user->userName, users[i].userName);
        strcpy(user->password, users[i].password);
        user->type = users[i].type;
        user->userId = users[i].userId;
        user->studentId = users[i].studentId;
        break;
    }
    if(user->type < 0)
        error = -1;
    goto cleanup;
cleanup:
    free(users);
    return error;
}

//returns a srting of all users with page addons
int getAllUsers(char **string, int usersPerPage, int *page, char *special){
    int64_t userTotal = readTotalUsers();
    USERS *users = malloc(sizeof(USERS) * (userTotal + 1));
    int maxPages = userTotal/usersPerPage,
        error = 0;
    if(!users)
        return -1;
    if(userTotal%usersPerPage != 0)
        maxPages++;
    if (*page >= maxPages)
        *page = maxPages-1;
    if(*page < 0)
        *page = 0;
    error = loadUserData(users);
    if(error != 0)
        goto cleanup;
    error = createUserString(string, users, userTotal, usersPerPage, *page);
    if(error != 0)
        goto cleanup;
    addPageInfo(string, *page, usersPerPage, userTotal, special, "Users");
    goto cleanup;
cleanup:
    free(users);
    return error;
}

// todo move user pointer to outside so i dont have to repeat the search every fkn time
int searchForUsername(char **string, char *search, int usersPerPage, int page){
    int64_t userTotal = readTotalUsers(),
            index = 0;
    USERS *users = malloc(sizeof(USERS) * (userTotal + 1)),
          *user = NULL;
    if(!users)
        return -1;
    int error = loadUserData(users);
    if(error != 0)
        goto cleanup;
    user = searchUsername(users, userTotal, search, &index);
    if(!user){
        error = -1;
        goto cleanup;
    }
    error = createUserString(string, user, 1, 1, 0);
    if(error != 0)
        goto cleanup;
    goto cleanup;
cleanup:
    free(users);
    return error;
}

int searchForUserId(char **string, int search, int usersPerPage, int page){
    int64_t userTotal = readTotalUsers(),
            index = 0;
    USERS *users = malloc(sizeof(USERS) * (userTotal + 1)),
          *user = NULL;
    if(!users)
        return -1;
    int error = loadUserData(users);
    if(error != 0)
        goto cleanup;
    user = searchUserId(users, userTotal, search, &index);
    if(!user){
        (*string) = malloc(sizeof(char) * 64);
        strcpy((*string), "Utilizador Nao Existe\n");
        goto cleanup;
    }
    error = createUserString(string, user, 1, 1, 0);
    if(error != 0)
        goto cleanup;
cleanup:
    free(users);
    return error;
}

int searchForUserType(char **string, USERS **userList, int64_t *totalUsers, int search, int usersPerPage, int *page){
    int error = 0;
    if((*userList) != NULL){
        int maxPages = *totalUsers/usersPerPage;
        if(*totalUsers%usersPerPage != 0)
            maxPages++;
        if(*page >= maxPages)
            *page = maxPages-1;
        if(*page < 0)
            *page = 0;
        error = createUserString(string, (*userList), (*totalUsers), usersPerPage, *page);
        if(error != 0)
            return -1;
        addPageInfo(string, *page, *totalUsers, *totalUsers, NULL, "Users");
        return 0;
    }
    int64_t userTotal = readTotalUsers();
    int64_t hit_size = 0;
    if(userTotal == 0)
        return -1;
    USERS *users = malloc(sizeof(USERS) * (userTotal + 1));
    if(!users){
        error = -1;
        goto cleanup;
    }
    *userList = malloc(sizeof(USERS) * (userTotal + 1));
    if(!userList){
        error = -1;
        goto cleanup;
    }
    error = loadUserData(users);
    if(error != 0)
        goto cleanup;
    searchWithType(users, (*userList), userTotal, &hit_size, search);
    *totalUsers = hit_size;
    error = createUserString(string, (*userList), *totalUsers, usersPerPage, *page);
    if(error != 0)
        goto cleanup;
    addPageInfo(string, *page, usersPerPage, *totalUsers, NULL, "Users");
    goto cleanup;
cleanup:
    if(users)
        free(users);
    return error;
}

int getUser(USERS *user, int id){
    int64_t userTotal = readTotalUsers();
    int64_t index = 0;
    USERS *temp = NULL,
          *users = NULL;
    users = malloc(sizeof(USERS) * (userTotal + 1));
    if(!users)
        return -1;
    int error = loadUserData(users);
    if(error != 0)
        goto cleanup;
    temp = searchUserId(users, userTotal, id, &index);
    if(!temp){
        error = 1;
        goto cleanup;
    }
    copyUser(user, (*temp));
    goto cleanup;
cleanup:
    free (users);
    return error;
}

int getUsers(USERS *hits, int *ids, int userAmount){
    int64_t userTotal = readTotalUsers(),
            index = 0;
    USERS *users = malloc(sizeof(USERS) * (userTotal + 1));
    if(!users)
        return -1;
    int error = loadUserData(users);
    if(error != 0)
        goto cleanup;
    for(int i = 0 ; i < userAmount ; i++){
        USERS *user = searchUserId(users, userTotal, ids[i], &index);
        copyUser(&hits[i], *user);
        if(&hits[i] == NULL)
            hits[i] = setUser();
    }
    goto cleanup;
cleanup:
    free(users);
    return error;
}
