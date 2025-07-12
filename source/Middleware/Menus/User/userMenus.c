void userNotifs(USERS *user){
    return;
}

void userEvent(USERS *user){
    char buffer[256];
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
                //administartion of events
                eventAdmin();
                continue;
            case 0:
                printf("Logout de %s, Adeus\n", user->userName);
                returnText("Login Menu", 3);
                *user = setUser();
                return;
            default:
                break;
        }
    }
}


