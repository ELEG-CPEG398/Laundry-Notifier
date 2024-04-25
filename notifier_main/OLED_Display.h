#ifndef OLED_Display
#define OLED_Display

#include <SPI.h>
#include <Wire.h>
#include <Adafruit_GFX.h>
#include <Adafruit_SH110X.h>

// Define button pins based on the board in use
#if defined(ESP8266)
  #define BUTTON_A  0
  #define BUTTON_B 16
  #define BUTTON_C  2
#elif defined(ESP32) && !defined(ARDUINO_ADAFRUIT_FEATHER_ESP32S2)
  #define BUTTON_A 15
  #define BUTTON_B 32
  #define BUTTON_C 14
#endif

// Function declarations for displaying different menus
void setup_OLED();
void OLED_loop();
void handleButtons();
void handleButtonA();
void handleButtonB();
void handleButtonC();
void displayMainMenu();
void displayStartMenu(bool isUnclaimed, bool inProgress);
bool displayCalibrateMenu(bool calibrateDevice);
void displaySettingsMenu();
void displayRegisterMenu();

enum MenuState {
  MAIN_MENU,
  START_MENU,
  SETTINGS_MENU,
  CALIBRATE_MENU,
  REGISTER_MENU
};

// Global Variables
MenuState currentState = MAIN_MENU;  // Track the current state
MenuState previousState = MAIN_MENU; // Track the previous state for accurate back navigation
Adafruit_SH1107 display = Adafruit_SH1107(64, 128, &Wire);

#endif