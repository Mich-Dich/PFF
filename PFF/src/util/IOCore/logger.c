#include "logger.h"

#include <stdio.h>
#include <string.h>
#include <stdarg.h>
#include <stdlib.h>
#include <inttypes.h>
#include <stdbool.h>

// Windows version
#if defined(_WIN32) || defined(__CYGWIN__)

    #include <io.h>
    #include <Shlwapi.h>
    #define access _access
    #define VA_LIST va_list
    #define F_OK 0

    const char* CL_Get_Error_String(int errnum) {

        static char buffer[128];
        strerror_s(buffer, sizeof(buffer), errnum);

        return buffer;
    }

    char* getFileName(const char* filePath) {

        const char* lastSlash = strrchr(filePath, '\\');
        char* local = "";
        if (lastSlash != NULL)
            strcpy_s(local, 1, lastSlash + 1);
        else
            strcpy_s(local, 1, filePath);

        return local;
    }

    #define GetBaseName(name) getFileName(name)
// APPLE IS NOT SUPORTED
#else
    #include <dirent.h>
    #include <libgen.h>
    #include <sys/stat.h>
    #include <unistd.h>
    #include <time.h>
    #define VA_LIST __builtin_va_list
    #define GetBaseName(name) basename(name)
#endif


#define MAX_NAME_LENGTH 32
#define REGISTERED_THREAD_NAME_LEN_MAX 256
#define MAX(a, b) ((a) > (b) ? (a) : (b))
#define MIN(a, b) ((a) < (b) ? (a) : (b))

#define LOGGER_FORMAT_MESSAGE(format, ...)          sprintf_s(Format_Buffer, sizeof(Format_Buffer), format, ##__VA_ARGS__);     \
                                                    strcat_s(message_out, sizeof(message_out), Format_Buffer);

#define CL_INTERNAL_ERROR_MSG(ErrorMsg, ...) {      fprintf(stderr, ErrorMsg, ##__VA_ARGS__);                                   \
                                                    exit(-1); }

#define SETUP_FOR_FORMAT_MACRO                      char message_out[MAX_MESSAGE_SIZE];                                         \
                                                        memset(message_out, 0, sizeof(message_out));                            \
                                                    char message_formatted[MAX_MESSAGE_SIZE];                                   \
                                                        memset(message_formatted, 0, sizeof(message_formatted));                \
                                                    char Format_Command[2] = "0\0";                                             \
                                                    char Format_Buffer[MAX_MESSAGE_SIZE];                                       \



typedef struct message_plus_thread {
    char text[MAX_MESSAGE_SIZE];
    uintptr_t thread;
} message_plus_thread;

typedef struct MessageBuffer{
    message_plus_thread messages[MAX_BUFFERED_MESSAGES];
    int count;
} MessageBuffer;

typedef struct ThreadNameMap {
    uintptr_t thread_id;
    char name[REGISTERED_THREAD_NAME_LEN_MAX];
    struct ThreadNameMap* next;
    struct ThreadNameMap* prev;
} ThreadNameMap;

typedef struct SpecificLogLevelFormat{
    bool isInUse;
    char* Format;
} SpecificLogLevelFormat;

// ------------------------------------------------------------------------------------------ Static Var ------------------------------------------------------------------------------------------

// Windows version
#if defined(_WIN32) || defined(__CYGWIN__)
    CRITICAL_SECTION LogLock;
    #define CRITICAL_SELECTION_INITIALIZE InitializeCriticalSection(&LogLock)
    #define CRITICAL_SELECTION_LOCK EnterCriticalSection(&LogLock);
    #define CRITICAL_SELECTION_UNLOCK LeaveCriticalSection(&LogLock);
#else
    static pthread_mutex_t LogLock = PTHREAD_MUTEX_INITIALIZER;
    #define CRITICAL_SELECTION_INITIALIZE LogLock = PTHREAD_MUTEX_INITIALIZER;
    #define CRITICAL_SELECTION_LOCK pthread_mutex_lock(&LogLock);
    #define CRITICAL_SELECTION_UNLOCK pthread_mutex_unlock(&LogLock);
#endif
static const char* Console_Colour_Strings[LL_MAX_NUM] = {"\x1b[1;41m", "\x1b[1;31m", "\x1b[1;93m", "\x1b[1;32m", "\x1b[1;94m", "\x1b[0;37m"};
static const char* Console_Colour_Reset = "\x1b[0;39m";
static const char* level_str[LL_MAX_NUM] = {"FATAL", "ERROR", "WARN", "INFO", "DEBUG", "TRACE"};
static const char* separator     = "-------------------------------------------------------------------------------------------------------\n";
static const char* separator_Big = "=======================================================================================================\n";
static int Is_Initalised = CL_FALSE;
static FILE* logFile;
static enum log_level internal_level = Trace;
static int log_level_for_buffer = 0;
static MessageBuffer Log_Message_Buffer = { .count = 0 };
static ThreadNameMap* firstEntry = NULL;
static ThreadNameMap* lastEntry = NULL;
static bool Loc_Use_separate_Files_for_every_Thread = true;
static char MainLogFileName[MAX_NAME_LENGTH +1] = "unknown.txt";
static char MainLogDirectory[MAX_NAME_LENGTH +1] = "logs";
static char* m_GeneralLogFormat = "[Default] [$B$F: $G$E] - $B$C$E$Z";
static char* m_GeneralLogFormat_BACKUP = "[Default] [$B$F: $G$E] - $B$C$E$Z";
static SpecificLogLevelFormat SpecificLogFormatArray[] = { 
    {false, "[$B$L$X$E] [$B$F: $G$E] - $B$C$E$Z"}, 
    {false, "[$B$L$X$E] [$B$F: $G$E] - $B$C$E$Z"}, 
    {false, "[$B$L$X$E] [$B$F: $G$E] - $B$C$E$Z"}, 
    {false, "[$B$L$X$E] [$B$F: $G$E] - $B$C$E$Z"}, 
    {false, "[$B$L$X$E] [$B$F: $G$E] - $B$C$E$Z"}, 
    {false, "[$B$L$X$E] [$B$F: $G$E] - $B$C$E$Z"},
};


// ------------------------------------------------------------------------------------------ private functions ------------------------------------------------------------------------------------------

// local Functions
void output_Message(enum log_level level, const char* message, uintptr_t threadID);
void WriteMessagesToFile();
bool Create_Log_File(const char* FileName);
ThreadNameMap* add_Thread_Name_Mapping(uintptr_t thread, const char* name);
ThreadNameMap* f_find_Entry(uintptr_t threadID);
void remove_Entry(uintptr_t threadID);
int remove_all_Files_In_Directory(const char *dirName);
void getCurrentTime(struct CL_Time_Info *timeInfo);

// ------------------------------------------------------------------------------------------ Semi-inline functions ------------------------------------------------------------------------------------------
// Print a separator "---"
void print_Separator(uintptr_t threadID)        { output_Message(Trace, separator, threadID); }

// Print a separator "==="
void print_Separator_Big(uintptr_t threadID)    { output_Message(Trace, separator_Big, threadID); }


// ------------------------------------------------------------------------------------------ public functions ------------------------------------------------------------------------------------------

// Create or reset a Log-File: [LogFileName] and setup output format & stream
// - [LogFileName] default logfile name
// - [m_GeneralLogFormat] general format for LogLevels that are not specifically set
// - [threadID] uintptr_t of main thread
// - [Use_separate_Files_for_every_Thread] use CL_TRUE or CL_FALSE
int log_init(const char* LogFileName, const char* directoryName, char* GeneralLogFormat, uintptr_t threadID, int Use_separate_Files_for_every_Thread) {

    if (Is_Initalised == CL_TRUE) {
        LOG(Warn, "Logger is already initalized");
        return -1;
    }

    // Validate input vars
    if (strlen(LogFileName) > MAX_NAME_LENGTH)
        CL_INTERNAL_ERROR_MSG("field [LogFileName] in log_init() is to long. Length [%" PRIu64 "/%d]", strlen(LogFileName), MAX_NAME_LENGTH);

    if (strlen(directoryName) > MAX_NAME_LENGTH)
        CL_INTERNAL_ERROR_MSG("field [directoryName] in log_init() is to long. Length [%" PRIu64 "/%d]", strlen(directoryName), MAX_NAME_LENGTH);

    if (strlen(GeneralLogFormat) <= 1)
        CL_INTERNAL_ERROR_MSG("field [GeneralLogFormat] in log_init() is empty");

    if (threadID == 0)
        CL_INTERNAL_ERROR_MSG("field [threadID] in log_init() is zero, please use yout thread_ID by using [CL_THREAD_ID]");

    CRITICAL_SELECTION_INITIALIZE;

    strcpy_s(MainLogFileName, sizeof(MainLogFileName), LogFileName);
    strcpy_s(MainLogDirectory, sizeof(MainLogDirectory), directoryName);
    m_GeneralLogFormat = GeneralLogFormat;
    Loc_Use_separate_Files_for_every_Thread = (Use_separate_Files_for_every_Thread == CL_TRUE) ? true : false;


// Windows version
#if defined(_WIN32) || defined(__CYGWIN__)

    // create directory [MainLogDirectory]
    if (!CreateDirectoryA(MainLogDirectory, NULL)) {
        if (ERROR_ALREADY_EXISTS != GetLastError())
            CL_INTERNAL_ERROR_MSG("Failed to create directory: %s\n", MainLogDirectory);
    }

#else

    // create directory [MainLogDirectory]
    struct stat st = {0};
    if (stat(path, &st) == -1) {
        // Directory does not exist, create it
        if (mkdir(path, 0700) == -1) {
            CL_INTERNAL_ERROR_MSG("Failed to create directory: %s\n", path);
            return -1;
        }
    }

#endif

    char filename[REGISTERED_THREAD_NAME_LEN_MAX];
    snprintf(filename, sizeof(filename), "%s/%s.log",MainLogDirectory,  MainLogFileName);     
    Create_Log_File(filename);

    register_thread_log_under_Name(threadID, MainLogFileName);
    Is_Initalised = CL_TRUE;
    LOG(Trace, "Initialize")
    return 0;
}

bool Create_Log_File(const char* FileName) {
    
    // Open File
    errno_t err;
    if ((err = fopen_s(&logFile, FileName, "w")) != 0 || logFile == NULL)
        CL_INTERNAL_ERROR_MSG("Failed to create file: %s\n", FileName)
    
    // print title section to start of file
    else {

        struct CL_Time_Info locTimeInfo;
        getCurrentTime(&locTimeInfo);
        fprintf(logFile, "[%04d/%02d/%02d - %02d:%02d:%02d] Log initialized\n    Output-file: [%s]\n    Starting-format: %s\n",
            locTimeInfo.TI_year + 1900, locTimeInfo.TI_month + 1, locTimeInfo.TI_day, locTimeInfo.TI_hour, locTimeInfo.TI_minute, locTimeInfo.TI_sec, FileName, m_GeneralLogFormat);

        if (LOG_LEVEL_ENABLED <= 4 || LOG_LEVEL_ENABLED >= 0) {

            static const char* loc_level_str[6] = {"FATAL", " + ERROR", " + WARN", " + INFO", " + DEBUG", " + TRACE"};
            
            size_t LevelText_len = 1;
            for (int x = 0; x < LOG_LEVEL_ENABLED + 2; x++)
                LevelText_len += strlen(loc_level_str[x]);

            char* LogLevelText = malloc(LevelText_len);
            if (LogLevelText == NULL)
                CL_INTERNAL_ERROR_MSG("FAILED to allocate memory in [Create_Log_File()] to print enabled LogLevels")
            
            LogLevelText[0] = '\0';
            for (int x = 0; x < LOG_LEVEL_ENABLED + 2; x++)
                strcat(LogLevelText, loc_level_str[x]);
                            
            fprintf(logFile, "    LOG_LEVEL_ENABLED = %d    enabled log macros are: %s\n", LOG_LEVEL_ENABLED, LogLevelText);
            free(LogLevelText);
        }
        fprintf(logFile, "%s\n", separator_Big);
        fclose(logFile);
    }
    return true;
}

// write buffered messages to logFile and clean up output stream
void log_shutdown(){

    LOG(Trace, "Shutdown")
    WriteMessagesToFile();
}

// Output a message to the standard output stream and a log file
// !! CAUTION !! - do NOT make logs messages longer than MAX_MESSAGE_SIZE
void log_output(const enum log_level level, const char* prefix, const char* funcName, char* fileName, const int Line, const uintptr_t thread_id, const char* message, ...) {

    // check if message empty
    if (message[0] == '\0' && prefix[0] == '\0')
        return;

    struct CL_Time_Info locTimeInfo;
    getCurrentTime(&locTimeInfo);

    SETUP_FOR_FORMAT_MACRO

    // write all arguments in to [message_formatted]
    VA_LIST args_ptr;
    va_start(args_ptr, message);
        vsnprintf(message_formatted, MAX_MESSAGE_SIZE, message, args_ptr);
    va_end(args_ptr);

    // Loop over Format string and build Final Message

    //#define LOGGER_FORMAT_MESSAGE(format, ...)        sprintf(Format_Buffer, format, ##__VA_ARGS__);
    //                                                    strcat(message_out, Format_Buffer);

    char* locTargetFormat = m_GeneralLogFormat;
    if (SpecificLogFormatArray[level].isInUse)
        locTargetFormat = SpecificLogFormatArray[level].Format;

    int FormatLen = (int)strlen(locTargetFormat);
    for (int x = 0; x < FormatLen; x++) {

        if (locTargetFormat[x] == '$' && x + 1 < FormatLen) {

            Format_Command[0] = locTargetFormat[x + 1];
            switch (Format_Command[0]) {
            
            // ------------------------------------  Basic Info  -------------------------------------------------------------------------------
            // Color Start
            case 'B':
                LOGGER_FORMAT_MESSAGE("%s", Console_Colour_Strings[level])
            break;
            
            // Color End
            case 'E': 
                LOGGER_FORMAT_MESSAGE("%s", Console_Colour_Reset)
            break;
            
            // input text (message)
            case 'C':
                LOGGER_FORMAT_MESSAGE("%s%s", prefix, message_formatted)
            break;

            // Log Level
            case 'L':
                LOGGER_FORMAT_MESSAGE("%s", level_str[level])
            break;

            case 'Z':
                LOGGER_FORMAT_MESSAGE("\n")
            break;

            // Log Level
            case 'X':
                if (level == Info || level == Warn)       { LOGGER_FORMAT_MESSAGE(" ") }                    
            break;
            
            // Function Name
            case 'F':
                LOGGER_FORMAT_MESSAGE("%s", funcName)
            break;
            
            // File Name
            case 'A':
                LOGGER_FORMAT_MESSAGE("%s", fileName)
            break;
            
            // File Name
            case 'P':
                LOGGER_FORMAT_MESSAGE("%x", (uint32_t)thread_id)
            break;
            
            // Shortened File Name
            case 'I':
                LOGGER_FORMAT_MESSAGE("%s", GetBaseName(fileName))
            break;
            
            // Line
            case 'G':
                LOGGER_FORMAT_MESSAGE("%d", Line)
            break;

            // ------------------------------------  Time  -------------------------------------------------------------------------------
            // Clock hh:mm:ss
            case 'T':
                LOGGER_FORMAT_MESSAGE("%02d:%02d:%02d", locTimeInfo.TI_hour, locTimeInfo.TI_minute, locTimeInfo.TI_sec)
            break;

            // Clock ss
            case 'H':
                LOGGER_FORMAT_MESSAGE("%02d", locTimeInfo.TI_hour)
            break;
            
            // Clock mm
            case 'M':
                LOGGER_FORMAT_MESSAGE("%02d", locTimeInfo.TI_minute)
            break;

            // Clock ss
            case 'S':
                LOGGER_FORMAT_MESSAGE("%02d", locTimeInfo.TI_sec)
            break;

            // Clock ss
            case 'J': 
                LOGGER_FORMAT_MESSAGE("%02d", locTimeInfo.TI_nSec)
            break;

            // ------------------------------------  Date  -------------------------------------------------------------------------------
            // Data yyyy/mm/dd
            case 'N':
                LOGGER_FORMAT_MESSAGE("%04d/%02d/%02d", locTimeInfo.TI_year, locTimeInfo.TI_month, locTimeInfo.TI_day)
            break;
            
            // Year
            case 'Y':
                LOGGER_FORMAT_MESSAGE("%04d", locTimeInfo.TI_year + 1900)
            break;
            
            // Month
            case 'O':
                LOGGER_FORMAT_MESSAGE("%02d", locTimeInfo.TI_month + 1)
            break;

            // Day
            case 'D':
                LOGGER_FORMAT_MESSAGE("%02d", locTimeInfo.TI_day)
            break;


            // ------------------------------------  Default  -------------------------------------------------------------------------------
            default:
                break;
            }

            x++;
        }

        else {

            strncat_s(message_out, sizeof(message_out), & locTargetFormat[x], 1);
        }
    }
    
    if (Is_Initalised == CL_TRUE)
        output_Message(level, (const char*)message_out, thread_id);
    else
        printf("%s LOGGER IS NOT INITALIZED: %s %s", Console_Colour_Strings[0], Console_Colour_Reset, message_out);
}

//
void output_Message(enum log_level level, const char* message, uintptr_t threadID) {
    
    // Print Message to standard output
    if (level <= internal_level) {

        printf("%s", message);
        fflush(stdout);
    }

    CRITICAL_SELECTION_LOCK
    // Save message in Buffer
    strncpy_s(Log_Message_Buffer.messages[Log_Message_Buffer.count].text,sizeof(Log_Message_Buffer.messages[Log_Message_Buffer.count].text), message, sizeof(Log_Message_Buffer.messages[0].text));
    Log_Message_Buffer.messages[Log_Message_Buffer.count].text[sizeof(Log_Message_Buffer.messages[Log_Message_Buffer.count].text) - 1] = '\0';
    Log_Message_Buffer.messages[Log_Message_Buffer.count].thread = threadID;
    Log_Message_Buffer.count++;

    // Check if buffer full OR important message
    if (Log_Message_Buffer.count >= (MAX_BUFFERED_MESSAGES -1) || (unsigned int)level < (6 - (unsigned int)log_level_for_buffer)) {
        
        WriteMessagesToFile();
        Log_Message_Buffer.count = 0;
    }
    
    CRITICAL_SELECTION_UNLOCK 
}

//
void WriteMessagesToFile() {

    ThreadNameMap* loc_Entry = NULL;
    char filename[REGISTERED_THREAD_NAME_LEN_MAX];
    for (int x = 0; x < Log_Message_Buffer.count; x++) {

        if(Loc_Use_separate_Files_for_every_Thread) {

            loc_Entry = f_find_Entry(Log_Message_Buffer.messages[x].thread);
            if(loc_Entry != NULL) {

                snprintf(filename, sizeof(filename), "%s", loc_Entry->name);
            }
            else {

                //printf("    loc_Entry: %s [tread: %lu]\n", ptr_To_String(loc_Entry), Log_Message_Buffer.messages[x].thread);
                snprintf(filename, sizeof(filename), "%s\\thread_log_%lu.log", MainLogDirectory, (unsigned long)Log_Message_Buffer.messages[x].thread);
                if(access(filename, F_OK) != 0) 
                    Create_Log_File(filename);
            }
        }

        else {

            snprintf(filename, sizeof(filename), "%s\\%s.log",MainLogDirectory,  MainLogFileName);
            if(access(filename, F_OK) != 0) {

                printf("  Could not poen [%s] => calling Create_Log_File()\n", filename);
                Create_Log_File(filename);
            }
        }
        
        // Open the file for writing (append mode)
        FILE* file = fopen(filename, "a");
        if (file == NULL) {

            printf("  ERROR: [%s] ", filename);
            perror("Error opening the file");
            continue;
        }
        
        fputs((const char*)Log_Message_Buffer.messages[x].text, file); 
        fclose(file);
    }
}

// change the 
int register_thread_log_under_Name(uintptr_t threadID, const char* name) {

    char filename[REGISTERED_THREAD_NAME_LEN_MAX];
    snprintf(filename, sizeof(filename), "%sthread_log_%lu.log", MainLogDirectory, (unsigned long)threadID);

    char newFilename[REGISTERED_THREAD_NAME_LEN_MAX];
    snprintf(newFilename, sizeof(newFilename), "%s%s.log", MainLogDirectory, name);

    // Create a new entry in list
    add_Thread_Name_Mapping(threadID, newFilename);

    if (access(filename, F_OK) != 0)
        return -1;

    int result = rename(filename, newFilename);
    if (result != 0) 
        return -1;

    return 0;
}

// ------------------------------------------------------------------------------------------ Thread-Name Mapping ------------------------------------------------------------------------------------------

//
ThreadNameMap* add_Thread_Name_Mapping(uintptr_t thread, const char* name) {

    ThreadNameMap* loc_Found = f_find_Entry(thread);
    if(loc_Found != NULL) {

        //printf("  thread already has an Entry in [ThreadNameMap], [name] was updated");
        strncpy_s(loc_Found->name, sizeof(loc_Found->name), name, MIN(strlen(name), REGISTERED_THREAD_NAME_LEN_MAX));
        return loc_Found;
    }

    ThreadNameMap* newEntry = (ThreadNameMap*) malloc(sizeof(ThreadNameMap));
    if (newEntry == NULL) {
        
        printf("  Memory allocation failed\n");
        return NULL;
    }

    memset(newEntry, 0, sizeof(ThreadNameMap));
    strncpy_s(newEntry->name, sizeof(newEntry->name), name, MIN(strlen(name), REGISTERED_THREAD_NAME_LEN_MAX));
    newEntry->thread_id = thread;
    
    if (firstEntry == NULL) {   // list is Empty

        firstEntry = newEntry;
        lastEntry = newEntry;
    
    } else {                      // Add to List-End

        lastEntry->next = newEntry;
        newEntry->prev = lastEntry;
        lastEntry = newEntry;
    }
    
    return newEntry;
}

// removes entry from linked list, if found
void remove_Entry(uintptr_t threadID) {

    CRITICAL_SELECTION_LOCK
    ThreadNameMap *locPointer = f_find_Entry(threadID);
    if (locPointer == NULL)
        return;
    
    if (firstEntry == lastEntry) {          // List has only one element

        firstEntry = NULL;
        lastEntry = NULL;
    }

    // List has more than one element
    else {

        if (locPointer == firstEntry){      // Is First?

            firstEntry = firstEntry->next;
            firstEntry->prev = NULL;
        
        } else if (locPointer == lastEntry) { // Is Last?

            lastEntry = lastEntry->prev;
            lastEntry->next = NULL;
        
        } else {                              // is in middle

            locPointer->prev->next = locPointer->next;
            locPointer->next->prev = locPointer->prev;
        }
    }

    free(locPointer);
    CRITICAL_SELECTION_UNLOCK
}

// Iterate through list and try to find entry by thread // Returns NULL if not found
ThreadNameMap* f_find_Entry(uintptr_t threadID) {
    
    //printf("    f_find_Entry()  [thread: %lu]\n", threadID);
    bool locFound = false;
    ThreadNameMap *locPointer = firstEntry;
    while (locFound == false && locPointer != NULL) {

        //printf("    Iterating over Entry List current Entry.thread: %lu] [next: %s]\n", locPointer->thread_id, ptr_To_String(locPointer->next));

        if (locPointer->thread_id == threadID)
            locFound = true;
        else
            locPointer = locPointer->next;
    }

    return locFound ? locPointer : NULL;
}

// ------------------------------------------------------------------------------------------ Formatting ------------------------------------------------------------------------------------------

// Change Format of log messages and backup previous Format
void set_Formatting(char* LogFormat) {

    m_GeneralLogFormat_BACKUP = m_GeneralLogFormat;
    m_GeneralLogFormat = LogFormat;
}

// Sets the Backup version of Format to be used as Main Format
void use_Formatting_Backup() {

    m_GeneralLogFormat = m_GeneralLogFormat_BACKUP;
}

//
void Set_Format_For_Specific_Log_Level(enum log_level level, char* Format) {

    SpecificLogFormatArray[level].isInUse = true;
    SpecificLogFormatArray[level].Format = Format;
}

//
void Disable_Format_For_Specific_Log_Level(enum log_level level) {

    SpecificLogFormatArray[level].isInUse = false;
}

// ------------------------------------------------------------------------------------------ Misc ------------------------------------------------------------------------------------------

//
void set_buffer_Level(int newLevel) {

    if( newLevel <= 4 && newLevel >= 0)
        log_level_for_buffer = newLevel;

    else 
        LOG(Error, "Input invalid Level (0 <= newLevel <= 4), input: %d", newLevel)
}

// Set what leg level should be printed to terminal
// CAUTION! this only applies to log levels that are enabled be LOG_LEVEL_ENABLED
void set_log_level(enum log_level new_level) {

    CL_VALIDATE(new_level < LL_MAX_NUM && new_level > Fatal, return, "", "Selected log level is out of bounds (1 <= [new_level: %d] <= 5)", new_level)

    LOG(Trace, "Setting [log_level: %s]", level_str[new_level])
    internal_level = new_level;
}

//
int remove_all_Files_In_Directory(const char *dirName) {

    // Windows version
    #if defined(_WIN32) || defined(__CYGWIN__)

        // Check if the directory exists, and create it if not
        if (GetFileAttributes((LPCWSTR)dirName) == INVALID_FILE_ATTRIBUTES) {
            if (CreateDirectory((LPCWSTR)dirName, NULL) == 0) {
                perror("Error creating directory");
                return -1;
            }
        }

        WIN32_FIND_DATA findFileData;
        HANDLE hFind = FindFirstFile((LPCWSTR)dirName, &findFileData);

        if (hFind == INVALID_HANDLE_VALUE) {
            perror("Error opening directory");
            return -1;
        }

        do {
            if (findFileData.dwFileAttributes & FILE_ATTRIBUTE_DIRECTORY) {
                // Ignore directories
                continue;
            }

            // It's a regular file, remove it
            char filepath[MAX_PATH];
            snprintf(filepath, sizeof(filepath), "%s\\%ws", dirName, findFileData.cFileName);

            if (remove(filepath) != 0) {
                perror("Error removing file");
                FindClose(hFind);
                return -1;
            }
        } while (FindNextFile(hFind, &findFileData) != 0);

        FindClose(hFind);
        return 0;

    #else
        DIR *dir = opendir(dirName);
        if (dir == NULL) {
            perror("Error opening directory");
            return -1;
        }

        struct dirent* entry;
        while ((entry = readdir(dir)) != NULL) {

            if (entry->d_type == DT_REG) {
            
                // It's a regular file, remove it
                char filepath[REGISTERED_THREAD_NAME_LEN_MAX *2];
                snprintf(filepath, sizeof(filepath), "%s/%s", dirName, entry->d_name);

                if (remove(filepath) != 0) {
                    perror("Error removing file");
                    closedir(dir);
                    return -1;
                }
            }
        }

        closedir(dir);
        return 0;
    #endif

}

// ------------------------------------------------------------------------------------------ Measure Time ------------------------------------------------------------------------------------------

//
void getCurrentTime(struct CL_Time_Info *timeInfo) {
    // Windows version
#if defined(_WIN32) || defined(__CYGWIN__)
    SYSTEMTIME systemTime;
    GetSystemTime(&systemTime);

    timeInfo->TI_year = systemTime.wYear;
    timeInfo->TI_month = systemTime.wMonth;
    timeInfo->TI_day = systemTime.wDay;
    timeInfo->TI_hour = systemTime.wHour;
    timeInfo->TI_minute = systemTime.wMinute;
    timeInfo->TI_sec = systemTime.wSecond;
    timeInfo->TI_nSec = systemTime.wMilliseconds * 1000000; // Convert milliseconds to nanoseconds
#else
    struct timeval currentTime;
    gettimeofday(&currentTime, NULL);

    struct tm *localTime = localtime(&currentTime.tv_sec);

    timeInfo->TI_year = localTime->tm_year + 1900;  // Years since 1900
    timeInfo->TI_month = localTime->tm_mon + 1;     // Months are 0-11, so add 1
    timeInfo->TI_day = localTime->tm_mday;
    timeInfo->TI_hour = localTime->tm_hour;
    timeInfo->TI_minute = localTime->tm_min;
    timeInfo->TI_sec = localTime->tm_sec;
    timeInfo->TI_nSec = currentTime.tv_usec * 1000; // Convert microseconds to nanoseconds
#endif
}



// remembers the exact time at witch this function was called
// NOT FINISHED
void Calc_Func_Duration_Start(struct CL_Time_Info* StartTime) {

    struct CL_Time_Info locTimeInfo;
    getCurrentTime(&locTimeInfo);   

    LOG(Trace, "Starting Tine measurement")
}

// Calculates the time difference between calling [Calc_Func_Duration_Start] and this function
// NOT FINISHED
void Calc_Func_Duration(const struct CL_Time_Info* StartTime) {

    struct CL_Time_Info endTime;
    getCurrentTime(&endTime);

    int elapsedYears = endTime.TI_year - StartTime->TI_year;
    int elapsedMonths = endTime.TI_month - StartTime->TI_month;
    int elapsedDays = endTime.TI_day - StartTime->TI_day;
    int elapsedHours = endTime.TI_hour - StartTime->TI_hour;
    int elapsedMinutes = endTime.TI_minute - StartTime->TI_minute;
    int elapsedSeconds = endTime.TI_sec - StartTime->TI_sec;
    int elapsedNanoSeconds = endTime.TI_nSec - StartTime->TI_nSec;

    // Adjust nanoseconds
    if (elapsedNanoSeconds < 0) {
        elapsedSeconds--;
        elapsedNanoSeconds += 1000000000;
    }
    
    SETUP_FOR_FORMAT_MACRO
    
    if (elapsedYears < 0)
        LOGGER_FORMAT_MESSAGE("%04d:", elapsedYears)
    if (elapsedMonths < 0)
        LOGGER_FORMAT_MESSAGE("%02d:", elapsedMonths)
    if (elapsedDays < 0)
        LOGGER_FORMAT_MESSAGE("%02d:", elapsedDays)
    if (elapsedHours < 0)
        LOGGER_FORMAT_MESSAGE("%02d:", elapsedHours)
    if (elapsedMinutes < 0)
        LOGGER_FORMAT_MESSAGE("%02d:", elapsedMinutes)
    if (elapsedSeconds < 0)
        LOGGER_FORMAT_MESSAGE("%02d:", elapsedSeconds)
    if (elapsedNanoSeconds < 0)
        LOGGER_FORMAT_MESSAGE("%09d:", elapsedNanoSeconds)

    LOG(Trace, "Ending %s", message_out)
}

int IsInitalized(void) {
    return Is_Initalised;
}


