#ifndef DEFS_H
#define DEFS_H
    //macro substitutes
    #ifdef _WIN32
        #define syscls system("cls")
    #endif
    #ifdef __linux__
        #define syscls system("clear")
    #endif
    //function macros
    #ifndef isasciinum
    #define isasciinum(STRING)\
        if(STRING >= 48 && STRING <= 57)
    #endif
    #ifndef isNonValid
    #define isNonValid(STRING)\
        if(STRING == ' ' || STRING == '\t' || STRING == '\n' || STRING == '\r' || STRING == '\v' || STRING == '\f' || STRING == '\0')
    #endif
    //program definitions
    #ifndef TXT_CONST
    #define TXT_CONST 56
    #endif
    #ifndef INDENT
    #define INDENT 4
    #endif
    #ifndef MARGIN
    #define MARGIN 4
    #endif
    //file definitions
    #ifndef DATADIR
        #define DATADIR "./Data"
    #endif
    #ifndef USERDATA
        #define USERDATA "./Data/userData.dat"
    #endif
    #ifndef STUDENTDATA
        #define STUDENTDATA "./Data/studentData.dat"
    #endif
    #ifndef EVENTDATA
        #define EVENTDATA "./Data/eventData.dat"
    #endif
    #ifndef EVENTINDEX
        #define EVENTINDEX "./Data/eventIndex.dat"
    #endif
#endif
