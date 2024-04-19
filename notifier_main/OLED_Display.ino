#include "OLED_Display.h"

MenuState currentState = MAIN_MENU;
MenuState previousState = MAIN_MENU; // Track the previous state for accurate back navigation

void setup_OLED() {
  Serial.begin(115200);
  
  delay(250);
  display.begin(0x3C, true); // Initialize the display

  Serial.println("OLED begun");
  display.display();
  delay(1000);
  display.clearDisplay();
  display.display();
  display.setRotation(1);

  pinMode(BUTTON_A, INPUT_PULLUP);
  pinMode(BUTTON_B, INPUT_PULLUP);
  pinMode(BUTTON_C, INPUT_PULLUP);
  Serial.println("Setup for 128x64 OLED FeatherWing Complete");

  displayMainMenu(); // Display the main menu initially
}

void OLED_loop() {
  handleButtons(); // Handle button presses
}

void handleButtons() {
  static unsigned long lastDebounceTime = 0;
  static bool lastButtonStateA = HIGH;
  static bool lastButtonStateB = HIGH;
  static bool lastButtonStateC = HIGH;
  const unsigned long debounceDelay = 50; // Debounce time in milliseconds

  bool currentButtonStateA = digitalRead(BUTTON_A);
  bool currentButtonStateB = digitalRead(BUTTON_B);
  bool currentButtonStateC = digitalRead(BUTTON_C);

  if ((millis() - lastDebounceTime) > debounceDelay) {
    if (currentButtonStateA == LOW && lastButtonStateA == HIGH) {
      handleButtonA();
      lastDebounceTime = millis();
    }
    if (currentButtonStateB == LOW && lastButtonStateB == HIGH) {
      handleButtonB();
      lastDebounceTime = millis();
    }
    if (currentButtonStateC == LOW && lastButtonStateC == HIGH) {
      handleButtonC();
      lastDebounceTime = millis();
    }
  }

  lastButtonStateA = currentButtonStateA;
  lastButtonStateB = currentButtonStateB;
  lastButtonStateC = currentButtonStateC;
}

void handleButtonA() {
  previousState = currentState; // Save the current state before changing
  switch (currentState) {
    case MAIN_MENU:
      currentState = START_MENU;
      displayStartMenu();
      break;
    case SETTINGS_MENU:
      currentState = CALIBRATE_MENU;
      displayCalibrateMenu();
      break;
    default:
      break;
  }
}

void handleButtonB() {
  previousState = currentState; // Save the current state before changing
  switch (currentState) {
    case MAIN_MENU:
      currentState = SETTINGS_MENU;
      displaySettingsMenu();
      break;
    case SETTINGS_MENU:
      currentState = REGISTER_MENU;
      displayRegisterMenu();
      break;
    default:
      break;
  }
}

void handleButtonC() {
  // Navigate back to the previous state
  currentState = previousState;
  previousState = MAIN_MENU; // Reset previousState to MAIN_MENU after going back
  switch (currentState) {
    case MAIN_MENU:
      displayMainMenu();
      break;
    case START_MENU:
      displayStartMenu();
      break;
    case SETTINGS_MENU:
      displaySettingsMenu();
      break;
    case CALIBRATE_MENU:
      displayCalibrateMenu();
      break;
    case REGISTER_MENU:
      displayRegisterMenu();
      break;
    default:
      displayMainMenu(); // Default to main menu if state is unknown
      break;
  }
}

void displayMainMenu() {
  display.clearDisplay();
  display.setTextSize(2);
  display.setTextColor(SH110X_WHITE);
  display.setCursor(0, 0);
  display.println("Main Menu");
  display.setTextSize(1);
  display.println("A: Start");
  display.println("B: Settings");
  display.display();
}

void displayStartMenu() {
  display.clearDisplay();
  display.setTextSize(2);
  display.setTextColor(SH110X_WHITE);
  display.setCursor(0, 0);
  display.println("Start Menu");
  display.setTextSize(1);
  display.println("Press C to go back");
  display.display();
}

void displayCalibrateMenu() {
  display.clearDisplay();
  display.setTextSize(2);
  display.setTextColor(SH110X_WHITE);
  display.setCursor(0, 0);
  display.println("Calibrate");
  display.setTextSize(1);
  display.println("Press C to go back");
  display.display();
}

void displaySettingsMenu() {
  display.clearDisplay();
  display.setTextSize(2);
  display.setTextColor(SH110X_WHITE);
  display.setCursor(0, 0);
  display.println("Settings");
  display.setTextSize(1);
  display.println("A: Calibrate");
  display.println("B: Register");
  display.println("C: Back");
  display.display();
}

void displayRegisterMenu() {
  display.clearDisplay();
  display.setTextSize(2);
  display.setTextColor(SH110X_WHITE);
  display.setCursor(0, 0);
  display.println("Register");
  display.setTextSize(1);
  display.println("Press C to go back");
  display.display();
}