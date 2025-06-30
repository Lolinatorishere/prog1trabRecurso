#include "./structs.h"

#ifndef LOGIN_H
#define LOGIN_H

USERS setUser();

int64_t readTotalUsers();

int createUser(char *username, char *password, int type);

int updateUser(int id, char *username, char *password, int *type);

int deleteUser(int userId);

int userValidate(char *username,char *password, USERS *user);

int showAllUsers(char **string, int usersPerPage, int *page, char *extras);

int searchForUsername(char **string, char *search, int usersPerPage, int page);

int searchForUserId(char **string, int search, int usersPerPage, int page);

int searchForUserType(char **string, USERS **userList, int64_t *totalUsers, int search, int usersPerPage, int *page);

int getUserWithId(USERS *user, int id);

#endif
