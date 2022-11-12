#pragma once;

#include <SPI.h>
#include <Adafruit_MAX31855.h> //https://github.com/adafruit/Adafruit-MAX31855-library/
#include "Logger.h"

// digital IO pins.
#define MAXDO 14 
#define MAXCLK 12
#define MAXCS 15

extern String getTCErrorCode();
extern Logger* logger;

extern void initTempSensor();
extern void updateTempSensor();
extern float getFreshTemp();
extern float getLastTemp();
extern float getAverageTemp();
