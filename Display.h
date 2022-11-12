#pragma once;

#include <SPI.h>
#include <Wire.h>
#include <Adafruit_GFX.h>
#include <Adafruit_SSD1306.h>

#define SCREEN_WIDTH 128 // OLED display width, in pixels
#define SCREEN_HEIGHT 64 // OLED display height, in pixels
//for ESP32
//SDA on 21
//SCL on 22
// Declaration for an SSD1306 display connected to I2C (SDA, SCL pins)
#define OLED_RESET 4 // Reset pin # (or -1 if sharing Arduino reset pin)
Adafruit_SSD1306 display(SCREEN_WIDTH, SCREEN_HEIGHT, &Wire, OLED_RESET);

void initDisplay()
{
  // SSD1306_SWITCHCAPVCC = generate display voltage from 3.3V internally
  if (!display.begin(SSD1306_SWITCHCAPVCC, 0x3C)) { // Address 0x3C for 128×32
    Serial.println(F("SSD1306 allocation failed"));
    for (;;); // Don’t proceed, loop forever
  }
  //display.setRotation(1); //rotate 45 degrees
  // Clear the buffer.
  display.clearDisplay();
  display.setTextColor(WHITE);
  display.setTextSize(2);
}
