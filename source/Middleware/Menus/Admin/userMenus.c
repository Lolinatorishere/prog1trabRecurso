
void NewUser(int admin){
    USERS userCreate = setUser();
    char username[256] = {'\0'};
    char password[256] = {'\0'};
    char studentName[256] = {'\0'};
    menuPrint("NewUser", 1, 1);
    printf("username:");
    fgets(username, 256, stdin);
    if(strlen(username) == 0 || username[0] == '\n'){
        printf("username nao pode ser null\n");
        sleep(1);
        return;
    }
    for(int i = strlen(username)-1 ; i < 256 ; i++)
        username[i] = '\0';
    trim(username);
    printf("password:");
    fgets(password, 256, stdin);
    if(strlen(password) == 0 || password[0] == '\n'){
        printf("password nao pode ser null\n");
        sleep(1);
        return;
    }
    for(int i = strlen(password)-1 ; i < 256 ; i++)
        password[i] = '\0';
    trim(password);
    printf("Nome Estudante:");
    fgets(password, 256, stdin);
    if(strlen(password) == 0 || password[0] == '\n'){
        printf("Estudante nao pode ser null\n");
        sleep(1);
        return;
    }
    for(int i = strlen(password)-1 ; i < 256 ; i++)
        password[i] = '\0';
    trim(password);
    strncpy(userCreate.userName, username, 256);
    strncpy(userCreate.password, password, 256);
    strncpy(userCreate.studentName, password, 256);
    switch(createUser(&userCreate, 0)){
        case 0:
            printf("\nuser %s criado com sucesso\n", username);
            sleep(1);
            break;
        case 1:
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
        advancedPrint(menuText, 1, 1, 1);
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
    USERS user;
    int page = 0,
        selectedID = 0,
        str_cmp = 0;
    char confirm[256] = "C E R T E Z A",
         extras[256] = "sel \"ID\" para remover utilizador\n",
         *menuText = NULL,
         toPrint[512] = {'\0'},
         buffer[256] = {'\0'};
    while(buffer[0] != '0'){
        getAllUsers(&menuText, 5, &page, extras);
        advancedPrint(menuText, 1, 1, 1);
        fgets(buffer, 256, stdin);
        if(buffer[0] == '+')
            page++;
        if(buffer[0] == '-')
            page--;
        if(buffer[0] == 's' && buffer[1] == 'e' && buffer[2] == 'l'){
            selectedID = int64FromString(buffer);
            if(selectedID == adminUser.userId){
                advancedPrint("Nao se pode apagar o utilizador proprio\n ", 1, 1, 0);
                continue;
            }
            if(getUser(&user, selectedID) != 0){
                menuPrint("userNonExists", 1, 1);
                sleep(1);
                goto cleanup;
            }
            sprintf(toPrint, "Tem A certeza que quer apagar\nUtilizador:\"%s\"\nTipo:%i\n ", user.userName, user.type);
            advancedPrint(toPrint, 1, 1, 0);
            printf("escreve: \"C E R T E Z A\", para confirmar");
            fgets(buffer, 256, stdin);
            for(int i = 0 ; i < strlen(buffer); i++){
                if(buffer[i] != confirm[i])
                    break;
                str_cmp++;
            }
            if(str_cmp != strlen(confirm))
                continue;
            if(deleteUser(selectedID) != 0){
                menuPrint("Error", 4, 4);
                sleep(1);
                goto cleanup;
            }
        }
    }
cleanup:
    free(menuText);
    return;
}

void usernameSearch(){
    char search[256] = {'\0'},
         *menuText = NULL;
    int64_t input = 0;
    int page = 0;
    while(1){
        menuPrint("searchingUser", 1, 1);
        printf("username:");
        fgets(search,256, stdin);
        search[strlen(search)-1] = '\0';
        trim(search);
        switch(searchForUsername(&menuText, search, 5, page)){
            case 0:
                advancedPrint(menuText, 1, 1, 1);
                break;
            case -1:
                goto cleanup;
            default:
                menuPrint("userNonExists", 1, 1);
                break;
        }
        advancedPrint(menuText, 1, 1, 1);
        fgets(search,256, stdin);
        input = int64FromString(search);
        if(input == 0)
            goto cleanup;
    }
cleanup:
    free(menuText);
    return;
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
        advancedPrint(menuText, 1, 1, 1);
        free(menuText);
        printf("para sair carrega 0:");
        fgets(buffer, 256, stdin);
        input = int64FromString(buffer);
        if(input == 0) return;
    }
}

void typeSearch(){
    USERS *users = NULL;
    int64_t totalUsers = 0;
    int64_t input = 0;
    char buffer[256] = {'\0'};
    char *menuText = NULL;
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
            advancedPrint(menuText, 1, 1, 1);
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

int editingUser(char *buffer, char **menuText, USERS adminUser, int page){
    USERS user = setUser(),
          editer = setUser();
    char editFunc[256] = "\n1 - username\n2 - password\n3 - studentName\n4 - type\n0 - voltar\n ",
         edit[256] = {'\0'},
         *copyhere = NULL,
         *temp = NULL;
    int input = 0,
        selectedID = int64FromString(buffer);
    if(getUser(&user, selectedID) < 0)
        return -1;
    if(user.userId == -1){
        menuPrint("userNonExists", 1, 1);
        sleep(1);
        return -1;
    }
    while(buffer[0] != '0'){
        editer = setUser();
        searchForUserId(menuText, selectedID, 5, page);
        copyhere = (char*) malloc(sizeof(char) * (strlen((*menuText)) + 2));
        strcpy(copyhere, (*menuText));
        temp = (char*) realloc((*menuText), sizeof(char) * (strlen((*menuText)) + strlen(editFunc) + 2));
        (*menuText) = temp;
        strcpy((*menuText), copyhere);
        free(copyhere);
        strcat((*menuText), editFunc);
        advancedPrint((*menuText), 1, 1, 1);
        fgets(buffer, 256, stdin);
        input = int64FromString(buffer);
        if(buffer[0] == '\n' || buffer[0] == '\0')
            return -1;
        if(input == 0){
            buffer[0] = '0';
            continue;
        }
        if(input == 1){
            printf("\nUsername:");
            fgets(editer.userName, 256, stdin);
            trim(editer.userName);
            if(strlen(editer.userName) < 1)
                continue;
        }
        if(input == 2){
            printf("\npassword:");
            fgets(editer.password, 256, stdin);
            trim(editer.password);
            if(strlen(editer.password) < 1)
                continue;
        }
        if(input == 3){
            printf("\nnovo nome do Aluno:");
            fgets(editer.studentName, 256, stdin);
            trim(editer.password);
            if(strlen(editer.password) < 1)
                continue;
        }
        if(input == 4){
            if(adminUser.userId == selectedID){
                printf("Administradores nao podem alterar o seu tipo");
                sleep(1);
                continue;
            }
            printf("\ntipo:");
            fgets(buffer, 256, stdin);
            editer.type = int64FromString(buffer);
        }
        switch(updateUser(user.userId, &editer)){
            case 3:
                printf("username Not Unique\n");
                sleep(1);
                break;
            case -1:
                printf("System Error\n");
                sleep(1);
                return -1;
            case 0:
                printf("user updated\n");
                sleep(1);
                break;
            default:
                break;
        }
    }
    return -1;
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
        advancedPrint(menuText, 1, 1, 1);
        fgets(buffer, 256, stdin);
        if(buffer[0] == '+')
            page++;
        if(buffer[0] == '-')
            page--;
        if(buffer[0] == 's' && buffer[1] == 'e' && buffer[2] == 'l')
            editingUser(buffer, &menuText, adminUser, page);
    }
    if(menuText)
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
        advancedPrint(menuText, 1, 1, 1);
        fgets(buffer, 256, stdin);
        if(buffer[0] == '+')
            page++;
        if(buffer[0] == '-')
            page--;
        if(buffer[0] == 's' && buffer[1] == 'e' && buffer[2] == 'l')
            editingUser(buffer, &menuText, adminUser, page);
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
            case 0:
                return;
            default:
                continue;
        }
    }
}
