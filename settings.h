#pragma once;

#include "eeprom_helpers.h"

//settings to let you change number of safety shutoff time and or temperature, name of app, and other things

namespace settings {    
  extern void set_defaults();
  extern void init_settings();
  void debug_settings();
    
  const char* get_app_name();  
  unsigned int get_runtime_limit();  
  float get_temperature_limit();  
  float get_temperature_sensor_offset();
  bool get_serial_graphing_enabled();
  bool get_logging_enabled();
  bool get_use_without_wifi();
  bool get_show_ip_address();
  
  void set_app_name(const char* input);  
  void set_runtime_limit(int input);  
  void set_temperature_limit(float input);  
  void set_temperature_sensor_offset(float input);
  void set_serial_graphing_enabled(bool input);
  void set_logging_enabled(bool input);
  void set_use_without_wifi(bool input);  
  void set_show_ip_address(bool input);
}
