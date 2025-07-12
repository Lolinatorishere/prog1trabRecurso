void userNotifs(USERS *user){
    return;
}

void userEvent(USERS *user){
    char buffer[256] = {'\0'};
    int64_t input = 0;
    while(1){
        menuPrint("userEvents", 1, 1);
        printf("input:");
        fgets(buffer, 256, stdin);
        input = int64FromString(buffer);
        switch(input){
            case 1:
                userEventIndexMenu(user);
                continue;
            case 2:
                listUserEvents(user);
                continue;
            case 0:
                return;
            default:
                return;
        }
    }
}


