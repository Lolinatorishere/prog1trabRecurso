#ifndef USER_H
#define USER_H
#include "./structs.h"
#include "stdbool.h"

USERS setUser();

int64_t readTotalUsers();

int createUser(USERS *userCreate, int type);

int updateUser(int id, USERS *update);

int deleteUser(int userId);

bool userValidate(char *username, char *password, USERS *user);

//returns a string with all users formated
int getAllUsers(char **string, int usersPerPage, int *page, char *special);

int searchForUsername(char **string, char *search, int usersPerPage, int page);

int searchForUserId(char **string, int search, int usersPerPage, int page);

int searchForUserType(char **string, USERS **userList, int64_t *totalUsers, int search, int usersPerPage, int *page);

int getUser(USERS *user, int id);

int getUsers(USERS *hits, int *ids, int userAmount);

#endif
