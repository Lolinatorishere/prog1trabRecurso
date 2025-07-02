#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <stdbool.h>
#include "../headers/screenPrint.h"
#include "../headers/stringParse.h"
#include "../headers/defs.h"
#include "../headers/user.h"
#include "../headers/menu.h"

int main(){
    //check first time boot:
    if(startUpCheck() != 0) return -1;
    //start program:
    initMenu();
    return 0;
}
