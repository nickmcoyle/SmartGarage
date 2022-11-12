#include "settings.h"

namespace settings {
  const char * app_name;
  unsigned int runtime_limit; //time in milliseconds used to shut off power after specified interval for safety
  float temperature_limit; //degrees Fahrenheit used to shut off power if reached this temperature
  bool logging_enabled; //turn on logging to file through logger class
  bool serial_graphing_enabled; //turn on printing data to serial for graphing in Processing software
  float temperature_sensor_offset; //offset to the thermocouple probe for calibration
  bool use_without_wifi; //allows to use the device without wifi, PID still works and you can turn it on or off with a button
  bool show_ip_address; //toggles showing the IP address on the display or not
  
  extern void set_defaults() {
    set_app_name("Esp PID");
    set_runtime_limit( 120 * 60 * 1000 ); //time in milliseconds used to shut off power after specified interval for safety
    set_temperature_limit(350.00); //degrees Fahrenheit used to shut off power if reached this temperature
    set_logging_enabled(true); //turn on logging to file through logger class
    set_serial_graphing_enabled(false); //turn on printing data to serial for graphing in Processing software
    set_temperature_sensor_offset(0.0); //offset to the thermocouple probe for calibration
    set_use_without_wifi(false); //allows to use the device without wifi, PID still works and you can turn it on or off with a button
    set_show_ip_address(true); //toggles showing the IP address on the display or not
  }
  
  extern void init_settings() {
    app_name = eeprom::readStr(APP_NAME_ADR);
    runtime_limit = eeprom::readInt(RUNTIME_LIMIT_ADR); //time in milliseconds used to shut off power after specified interval for safety
    temperature_limit = eeprom::readFloat(TEMPERATURE_LIMIT_ADR); //degrees Fahrenheit used to shut off power if reached this temperature
    logging_enabled = eeprom::readBool(LOGGING_ENABLED_ADR); //turn on logging to file through logger class
    serial_graphing_enabled = eeprom::readBool(SERIAL_GRAPHING_ENABLED_ADR); //turn on printing data to serial for graphing in Processing software
    temperature_sensor_offset = eeprom::readBool(TEMPERATURE_SENSOR_OFFSET_ADR); //offset to the thermocouple probe for calibration
    use_without_wifi = eeprom::readBool(USE_WITHOUT_WIFI_ADR); //allows to use the device without wifi, PID still works and you can turn it on or off with a button
    show_ip_address = eeprom::readBool(SHOW_IP_ADDRESS_ADR); //toggles showing the IP address on the display or not
    //Serial.println("initing settings: runtimelimit: " + String(runtime_limit) + " temp limit" + String(temperature_limit) + " app_name: " + String(app_name));
    
    //check if eeprom is not initialized on a new board and set default values    
    if(logging_enabled != true && logging_enabled != false || use_without_wifi != true && use_without_wifi != false || show_ip_address != true && show_ip_address != false || serial_graphing_enabled != true && serial_graphing_enabled != false) {
      Serial.println("setting defaults"); 
      set_defaults();
    } else {
      //debug_settings();
    }
  }
  
  const char* get_app_name() {
    return app_name;
  }
  
  unsigned int get_runtime_limit() {
    return runtime_limit;
  }
  
  float get_temperature_limit() {
    return temperature_limit;
  }
  
  bool get_logging_enabled() {
    return logging_enabled;
  }
  
  bool get_serial_graphing_enabled() {
    return serial_graphing_enabled;
  }
  
  float get_temperature_sensor_offset() {
    return temperature_sensor_offset;
  }

  bool get_use_without_wifi() {
    return use_without_wifi;
  }   

  bool get_show_ip_address() {
    return show_ip_address;
  }  
  
  void set_app_name(const char* input) {
    // set the value and store it to eeprom
    static char arrayToStore[64];
    strncpy(arrayToStore, input, sizeof(arrayToStore));
    arrayToStore[63] = '\0';
    app_name = arrayToStore;
    eeprom::writeStr(app_name, APP_NAME_ADR);
  }
  
  void set_runtime_limit(int input) {
    if (input > 0) {
      // set the value and store it to eeprom
      runtime_limit = input;
      eeprom::writeInt(runtime_limit, RUNTIME_LIMIT_ADR);
    }
  }
  
  void set_temperature_limit(float input) {
    if (input > 0.0) {
      //Serial.println("writing temp limit to eeprom: " + String(input));
      // set the value and store it to eeprom
      temperature_limit = input;
      eeprom::writeFloat(temperature_limit, TEMPERATURE_LIMIT_ADR);
    }
  }
  
  void set_logging_enabled(bool input) {
    // set the value and store it to eeprom
    logging_enabled = input;
    eeprom::writeBool(logging_enabled, LOGGING_ENABLED_ADR);
  }
  
  void set_serial_graphing_enabled(bool input) {
    // set the value and store it to eeprom
    serial_graphing_enabled = input;
    eeprom::writeBool(serial_graphing_enabled, SERIAL_GRAPHING_ENABLED_ADR);
  }
  
  void set_temperature_sensor_offset(float input) {
    // set the value and store it to eeprom
    temperature_sensor_offset = input;
    eeprom::writeFloat(temperature_sensor_offset, TEMPERATURE_SENSOR_OFFSET_ADR);  
  }

  void set_use_without_wifi(bool input) {
    // set the value and store it to eeprom    
    use_without_wifi = input;
    eeprom::writeBool(use_without_wifi, USE_WITHOUT_WIFI_ADR);
  }

  void set_show_ip_address(bool input) {
    // set the value and store it to eeprom
    show_ip_address = input;
    eeprom::writeBool(show_ip_address, SHOW_IP_ADDRESS_ADR);
  }

  void debug_settings() {    
  Serial.print("Settings:");
  Serial.println("app_name: " + String(app_name));
  Serial.println("runtime_limit: " + String(runtime_limit));
  Serial.println("temperature_limit: " + String(temperature_limit));
  Serial.println("logging_enabled: " + String(logging_enabled));
  Serial.println("serial_graphing_enabled: " + String(serial_graphing_enabled));
  Serial.println("temperature_sensor_offset: " + String(temperature_sensor_offset));
  Serial.println("use_without_wifi: " + String(use_without_wifi));
  Serial.println("show_ip_address: " + String(show_ip_address));
  Serial.println("");
  }
}
