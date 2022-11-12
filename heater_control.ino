// HeaterControl
// Tim Hirzel
// Dec 2007
//
// This file is for controlling a heater via a solid state zero crossing relay

// since these are zero-crossing relays, it makes sense to just match my local
// AC frequency, 60hz
//
// All code released under
// Creative Commons Attribution-Noncommercial-Share Alike 3.0

float heatCycles; // the number of millis out of 1000 for the current heat amount (percent * 10)

boolean heaterState = 0;

unsigned long heatCurrentTime, heatLastTime;

void initHeater() {
  pinMode(HEAT_RELAY_PIN , OUTPUT);
}

void updateHeater() {
  boolean h;
  heatCurrentTime = millis();
  //Serial.println("heatCurrentTime - heatLastTime= " + String(heatCurrentTime - heatLastTime) + "; heatcyles: " + String(heatCycles));
  if(heatCurrentTime - heatLastTime >= 1000) { 
    // begin cycle
    _turnHeatElementOnOff(1);  //
    heatLastTime = heatCurrentTime;  
  }
  if (heatCurrentTime - heatLastTime >= heatCycles) {
    _turnHeatElementOnOff(0);
  }
}

void setHeatPowerPercentage(float power) {
  //Serial.println("power = " + String(power));
  if (power <= 0.0) {
    power = 0.0;
  }    
  if (power >= 1000.0) {
    power = 1000.0;
  }
  heatCycles = power;
}

float getHeatCycles() {
  return heatCycles;
}

void _turnHeatElementOnOff(boolean onOff) {  
  digitalWrite(HEAT_RELAY_PIN, onOff);     //turn pin high
  String onOffString = "off";
  if(onOff) onOffString = "on";
  //Serial.println("_turnHeatElementOnOff: " + onOffString);
  heaterState = onOff;
}

// End Heater Control
