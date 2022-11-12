#include "smart_outlet.h"
#include "accessory_pins.h"
#include "temperature_sensor.h"
#include "settings.h"

const char* outletStatus;
unsigned long PowerOnTimeStamp; //the timestamp at which the power was turned on, or 0 if the outlet is off

/**
  A method to initialize I/O pins
**/
extern void initSmartOutlet() {
  pinMode(MAIN_POWER_RELAY_PIN, OUTPUT);
  pinMode(MAINS_POWER_DETECTOR_PIN, INPUT);
  digitalWrite(MAIN_POWER_RELAY_PIN, LOW);
  pinMode(LED_PIN, OUTPUT);
  digitalWrite(LED_PIN, LOW);
  PowerOnTimeStamp = 0;
  outletStatus = getOutletStatus();
}

/**
   A method to get the state of the relay, if it is on or not and return a String: on or off
*/
extern const char* getOutletStatus() {
  if (isPowerOn()) {
    outletStatus = "ON";
  } else {
    outletStatus = "OFF";
  }
  return outletStatus;
}

/**
   A method to get the state of the relay, if it is on or not and return boolean
*/
extern boolean isPowerOn() {
  return (digitalRead(MAIN_POWER_RELAY_PIN) == 1);  
}

/**
   A method to turn the relay and LED off/on
*/
extern void togglePower() {  
  digitalWrite(MAIN_POWER_RELAY_PIN, !digitalRead(MAIN_POWER_RELAY_PIN));      
  
  digitalWrite(LED_PIN, !digitalRead(LED_PIN));

  //if the coffee machine is on, then reset the run timer, else it should be 0
  if (isPowerOn()) {
    PowerOnTimeStamp = millis();    
  } else {
    PowerOnTimeStamp = 0;
  }
}

/**
   A method to turn the relay and LED on
*/
extern void powerOn() {
  digitalWrite(MAIN_POWER_RELAY_PIN, HIGH);
  digitalWrite(LED_PIN, HIGH);
  PowerOnTimeStamp = millis();  
}

/**
   A method to turn the relay and LED off
*/
extern void powerOff(){
  digitalWrite(MAIN_POWER_RELAY_PIN, LOW);
  digitalWrite(LED_PIN, LOW);
  PowerOnTimeStamp = 0;  
}


/**
  A method to calculate the amount of time the relay has been on. Returns milliseconds.
**/
unsigned long getRuntime() {
  unsigned long runTime; //the amount of time the smart outlet has been on
  if (isPowerOn()) {
    runTime = millis() - PowerOnTimeStamp;
  } else {
    runTime = 0;
  }
  return runTime;
}

/**
  A method to calculate the amount of time the relay has been on. Returns string.
**/
const char * getRuntimeString() {
  if (!isPowerOn()) {
    return "--";
  }
  unsigned long run_time = getRuntime();
  unsigned char seconds = (run_time / 1000) % 60;
  unsigned short int minutes = (run_time) / 60000;

  static char response[64];
  char * p = response;
  if (minutes < 1) {
    p += sprintf(p, "%is", seconds);
  } else {
    p += sprintf(p, "%im", minutes);
  }
  *p += 0;

  return response;
}

/**
   A method to turn off the relay if it has been on for 1 hour just for safety
*/
extern void checkSafety() {
  if (digitalRead(MAIN_POWER_RELAY_PIN) == HIGH) {
    unsigned long run_time = getRuntime();

    //check exceeded runtime limit
    if (run_time >= settings::get_runtime_limit()) {
      togglePower();
      char buf[48];
      sprintf(buf, "smartoutlet overtime: %d minutes", (run_time / 60000));
      buf[47] = '\0';
      logger->logWarning(buf);
    }

    //check thermal runaway for whatever reason
    float averageTemp = getAverageTemp(); //average temp over a few seconds
    if (averageTemp > settings::get_temperature_limit()) {
      powerOff();
      char buf[32];
      sprintf(buf, "smartoutlet overheat %f F", averageTemp);
      buf[31] = '\0';
      logger->logError(buf);
    }
  }
}
