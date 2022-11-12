#include "temperature_sensor.h"
#include "settings.h"

Adafruit_MAX31855 thermocouple(MAXCLK, MAXCS, MAXDO);
float latestReading;
float tcSum;
int readCount;
boolean isError = false;
static int errorCounter = 0;
static int tc_error_code;
float temperature;
unsigned long LOG_UPDATE_INTERVAL = 5000;
unsigned long LAST_LOG_TIME = 0;

char* MAX31855_ERROR_CODES[5] = {"NaN", "open_circuit", "short_to_ground", "short_to_vcc", "reading_0.0"};

float temperatureHistory[10]; //the last 10 temp readings used to calculate an average
int historyIterator = 0;

extern void initTempSensor() {
  float latestReading = 0.0;
  float tcSum = 0.0;
  int readCount = 0;
}

extern void updateTempSensor() {
  tc_error_code = thermocouple.readError();
  isError = (tc_error_code == 1 || tc_error_code == 2 || tc_error_code == 4);
  temperature = float(thermocouple.readFahrenheit()) + settings::get_temperature_sensor_offset(); //with calibration
  //Serial.println("temperature: " + String(temperature));
  
  if (isnan(temperature) || isError || temperature == 0.00) {
    errorCounter++;
    //log the error once and only again if the error persists longer than LOG_UPDATE_INTERVAL
    if (millis() - LAST_LOG_TIME > LOG_UPDATE_INTERVAL) {
      char buf[64];
      if (isError) {
        sprintf(buf, "thermocouple error: (%d) %s", tc_error_code, MAX31855_ERROR_CODES[tc_error_code]);
      } else if (isnan(temperature)) {
        sprintf(buf, "thermocouple error: (%d) %s", 0, MAX31855_ERROR_CODES[0]);
      } else  if (temperature == 0.00) {
        sprintf(buf, "thermocouple error: (%d) %s", 4, MAX31855_ERROR_CODES[4]);
      }
      buf[63] = '\0';
      logger->logWarning(buf);
      LAST_LOG_TIME = millis();
    }
  } else {
    tcSum += temperature;
    readCount += 1;
  }
}

extern String getTCErrorCode() {
  return String(tc_error_code) + "_" + String(errorCounter);
}

extern float getFreshTemp() {
  //prevent divide by zero
  if (readCount != 0) {
    latestReading = tcSum / readCount;
    readCount = 0;
    tcSum = 0.0;
  }

  if (historyIterator == 10) historyIterator = 0;
  temperatureHistory[historyIterator++] = latestReading;

  return latestReading;
}

extern float getLastTemp() {
  return latestReading;
}

extern float getAverageTemp() {
  float average = 0.0;
  for (int i = 0; i < 10; ++i) {
    average += temperatureHistory[i];
  }
  return average / 10.0;
}
