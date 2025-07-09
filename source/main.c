#include "../headers/menu.h"

int main(){
    //check first time boot:
    if(startUp(0) != 0)
        return -1;
    //start program:
    initMenu();
    return 0;
}
