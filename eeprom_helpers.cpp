#include "eeprom_helpers.h"

namespace eeprom {
  extern void initEEPROM() {
    EEPROM.begin(512);
  }
  
  extern float readFloat(int address) {
    float output = 0.0;
    EEPROM.get(address, output);
    //Serial.println("read float: " + String(output) + " from eeprom address: " + String(address));
    return output;
  }
  
  extern void writeFloat(float value, int address) {
    EEPROM.put(address, value);
    EEPROM.commit();
    //float myTemp = 0.00;
    //EEPROM.get(address, myTemp);
    //Serial.println("Just saved " + String(value) + " to eeprom. Should be: " + String(myTemp) + " at address: " + String(address));
  }
  
  extern int readInt(int address) {
    int output = 0;
    EEPROM.get(address, output);
    //Serial.println("read int: " + String(output) + " from eeprom address: " + String(address));
    return output;
  }
  
  extern void writeInt(int value, int address) {
    EEPROM.put(address, value);
    EEPROM.commit();
  }
  
  extern const char * readStr(int address) {
    static char output[64];
    EEPROM.get(address, output);
    return output;
  }
  
  extern void writeStr(const char * value, int address) {
    char arrayToStore[64];
    strncpy(arrayToStore, value, sizeof(arrayToStore));
    arrayToStore[63] = '\0';
    EEPROM.put(address, arrayToStore);
    EEPROM.commit();
    /*
    char output[64];
    EEPROM.get(address, output);
    Serial.println("Just saved " + String(arrayToStore) + " to eeprom. Should be: " + String(output) + " at address: " + String(address));
    */
  }

  extern bool readBool(int address) {
    bool output;
    EEPROM.get(address, output);
    //Serial.println("read int: " + String(output) + " from eeprom address: " + String(address));
    return output;
  }
  
  extern void writeBool(bool value, int address) {
    EEPROM.put(address, value);
    EEPROM.commit();
  }  
}
