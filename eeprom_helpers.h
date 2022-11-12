#pragma once

// EEPROM lets you store 1 byte at a time at 1 address
// a float might be a 32bit value (4 bytes)
// use EEPROM library get and put functions to save floats

#include <EEPROM.h>

// These are addresses into EEPROM memory.  The values to be stored are floats which
// need 4 bytes each.  Thus 0,4,8,12,...
#define PGAIN_ADR 0
#define IGAIN_ADR 4
#define DGAIN_ADR 8
#define WINDUP_ADR 12
#define ESPRESSO_TEMP_ADDRESS 16
#define STEAM_TEMP_ADDRESS 20  //steam temp currently not used with bare bones setup
#define TEMPERATURE_LIMIT_ADR 24
#define RUNTIME_LIMIT_ADR 28
#define LOGGING_ENABLED_ADR 32
#define SERIAL_GRAPHING_ENABLED_ADR 36
#define TEMPERATURE_SENSOR_OFFSET_ADR 40
#define USE_WITHOUT_WIFI_ADR 44
#define SHOW_IP_ADDRESS_ADR 48
#define APP_NAME_ADR 100

namespace eeprom {
  extern void initEEPROM();
  extern float readFloat(int address);
  extern void writeFloat(float value, int address);
  extern int readInt(int address);
  extern void writeInt(int value, int address);  
  extern const char * readStr(int address);
  extern void writeStr(const char* value, int address);
  extern bool readBool(int address);
  extern void writeBool(bool value, int address);
}
