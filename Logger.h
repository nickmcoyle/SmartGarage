#pragma once;

#include <FS.h>// this needs to be first, or it all crashes and burns...
#include <SPIFFS.h>
#include "NTPTime.h"
#define FORMAT_SPIFFS_IF_FAILED true

#define LOG_FILE_NAME "/log.txt"
#define FILE_SIZE_LIMIT 10*1024 //clear the log when it reaches this size

class Logger {
  private:
    static Logger* instance;
    Logger(NTPTime* timeptr); //private constructor to prevent instancing
    void setTimeObject(NTPTime* timeptr);
    void initializeStorage();
    void writeLog(const char* log_level, const char* message);
    NTPTime* myTime;
    const char* logFileName;
    size_t logFileSize;

  public:
    static Logger* getInstance(NTPTime* timeptr);    
    void logInfo(const char* message);
    void logWarning(const char* message);
    void logError(const char* message);
    size_t getLogFileSize();
    const int getLogFileSizeLimit();
    void readLogFile(char* output);
    void clearLogFile();
    void printDebug();   
};
