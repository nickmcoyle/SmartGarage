#pragma once

// PID control code
// Tim Hirzel
// December 2007

// This is a module that implements a PID control loop
// initialize it with 3 values: pgain,igain,dgain
// and then tune the feedback loop with the setP etc funcs
//
// this was written based on a great PID by Tim Wescott:
// https://www.embedded.com/2000/0010/0010feat3.htm
//
//
// All code released under
// Creative Commons Attribution-Noncommercial-Share Alike 3.0

//has the initialize, read and write methods and the addresses
#include "eeprom_helpers.h"

extern float getTargetTemp();
extern float getPGain();
extern float getIGain();
extern float getDGain();
extern float getWindupGuardGain();

extern void initPID();
extern void setTargetTemp(float t);
extern void setPGain(float p);
extern void setIGain(float i);
extern void setDGain(float d);
extern float setWindupGuardGain(float wgg);

extern float updatePID(float targetTemp, float curTemp); //this is where the control algorithm lives

extern void printPIDDebugString();

extern void initDefaultPIDValues();
