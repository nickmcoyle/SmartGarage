#include "NTPTime.h"
#include <WiFiUdp.h>
#include <WiFi.h>

WiFiUDP udp; // A UDP instance to let us send and receive packets over UDP
unsigned int localPort = 2390; //local port to listen for UDP packets
const int NTP_PACKET_SIZE = 48; //NTP time stamp is in the first 48 bytes of the message
byte packetBuffer[NTP_PACKET_SIZE]; //buffer to hold incoming and outgoing packets
IPAddress timeServerIP; //Don't hardwire the IP address or we won't get the benefits of the pool. Lookup the IP address for the host name instead
unsigned long lastWaitTime = millis();

NTPTime::NTPTime(const char* myServerUrl, int myOffset, bool enableDST) :
  ntpServerName(myServerUrl),
  offset(myOffset),
  enableDSTChanges(enableDST),
  isDST(false),
  AmPm("")
{
}

NTPTime::NTPTime(const char* myServerUrl, int myOffset) :
  NTPTime(myServerUrl, myOffset, false)
{
}

NTPTime::NTPTime(const char* myServerUrl) :
  NTPTime(myServerUrl, 0, false)
{
}

void NTPTime::beginClient() {
  //get a random server from the pool
  //if your wifi config uses static ip, you have to have DNS server, gateway and subnet all correct for this to work
  WiFi.hostByName(ntpServerName, timeServerIP);
  //Serial.println(timeServerIP);
  updateTime();
}

// send an NTP request to the time server at the given address
unsigned long NTPTime::sendNTPpacket(IPAddress& address)
{
  //Serial.println("sending NTP packet...");
  // set all bytes in the buffer to 0
  memset(packetBuffer, 0, NTP_PACKET_SIZE);
  // Initialize values needed to form NTP request
  // (see URL above for details on the packets)
  packetBuffer[0] = 0b11100011;   // LI, Version, Mode
  packetBuffer[1] = 0;     // Stratum, or type of clock
  packetBuffer[2] = 6;     // Polling Interval
  packetBuffer[3] = 0xEC;  // Peer Clock Precision
  // 8 bytes of zero for Root Delay & Root Dispersion
  packetBuffer[12]  = 49;
  packetBuffer[13]  = 0x4E;
  packetBuffer[14]  = 49;
  packetBuffer[15]  = 52;

  // all NTP fields have been given values, now
  // you can send a packet requesting a timestamp:
  udp.beginPacket(address, 123); //NTP requests are to port 123
  udp.write(packetBuffer, NTP_PACKET_SIZE);
  udp.endPacket();
}

void NTPTime::getTimeFromTimeServer() {
  sendNTPpacket(timeServerIP); // send an NTP packet to a time server
  // wait 1 second to see if a reply is available
  if ((millis() - lastWaitTime) >= 1000) {
    int cb = udp.parsePacket();
    if (!cb) {
      //Serial.println("unable to get response packet from NTP server");
    }
    else {
      //Serial.print("packet received, length=");
      //Serial.println(cb);
      // We've received a packet, read the data from it
      udp.read(packetBuffer, NTP_PACKET_SIZE); // read the packet into the buffer

      //the timestamp starts at byte 40 of the received packet and is four bytes,
      // or two words, long. First, extract the two words:

      unsigned long highWord = word(packetBuffer[40], packetBuffer[41]);
      unsigned long lowWord = word(packetBuffer[42], packetBuffer[43]);
      // combine the four bytes (two words) into a long integer
      // this is NTP time (seconds since Jan 1 1900):
      unsigned long secsSince1900 = highWord << 16 | lowWord;

      const unsigned long seventyYears = 2208988800UL;
      // subtract seventy years:
      epochTime = secsSince1900 - seventyYears;

      rawTime = epochTime + offset;
    }
    lastWaitTime = millis();
  }
}

int NTPTime::getSecond(unsigned long timeLong) {
  return (timeLong % 60);
}

int NTPTime::getMinute(unsigned long timeLong) {
  return ((timeLong  % 3600) / 60); //the minute (3600 equals secs per minute)
}

int NTPTime::getHour(unsigned long timeLong) {
  return ((timeLong  % 86400L) / 3600); //the hour (86400 equals secs per day)
}

int NTPTime::getDay(unsigned long timeLong) {
  struct tm* tm = localtime(&rawTime);
  return tm->tm_mday;
}

int NTPTime::getMonth(unsigned long timeLong) {
  struct tm* tm = localtime(&rawTime);
  return tm->tm_mon + 1; //months since january
}

int NTPTime::getYear(unsigned long timeLong) {
  struct tm* tm = localtime(&rawTime);
  return tm->tm_year + 1900; //years since 1900
}

//checks current date and time if DST is in effect and sets a flag to true if it and false if it isn't
void NTPTime::checkDST() {
  if (isDayLightSavingsTime()) {
    isDST = true;
  } else {
    isDST = false;
  }
}

void NTPTime::updateTime() {
  getTimeFromTimeServer();
}

unsigned long NTPTime::getRawTime() {
  return rawTime;
}

const char* NTPTime::getTimeFormatted() {
  static char buf[20];
  int hours = getHour(rawTime);
  int minutes = getMinute(rawTime);

  //if daylight savings checking is enabled and it is currently daylight savings time,
  //just add 1 hour, otherwise the standard offset is already correct
  if (enableDSTChanges) {
    if (isDayLightSavingsTime()) hours += 1;
  }

  //this adds a leading 0 to any minutes that are between 0 and 9 so they look good
  char minutesStr[3];
  toLeadingZero(minutes, minutesStr);

  //set the AmPm value for the current time
  if (hours < 12 || hours == 24) {
    AmPm = "AM";
  } else {
    AmPm = "PM";
  }

  //change to 12 hour format
  if (hours == 0) {
    hours = 12;
  }
  if (hours > 12) {
    hours -= 12;
  }

  sprintf(buf, "%d:%s", hours, minutesStr);
  return buf;
}

const char* NTPTime::getTimeStamp() {
  int hours = getHour(rawTime);
  int minutes = getMinute(rawTime);
  int seconds = getSecond(rawTime);

  //if daylight savings checking is enabled and it is currently daylight savings time,
  //just add 1 hour, otherwise the standard offset is already correct
  if (enableDSTChanges &&  isDayLightSavingsTime()) hours += 1;

  //this adds a leading 0 to any minutes that are between 0 and 9 so they look good
  char hoursStr[3];
  toLeadingZero(hours, hoursStr);

  char minutesStr[3];
  toLeadingZero(minutes, minutesStr);

  char secondsStr[3];
  toLeadingZero(seconds, secondsStr);

  static char buf[16];
  sprintf(buf, "%s:%s:%s", hoursStr, minutesStr, secondsStr);
  return buf;
}

const char* NTPTime::getAmPm() {
  return AmPm;
}

const char* NTPTime::getDateStamp() {
  int rawMonth = getMonth(rawTime);
  int rawDay = getDay(rawTime);

  char monthStr[3];
  toLeadingZero(rawMonth, monthStr);

  char dayStr[3];
  toLeadingZero(rawDay, dayStr);

  static char buf[16];

  sprintf(buf, "%s/%s/%d", monthStr, dayStr, getYear(rawTime));
  return buf;
}

//adds a 0 to the front of any number less than 10 so that it takes two decimal places
void NTPTime::toLeadingZero(const int input, char* output) {
  if (input < 10) {
    sprintf(output, "0%d", input);
  } else {
    itoa(input, output, 10);
  }
}

const char* NTPTime::getDateTimeStamp() {
  static char buf[20];
  sprintf(buf, "%s %s", getDateStamp(), getTimeStamp());
  return buf;
}

boolean NTPTime::isDayLightSavingsTime()
{
  //Receives unix epoch time and returns seconds of offset for local DST
  //Valid thru 2099 for US only, Calculations from "http://www.webexhibits.org/daylightsaving/i.html"
  //Code idea from jm_wsb @ "http://forum.arduino.cc/index.php/topic,40286.0.html"
  //Get epoch times @ "http://www.epochconverter.com/" for testing
  //DST update wont be reflected until the next time sync
  bool result = false;
  int theYear = getYear(rawTime);
  int theMonth = getMonth(rawTime);
  int theDay = getDay(rawTime);
  int theHour = getHour(rawTime);
  //Serial.println("The day: " + String(theDay) + " The month: " + String(theMonth) + " The Year: " + String(theYear));
  int beginDSTDay = (14 - (1 + theYear * 5 / 4) % 7);
  int beginDSTMonth = 3;
  int endDSTDay = (7 - (1 + theYear * 5 / 4) % 7);
  int endDSTMonth = 11;
  if (((theMonth > beginDSTMonth) && (theMonth < endDSTMonth)) ||
      ((theMonth == beginDSTMonth) && (theDay > beginDSTDay)) ||
      ((theMonth == beginDSTMonth) && (theDay == beginDSTDay) && (theHour >= 2)) ||
      ((theMonth == endDSTMonth) && (theDay < endDSTDay)) ||
      ((theMonth == endDSTMonth) && (theDay == endDSTDay) && (theHour < 1))) {
    result = true;  //Add back in one hours worth of seconds - DST in effect
  }
  isDST = result;
  return result;
}
