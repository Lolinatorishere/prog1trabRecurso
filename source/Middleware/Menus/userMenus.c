
void NewUser(int admin){
    char username[256] = {'\0'};
    char password[256] = {'\0'};
    menuPrint("NewUser", 1, 1);
    printf("username:");
    fgets(username, 256, stdin);
    if(strlen(username) == 0 || username[0] == '\n'){
        printf("username cant be null\n");
        sleep(1);
        return;
    }
    for(int i = strlen(username)-1 ; i < 256 ; i++)
        username[i] = '\0';
    trim(username);
    printf("password:");
    fgets(password, 256, stdin);
    if(strlen(password) == 0 || password[0] == '\n'){
        printf("password cant be null\n");
        sleep(1);
        return;
    }
    for(int i = strlen(password)-1 ; i < 256 ; i++)
        password[i] = '\0';
    trim(password);
    switch(createUser(username,password, 0)){
        case 1:
            printf("\nuser %s criado com sucesso\n", username);
            sleep(1);
            break;
        case 0:
            printf("\n%s ja existe\n", username);
            sleep(1);
            break;
        case -1:
            printf("\nHouve um erro a carregar os utilizadores\n");
            sleep(1);
            break;
        default:
            printf("that was an issue");
            sleep(1);
            return;
        }
    return;
}

void userIndexMenu(){
    int page = 0;
    char *menuText = NULL;
    char buffer[256] = {'\0'};
    while(buffer[0] != '0'){
        getAllUsers(&menuText, 5, &page, NULL);
        advancedPrint(menuText, 1, 1);
        //handle menuInput
        fgets(buffer, 256, stdin);
        if(buffer[0] == '+')
            page++;
        if(buffer[0] == '-')
            page--;
    }
    free(menuText);
    return;
}

void userDeleteMenu(USERS adminUser){
    char extras[256] = "sel \"ID\" para remover utilizador\n";
    USERS user;
    int page = 0;
    int selectedID = 0;
    char buffer[256] = {'\0'};
    char *menuText = NULL;
    char confirm[256] = "C E R T E Z A";
    char toPrint[512] = {'\0'};
    int str_cmp = 0;
    while(buffer[0] != '0'){
        getAllUsers(&menuText, 5, &page, extras);
        advancedPrint(menuText, 1, 1);
        fgets(buffer, 256, stdin);
        if(buffer[0] == '+'){
            page++;
        }
        if(buffer[0] == '-'){
            page--;
        }
        if(buffer[0] == 's' && buffer[1] == 'e' && buffer[2] == 'l'){
            selectedID = int64FromString(buffer);
            if(selectedID == adminUser.userId){
                advancedPrint("Nao se pode apagar o utilizador proprio\n", 1, 1);
                continue;
            }
            if(getUser(&user, selectedID) != 0){
                free(menuText);
                menuPrint("userNonExists", 1, 1);
                sleep(1);
                return;
            }
            sprintf(toPrint, "Tem A certeza que quer apagar\nUtilizador:\"%s\"\nTipo:%i\n", user.userName, user.type);
            advancedPrint(toPrint, 1, 1);
            printf("escreve: \"C E R T E Z A\", para confirmar");
            fgets(buffer, 256, stdin);
            for(int i = 0 ; i < strlen(buffer); i++){
                if(buffer[i] != confirm[i]) break;
                str_cmp++;
            }
            if(str_cmp != strlen(confirm))continue;
            if(deleteUser(selectedID) != 0){
                free(menuText);
                menuPrint("Error", 4, 4);
                sleep(1);
                return;
            }
        }
    }
    free(menuText);
    return;
}

void usernameSearch(){
    char search[256] = {'\0'};
    char *menuText = NULL;
    int64_t input = 0;
    int page = 0;
    while(1){
        menuPrint("searchingUser", 1, 1);
        printf("username:");
        fgets(search,256, stdin);
        search[strlen(search)-1] = '\0';
        switch(searchForUsername(&menuText, search, 5, page)){
        case 0:
            advancedPrint(menuText, 1, 1);
        break;
        case -1:
            free(menuText);
            return;
        default:
            menuPrint("userNonExists", 1, 1);
            break;
        }
        advancedPrint(menuText, 1, 1);
        free(menuText);
        fgets(search,256, stdin);
        input = int64FromString(search);
        if(input == 0) return;
    }
}

void idSearch(){
    char buffer[256] = {'\0'};
    char *menuText = NULL;
    int64_t input = 0;
    int page = 0;
    while(1){
        menuPrint("searchingUser", 1, 1);
        printf("user ID:");
        fgets(buffer, 256, stdin);
        input = int64FromString(buffer);
        searchForUserId(&menuText, input, 5, page);
        advancedPrint(menuText, 1, 1);
        free(menuText);
        printf("para sair carrega 0:");
        fgets(buffer, 256, stdin);
        input = int64FromString(buffer);
        if(input == 0) return;
    }
}

void typeSearch(){
    char buffer[256] = {'\0'};
    char *menuText = NULL;
    USERS *users = NULL;
    int64_t totalUsers = 0;
    int64_t input = 0;
    int page = 0;
    while(1){
        menuPrint("searchingUser", 1, 1);
        printf("user tipo:");
        fgets(buffer,256, stdin);
        input = int64FromString(buffer);
        searchForUserType(&menuText, &users, &totalUsers, input, 5, &page);
        if(totalUsers == 0){
            menuPrint("userNonExists", 1, 1);
            free(menuText);
            free(users);
            return;
        }
        strcpy(buffer, "\0");
        while(1){
            searchForUserType(&menuText, &users, &totalUsers, input, 5, &page);
            advancedPrint(menuText, 1, 1);
            fgets(buffer, 256, stdin);
            if(buffer[0] == '+')
                page++;
            if(buffer[0] == '-')
                page--;
            if(buffer[0] == '0')
                break;
        }
        free(menuText);
        free(users);
        return;
    }
}

void userSearchMenu(){
    char *menuText = NULL;
    char buffer[256] = {'\0'};
    while(1){
        menuPrint("searchUser", 1, 1);
        //handle menuInput
        fgets(buffer, 256, stdin);
        int64_t input = int64FromString(buffer);
        switch(input){
            //1 - Username
            //2 - Id user
            //3 - Tipo user
            //0 - sair
            case 1:
                usernameSearch(); 
                return;
            case 2:
                idSearch();
                continue;
            case 3:
                typeSearch();
                continue;
            case 0:
                free(menuText);
                return;
            default:
                continue;
        }
    }
}

void editingUser(char *buffer, char *menuText, USERS adminUser, int page){
    USERS user = setUser();
    char password[256] = {'\0'},
         username[256] = {'\0'},
         editFunc[256] = "\n1 - username\n2 - password\n3 - type\n0 - voltar\n",
         edit[256] = {'\0'};
    int input = 0,
        *type = NULL,
        *alunoId = NULL,
        selectedID = 0;
    selectedID = int64FromString(buffer);
    if(getUser(&user, selectedID) < 0){
        free(menuText);
        return;
    }
    if(user.userId == -1){
        menuPrint("userNonExists", 1, 1);
        sleep(1);
        return;
    }
    while(buffer[0] != '0'){
        type = NULL;
        searchForUserId(&menuText, selectedID, 5, page);
        char *copyhere = malloc(sizeof(char)*strlen(menuText));
        strcpy(copyhere, menuText);
        char *temp = realloc(menuText, sizeof(char) * sizeof(menuText) + strlen(editFunc));
        menuText = temp;
        strcpy(menuText, copyhere);
        free(copyhere);
        strcat(menuText, editFunc);
        advancedPrint(menuText, 1, 1);
        fgets(buffer, 256, stdin);
        input = int64FromString(buffer);
        if(buffer[0] == '\n' || buffer[0] == '\0'){
            free(menuText);
            return;
        }
        if(input == 0){
            buffer[0] = '0';
            continue;
        }
        strncpy(username, "\0", 256);
        strncpy(password, "\0", 256);
        if(input == 1){
            printf("\nUsername:");
            fgets(username, 256, stdin);
            trim(username);
            if(strlen(username) < 1)
                continue;
        }
        if(input == 2){
            printf("\npassword:");
            fgets(password, 256, stdin);
            trim(password);
            if(strlen(password) < 1)
                continue;
        }
        if(input == 3){
            if(adminUser.userId == selectedID){
                printf("Administradores nao podem alterar o seu tipo");
                sleep(1);
                continue;
            }
            printf("\ntipo:");
            fgets(buffer, 256, stdin);
            *type = int64FromString(buffer);
        }
        //if(input == 4){
        //    if(adminUser.alunoId == selectedID){
        //        printf("Administradores nao podem alterar o seu tipo");
        //        continue;
        //    }
        //    printf("\ntipo:");
        //    fgets(buffer, 256, stdin);
        //    input = int64FromString(buffer);
        //}
        switch(updateUser(user.userId, username, password, type, alunoId)){
            case 3:
                printf("username Not Unique\n");
                sleep(1);
                break;
            case -1:
                printf("System Error\n");
                sleep(1);
                return;
            case 0:
                printf("user updated\n");
                sleep(1);
                break;
            default:
                break;
        }
        strncpy(username, "\0", 256);
        strncpy(password, "\0", 256);
        strcpy(buffer, "\0");
    }
    return;
}

void userAlterMenu(USERS adminUser){
    char extras[256] = "sel \"ID\" para editar utilizador\n";
    int page = 0;
    int input = 0;
    char buffer[256] = {'\0'};
    char *menuText = NULL;
    while(buffer[0] != '0'){
        input = 0;
        getAllUsers(&menuText, 5, &page, extras);
        advancedPrint(menuText, 1, 1);
        fgets(buffer, 256, stdin);
        if(buffer[0] == '+')
            page++;
        if(buffer[0] == '-')
            page--;
        if(buffer[0] == 's' && buffer[1] == 'e' && buffer[2] == 'l')
            editingUser(buffer, menuText, adminUser, page);
    }
    free(menuText);
    return;
}

void userStudentMenu(USERS adminUser){
    char extras[256] = "sel \"ID\" para editar utilizador\n";
    int page = 0;
    int input = 0;
    char buffer[256] = {'\0'};
    char *menuText = NULL;
    while(buffer[0] != '0'){
        input = 0;
        getAllUsers(&menuText, 5, &page, extras);
        advancedPrint(menuText, 1, 1);
        fgets(buffer, 256, stdin);
        if(buffer[0] == '+')
            page++;
        if(buffer[0] == '-')
            page--;
        if(buffer[0] == 's' && buffer[1] == 'e' && buffer[2] == 'l')
            editingUser(buffer, menuText, adminUser, page);
    }
        free(menuText);
    return;
}

void userAdmin(USERS user){
    char buffer[256];
    while(1){
        menuPrint("userControls", 1, 1);
        printf("input:");
        fgets(buffer, 256, stdin);
        int64_t input = int64FromString(buffer);
        //1 - Criar Um Novo Utilizador
        //2 - Ver Utilizadores registados
        //3 - Alterar utilizador registado
        //4 - Apagar Utilizador do Sistema
        //5 - Procurar Utilizador do Sistema
        switch(input){
            case 1:
                NewUser(100);
                continue;
            case 2:
                userIndexMenu();
                continue;
            case 3:
                userAlterMenu(user);
                continue;
            case 4:
                userDeleteMenu(user);
                continue;
            case 5:
                userSearchMenu();
                continue;
            case 6:
                userStudentMenu(user);
                continue;
            case 0:
                return;
            default:
                continue;
        }
    }
}
