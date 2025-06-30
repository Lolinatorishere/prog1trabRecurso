#ifndef DEFS_H
#define DEFS_H

    #ifdef _WIN32
        #define syscls system("cls")
    #endif
    #ifdef __linux__
        #define syscls system("clear")
    #endif
    #ifndef TXT_CONST
    #define TXT_CONST 56
    #endif
    #ifndef INDENT
    #define INDENT 4
    #endif
    #ifndef MARGIN
    #define MARGIN 4
    #endif
    #ifndef isasciinum
    #define isasciinum(STRING)\
        if(STRING >= 48 && STRING <= 57)
    #endif
    #ifndef USERDATA
        #define USERDATA "./Data/userData.dat"
    #endif
    #ifndef EVENTDATA
        #define EVENTDATA "./Data/eventData.dat"
    #endif

#endif
