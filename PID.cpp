#include "PID.h"
#include "Logger.h"

extern Logger* logger;

float iState = 0.00;
float lastTemp = 0.00;
float pTerm, iTerm, dTerm, pgain, igain, dgain, windupguardgain, targetTemp;

extern void initPID() {
  pgain = eeprom::readFloat(PGAIN_ADR);
  igain = eeprom::readFloat(IGAIN_ADR);
  dgain = eeprom::readFloat(DGAIN_ADR);
  windupguardgain = eeprom::readFloat(WINDUP_ADR);
  targetTemp = eeprom::readFloat(ESPRESSO_TEMP_ADDRESS);
  
  //check if eeprom is not initialized on a new board and set default values  
  //if(isnan(targetTemp)) Serial.println("setting default PID values"); initDefaultPIDValues();
}

float getTargetTemp() {
  return targetTemp;
}

float getPGain() {
  // get the P gain
  return pgain;
}
float getIGain() {
  // get the I gain
  return igain;
}
float getDGain() {
  // get the D gain
  return dgain;
}
float getWindupGuardGain() {
  return windupguardgain;
}

void setTargetTemp(float t) {
  targetTemp = t;
  eeprom::writeFloat(t, ESPRESSO_TEMP_ADDRESS);
}

void setPGain(float p) {
  // set the P gain and store it to eeprom
  pgain = p;
  eeprom::writeFloat(p, PGAIN_ADR);
}

void setIGain(float i) {
  // set the I gain and store it to eeprom
  igain = i;
  eeprom::writeFloat(i, IGAIN_ADR);
}

void setDGain(float d) {
  // set the D gain and store it to eeprom
  dgain = d;
  eeprom::writeFloat(d, DGAIN_ADR);
}
float setWindupGuardGain(float wgg) {
  windupguardgain = wgg;
  eeprom::writeFloat(wgg, WINDUP_ADR);
}

float updatePID(float targetTemp, float curTemp)
{  
  // these local variables can be factored out if memory is an issue,
  // but they make it more readable
  double result;
  float error;
  float windupGuard;

  // determine how badly we are doing
  error = targetTemp - curTemp;
  //Serial.println("from PID algorithm file, targetTemp= " + String(targetTemp));
  //Serial.println("from PID algorithm file, error= " + String(error));
  // the pTerm is the view from now, the pgain judges
  // how much we care about error we are this instant.
  pTerm = pgain * error;  
  // iState keeps changing over time; it's
  // overall "performance" over time, or accumulated error
  iState += error;
  //Serial.println("iState: " + String(iState));
  // to prevent the iTerm getting huge despite lots of
  //  error, we use a "windup guard"
  // (this happens when the machine is first turned on and
  // it cant help be cold despite its best efforts)

  // not necessary, but this makes windup guard values
  // relative to the current iGain
  windupGuard = windupguardgain / igain;  
  //Serial.println("windupGuard: " + String(windupGuard));
  if (iState > windupGuard) {
    iState = windupGuard;
  } else if (iState < -windupGuard) {
    iState = -windupGuard;
  }
  
  iTerm = igain * iState;

  // the dTerm, the difference between the temperature now
  //  and our last reading, indicated the "speed,"
  // how quickly the temp is changing. (aka. Differential)
  dTerm = (dgain* (curTemp - lastTemp));
  //Serial.println("curTemp: " + String(curTemp) + " lastTemp: " + String(lastTemp));
  // now that we've use lastTemp, put the current temp in
  // our pocket until for the next round
  lastTemp = curTemp;
  //Serial.println("from PID algorithm file, temperature= " + String(lastTemp));
  // the magic feedback bit  
  return  pTerm + iTerm - dTerm;
}

void printPIDDebugString() {
  // A  helper function to keep track of the PID algorithm
  Serial.print("PID formula (P + I - D): ");
  Serial.print(String(pTerm));
  Serial.print(" + ");
  Serial.print(String(iTerm));
  Serial.print(" - ");
  Serial.print(String(dTerm));
  Serial.print(" POWER: ");
  //Serial.print(String(getHeatCycles()));
  Serial.print("\n");
}

void initDefaultPIDValues() {
  const char* buf;
  buf = "PID settings reset to default values\0";  
  logger->logInfo(buf);
  
  setTargetTemp(202.00);    
  setPGain(180.00);
  setIGain(0.125);
  setDGain(0.03125);
  setWindupGuardGain(0.00);
}
