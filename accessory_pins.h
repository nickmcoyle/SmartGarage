#pragma once
//PINS NOT AVAILABLE 12,13,14,15,16,18,19,21,22,26,34,32
#define BUTTON1_PIN 19
#define STOPWATCH_BUTTON_PIN 18 //shot timer stopwatch feature
#define REBOOT_BUTTON_PIN 13 //press to restart the device, hold to factory reset the config
#define POWER_BUTTON_PIN 16 //the button to turn on or off the relays
#define LED_PIN 23 //LED to indicate power on 
#define MAIN_POWER_RELAY_PIN 26 //the relay used to turn the power on or off to the machine at the main power switch
#define MAINS_POWER_DETECTOR_PIN 34 //detects when the power switch on the espresso machine was switched on
#define HEAT_RELAY_PIN 32 //the solid state relay used to control the power going to the boiler/heater for brewing coffee(not steaming)
