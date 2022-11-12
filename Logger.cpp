#include "Logger.h"
#include "settings.h"

Logger* Logger::instance = 0;

Logger* Logger::getInstance(NTPTime* timeptr) {
  if (instance == 0) {
    instance = new Logger(timeptr);
  }
  return instance;
}

Logger::Logger(NTPTime* timeptr) :
  logFileName(LOG_FILE_NAME)
{
  initializeStorage();
  setTimeObject(timeptr);
}

void Logger::initializeStorage() {
  //open file in write mode
  File logFile = SPIFFS.open(logFileName, "a");
  logFileSize = logFile.size();
  logFile.close();
}

void Logger::setTimeObject(NTPTime* timeptr) {
  myTime = timeptr;
}

void Logger::writeLog(const char* log_level, const char* message) {
  if(settings::get_logging_enabled() == false) return;
  
  size_t bufferSize = 128;
  char buf[bufferSize + 1];
  sprintf(buf, "%s %s %s", myTime->getDateTimeStamp(), log_level, message);
  buf[bufferSize] = '\0';

  File logFile;

  if (logFileSize >= FILE_SIZE_LIMIT) {
    //open file in overrite mode
    logFile = SPIFFS.open(logFileName, "w");
  } else {
    //open file in append mode
    logFile = SPIFFS.open(logFileName, "a");
  }

  logFile.println(buf);
  logFileSize = logFile.size();
  logFile.close();
}

void Logger::logInfo(const char* message) {
  const char *log_level = "INFO";
  writeLog(log_level, message);
}

void Logger::logWarning(const char* message) {
  const char *log_level = "WARN";
  writeLog(log_level, message);
}

void Logger::logError(const char* message) {
  const char *log_level = "ERRR";
  writeLog(log_level, message);
}

size_t Logger::getLogFileSize() {
  return logFileSize;
}

const int Logger::getLogFileSizeLimit() {
  return FILE_SIZE_LIMIT;
}

void Logger::readLogFile(char* output) {
  File logFile = SPIFFS.open(logFileName, "r");
  
  int bytes_read;
  if (logFile) {   
    while (logFile.available()) {
      bytes_read = logFile.read((uint8_t *)output, logFile.size());      
    }
  }
  logFile.close();  
  output[bytes_read - 1] = '\0'; //null terminate the buffer
}

void Logger::clearLogFile() {   
  File logFile;  
  //open file in overrite mode
  logFile = SPIFFS.open(logFileName, "w"); 
  logFile.println();
  logFileSize = logFile.size();
  logFile.close();
}

void Logger::printDebug() {
  //open file in read mode
  File logFile = SPIFFS.open(logFileName, "r");

  if (logFile) {
    Serial.println("contents of file: " + String(logFileName));
    while (logFile.available()) {
      Serial.write(logFile.read());
    }
    logFile.close();
  } else {
    Serial.println("error opening the log file!");
  }
}
