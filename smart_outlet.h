#pragma once
#include <Arduino.h>
#include "Logger.h"

extern void initSmartOutlet();
extern Logger* logger;

/**
   A method to get the state of the relay, if it is on or not and return a String: on or off
*/
extern const char* getOutletStatus();
/**
   A method to get the state of the relay, if it is on or not and return a boolean
*/
extern boolean isPowerOn();
/**
   A method to turn the relay and LED off/on
*/
extern void togglePower();

/**
   A method to turn the LED on because the main power switch on the front panel of the machine was flipped on
*/
extern void powerOn();

/**
   A method to turn the LED off because the main power switch on the front panel of the machine was flipped off
*/
extern void powerOff();

/**
  A method to calculate the amount of time the relay has been on. Returns milliseconds.
**/
extern unsigned long getRuntime();
/**
  A method to calculate the amount of time the relay has been on. Returns string.
**/
extern const char* getRuntimeString();

/**
   A method to turn off the relay if it has been on for too long or just for safety
*/
extern void checkSafety();
