#pragma once;

#include "Arduino.h"
#include <time.h>

class NTPTime
{
  public:
    NTPTime(const char* myServerUrl, int myOffset, bool enableDST);
    NTPTime(const char* myServerUrl, int myOffset);
    NTPTime(const char* myServerUrl);
    void beginClient();
    void updateTime();
    unsigned long getRawTime();
    const char* getTimeFormatted();
    const char* getAmPm();    
    const char* getTimeStamp();
    const char* getDateStamp();
    const char* getDateTimeStamp();

  private:
    int offset;
    bool isDST; //flag is daylight savings in effect for the current datetime
    bool enableDSTChanges; //flag for if the client cares about checking DST changes or not
    const char* AmPm;
    const char* errorMsg;
    time_t rawTime;
    unsigned long epochTime;
    const char* ntpServerName;

    unsigned long sendNTPpacket(IPAddress& address);
    void getTimeFromTimeServer();
    int getMinute(unsigned long timeLong);
    int getSecond(unsigned long timeLong);
    int getHour(unsigned long timeLong);
    int getDay(unsigned long timeLong);
    int getMonth(unsigned long timeLong);
    int getYear(unsigned long timeLong);
    void checkDST();
    boolean isDayLightSavingsTime();
    void toLeadingZero(const int input, char* output);
};
