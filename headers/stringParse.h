#ifndef STRINGPARSE_H
#define STRINGPARSE_H
/*
 obtains user input and returns the string with the replace value
 if replace is empty or null will replace with nothing
*/
int stringReplace(char *input, char *filter, char *replace);

int centerString(int size, char *text);

void trim(char *str);

//int intNumberInput(char *input, int *output);

int64_t int64FromString(char *input);

//int floatNumberInput(char *input, float *output);

double doubleFromString(char *input);

#endif
