//TODO LIST
/*
   add functionality to work with google home
   improve button press vs hold functionality so the press doesnt fire if you are holding
   add story or rock paper scissors game and button
   add wifi login history and ability to connect to previous wifis when preferred one isnt available
*/

#include <FS.h>// this needs to be first, or it all crashes and burns...
#include <WiFi.h>
#include "WiFiManager.h"
#include <ESPmDNS.h>
#include <WiFiUdp.h>
#include "app_httpd.h"
#include <SPIFFS.h>
#include <ArduinoOTA.h>
#include "PID.h"
#include "accessory_pins.h"
#include "Display.h"
#include "smart_outlet.h"
#include "temperature_sensor.h"
#include "NTPTime.h"
#include "Logger.h"
#include "animations.h"
#include "Button.h"
#include "eeprom_helpers.h"
#include "settings.h"
#include "ESP8266FtpServer.h"
FtpServer ftpSrv;

#define PID_UPDATE_INTERVAL 200 //milliseconds
#define DISPLAY_UPDATE_INTERVAL 500
#define NTPTIME_UPDATE_INTERVAL 10000
#define CHECKSAFETY_INTERVAL 60000 // 1 minute

float heatPower; //0 - 1000  milliseconds on per second
static unsigned long lastPIDTime;  //most recent PID update time in ms
static unsigned long lastTempReadingTime = 0;
static unsigned long lastRefreshDisplayTime = 0;
static unsigned long lastRefreshNTPTime = 0;
unsigned long currentMillis = 0;
unsigned long lastcheckSafetyTime = 0;

WiFiManager wm;
static int handleWiFiDropOutDelay = 60 * 1000;
unsigned long lastHandleWiFiDropOutTimeStamp = 0;
int handleWiFiDropOutReconnectAttempts = 1;
NTPTime myTime("0.us.pool.ntp.org", -8 * 3600, true);
Logger* logger;

int stopWatchStoppedSeconds = 0;
int stopWatchSeconds = 0;
int previousStopWatchSeconds = 0;
unsigned long stopWatchStopMillis = 0;
int stopWatchState = LOW;
unsigned long stopWatchStartMillis = 0;
unsigned long stopWatchElapsedMillis = 0;

// define the buttons
Button btn1(BUTTON1_PIN);
Button stopWatchBtn(STOPWATCH_BUTTON_PIN);
Button powerBtn(POWER_BUTTON_PIN);
Button rebootBtn(REBOOT_BUTTON_PIN);
unsigned long lastRebootMillis = 0;

#define IS_SILVIA false
int current_mains_power_state = LOW;
int previous_mains_power_state = LOW;
unsigned long LAST_MAINS_UPDATE_TIME;
unsigned long MAINS_POWER_UPDATE_INTERVAL = 500;

void setup() {
  Serial.begin(115200);
  Serial.setDebugOutput(true);
  Serial.println();
  SPIFFS.begin();
  logger = Logger::getInstance(&myTime);
  eeprom::initEEPROM();
  settings::init_settings();
  initTempSensor();
  initDisplay();

  //PID STUFF
  initPID();
  lastPIDTime = millis();
  initHeater();
  initSmartOutlet();

  //if esp32 cam this turns off the onboard flash light
  pinMode(4, OUTPUT);
  digitalWrite(4, LOW);

  if (!settings::get_use_without_wifi()) {
    initWifi();
    start_webserver();
    Serial.print("Web server Ready! Use 'http://");
    Serial.print(WiFi.localIP());
    Serial.println("' to connect");

    ArduinoOTA.onStart([]() {
      String type;
      if (ArduinoOTA.getCommand() == U_FLASH)
        type = "sketch";
      else
        type = "filesystem";
      Serial.println("Start updating " + type);
    });
    ArduinoOTA.onEnd([]() {
      Serial.println("\nEnd");
    });
    ArduinoOTA.onProgress([](unsigned int progress, unsigned int total) {
      Serial.printf("Progress: %u%%\r", (progress / (total / 100)));
    });
    ArduinoOTA.onError([](ota_error_t error) {
      Serial.printf("Error[%u]: ", error);
      if (error == OTA_AUTH_ERROR)         Serial.println("Auth Failed");
      else if (error == OTA_BEGIN_ERROR)   Serial.println("Begin Failed");
      else if (error == OTA_CONNECT_ERROR) Serial.println("Connect Failed");
      else if (error == OTA_RECEIVE_ERROR) Serial.println("Receive Failed");
      else if (error == OTA_END_ERROR)     Serial.println("End Failed");
    });
    ArduinoOTA.begin();

    //NTP time
    myTime.beginClient();

    ftpSrv.begin("esp8266", "esp8266"); // username, password for ftp. Set ports in ESP8266FtpServer.h (default 21, 50009 for PASV)    
  }  
}

void loop() {
  currentMillis = millis();

  if (!settings::get_use_without_wifi()) {
    ftpSrv.handleFTP();
    ArduinoOTA.handle();
    handleWiFiDropOut();
    //refresh the time
    if (currentMillis - lastRefreshNTPTime >= NTPTIME_UPDATE_INTERVAL)
    {
      lastRefreshNTPTime = currentMillis;
      myTime.updateTime();
    }
  }

  updateTempSensor();

  if ((currentMillis - lastcheckSafetyTime) >= CHECKSAFETY_INTERVAL) { 
    lastcheckSafetyTime = currentMillis;
    checkSafety();  
  }
  
  // every second, udpate the current heat control, and print out current status
  // This checks for rollover with millis()
  if (currentMillis < lastPIDTime) {
    lastPIDTime = 0;
  }
  if ((currentMillis - lastPIDTime) >= PID_UPDATE_INTERVAL) {    
    lastPIDTime +=  PID_UPDATE_INTERVAL;
    heatPower = updatePID(getTargetTemp(), getFreshTemp());
    setHeatPowerPercentage(heatPower);
    if (settings::get_serial_graphing_enabled()) printStatusForGraph(); //using Serial, prints the P I D values to a graph in the Processing application
  }

  //only activate heater if power is on
  if (isPowerOn()) {
    updateHeater();
  } else {
    _turnHeatElementOnOff(0);
  }

  
  /*
  current_mains_power_state = digitalRead(MAINS_POWER_DETECTOR_PIN);

  if (IS_SILVIA) {
    if (current_mains_power_state != previous_mains_power_state) {
      if ((currentMillis - LAST_MAINS_UPDATE_TIME) >= MAINS_POWER_UPDATE_INTERVAL) {
        previous_mains_power_state = current_mains_power_state;
        LAST_MAINS_UPDATE_TIME = currentMillis;
        //Serial.println(current_mains_power_state);
        const char* buf;
        if (!isPowerOn() && current_mains_power_state == LOW) {
          togglePower(); //power switch got flipped on and power isnt already on, so toggle it on
          buf = "Power toggled on by mains switch";
          logger->logInfo(buf);
        }
        if (isPowerOn() && current_mains_power_state == HIGH) {
          togglePower(); //power switch flipped off and power is on so toggle it off
          buf = "Power toggled off by mains switch";
          logger->logInfo(buf);
        }
      }
    }
  }
  */

  /* Gaggia
  if (current_mains_power_state != previous_mains_power_state) {        
    if ((currentMillis - LAST_MAINS_UPDATE_TIME) >= MAINS_POWER_UPDATE_INTERVAL) {
      Serial.println("current_mains_power_state" + String(current_mains_power_state));      
      LAST_MAINS_UPDATE_TIME = currentMillis;
      previous_mains_power_state = current_mains_power_state;            
      //Serial.println(current_mains_power_state);
      const char* buf;
      if (!isPowerOn() && current_mains_power_state == 1) {
        powerOn(); //power switch got flipped on and power isnt already on, so toggle it on
        buf = "Power toggled on by mains switch";
        logger->logInfo(buf);
      } else if (current_mains_power_state == 0) {
        powerOff(); //power switch flipped off and power is on so toggle it off
        buf = "Power toggled off by mains switch";
        logger->logInfo(buf);
      }
    }
  }
  */

  if ((currentMillis - lastRefreshDisplayTime) >= DISPLAY_UPDATE_INTERVAL) {
    lastRefreshDisplayTime = currentMillis;
    refreshDisplay();
  }

  switch (stopWatchBtn.getState()) {
    case 2:
      if (stopWatchState == LOW) {
        stopWatchState = !stopWatchState;
        stopWatchStartMillis = millis();
      } else if (stopWatchState == HIGH) {
        stopWatchState = !stopWatchState;
        stopWatchStoppedSeconds = stopWatchSeconds;
      }
      break;
    case 3:
      //reset stopwatch
      stopWatchStoppedSeconds = 0;
      stopWatchSeconds = 0;
      stopWatchState = LOW;
      break;
  }

  switch (btn1.getState()) {
    case 2: //pressed
      showWalkerAnimation();
      break;
    case 3: //held
      showVomiterAnimation();
      break;
  }

  switch (rebootBtn.getState()) {
    case 2: //pressed
      lastRebootMillis = currentMillis;
      break;
    case 3: //held
      //reset settings - wipe credentials for testing
      display.clearDisplay();
      display.setCursor(0, 0);
      display.setTextSize(1);
      display.println("erasing settings!");
      display.display();
      //SPIFFS.format();
      wm.resetSettings();
      break;
  }
  if (lastRebootMillis != 0 && (currentMillis - lastRebootMillis) >= 3000) {
    esp_restart();
  }


  switch (powerBtn.getState()) {
    case 2: //pressed
      togglePower();
      previous_mains_power_state = isPowerOn();
      break;
    case 3: //held
      if (!settings::get_use_without_wifi()) {
        settings::set_show_ip_address(!settings::get_show_ip_address());
      } else {
        settings::set_use_without_wifi(!settings::get_use_without_wifi());
        esp_restart();
      }
      break;
  }

  if (stopWatchState == HIGH) {
    stopWatchElapsedMillis = (millis() - stopWatchStartMillis);
    stopWatchSeconds = (stopWatchElapsedMillis / 1000) % 60;
    stopWatchSeconds += stopWatchStoppedSeconds;
    if (stopWatchSeconds != previousStopWatchSeconds) {
      previousStopWatchSeconds = stopWatchSeconds;
    }
  }

}

/**
   A method to update what is shown on the display screen
*/
void refreshDisplay() {
  display.clearDisplay();
  display.setTextSize(2);
  display.setCursor(0, 0);
  display.printf("%.1f", getFreshTemp());
  display.setTextSize(1);
  display.print((char)247);
  display.print("F");
  display.setTextSize(2);
  if (!settings::get_use_without_wifi()) {
    display.print('\n');
    display.print(myTime.getTimeFormatted());
    display.setTextSize(1);
    display.print(myTime.getAmPm());
    //the IP address
    display.setTextSize(2);
    display.println('\n');
    display.setTextSize(1);
    if (settings::get_show_ip_address()) display.println(WiFi.localIP());
  }
  
  //the stopwatch
  display.setTextSize(1);
  display.setCursor(100, 20);
  display.setTextSize(2);
  if (stopWatchSeconds != 0) display.print(stopWatchSeconds);
  
  //power symbol if power is on
  if(isPowerOn()) display.drawBitmap(55,45,power_bitmap,16,16,1);  
  
  display.display();
}

void handleWiFiDropOut() {
  if (handleWiFiDropOutReconnectAttempts > 60) {
    // if we still have not connected restart
    esp_restart();
  }
  if (WiFi.status() != WL_CONNECTED && ((millis() - lastHandleWiFiDropOutTimeStamp) > handleWiFiDropOutDelay)) {
    WiFi.mode(WIFI_STA);
    WiFi.disconnect();
    delay(100);
    char buf[48];
    sprintf(buf, "WiFi dropped, reconnect attempt %d", handleWiFiDropOutReconnectAttempts);
    buf[47] = '\0';
    logger->logInfo(buf);

    //WiFi.begin(wm.getWiFiSSID(true).c_str(), wm.getWiFiPass(true).c_str());
    WiFi.begin();
    handleWiFiDropOutReconnectAttempts++;
    lastHandleWiFiDropOutTimeStamp = millis();
  }
}
