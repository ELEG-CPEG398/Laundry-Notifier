#include "OLED_Display.h"

void setup_OLED() {
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
  displayMainMenu(); // Display the main menu initially
}

void OLED_loop() {
  handleButtons(); // Handle button presses
  if (displayNeedsUpdate) {
    switch (currentState) {
      case MAIN_MENU:
        displayMainMenu();
        break;
    }
  }
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
      displayStartMenu(false,false,"");
      displayNeedsUpdate = true;  // Set the flag to update the display
      break;
    case START_MENU:
      //user = "";
      currentState = FINGERPRINT_MENU;
      displayFingerPrintMenu(false);
      break;
    case SETTINGS_MENU:
      currentState = CALIBRATE_MENU;
      displayCalibrateMenu(false);
      break;
    case CALIBRATE_MENU:
      THRESHOLD = 1.0; // Reset to default value
      Serial.println("Threshold value reset");
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
      displayNeedsUpdate = true;  // Set the flag to update the display
      break;
    case START_MENU:
      displayStartMenu(false,false,"");
      break;
    case SETTINGS_MENU:
      displaySettingsMenu();
      break;
    case CALIBRATE_MENU:
      displayCalibrateMenu(false);
      break;
    case REGISTER_MENU:
      displayRegisterMenu();
      break;
    default:
      displayMainMenu(); // Default to main menu if state is unknown
      displayNeedsUpdate = true;  // Set the flag to update the display
      break;
  }
}

void displayMainMenu() {
  if (!displayNeedsUpdate) return; 
  
  display.clearDisplay();
  display.setTextSize(2);
  display.setTextColor(SH110X_WHITE);
  display.setCursor(0, 0);
  display.println("Main Menu");
  display.setTextSize(1);
  display.println("A: Start");
  display.println("B: Settings");
  display.display();
  displayNeedsUpdate = false;
}

void displayStartMenu(bool isUnclaimed, bool inProgress, String username) {
  display.clearDisplay();
  display.setTextSize(2);
  display.setTextColor(SH110X_WHITE);

  display.setCursor(0, 0);
  if(inProgress){
    display.println("Ongoing");
    display.setTextSize(1);
  }
  else{
    display.println("Waiting...");
    display.setTextSize(1);
  }

  if(isUnclaimed){
    display.println("A: Claim Ownership");
  }
  else{
    display.println("Owned by " + user);
    display.println("A: Change Ownership");
  }

  display.println("C: Back");

  display.display();
}

bool displayCalibrateMenu(bool calibrateDevice) {
  if(calibrateDevice){
    display.clearDisplay();
    display.setTextSize(1);
    display.setTextColor(SH110X_WHITE);
    display.setCursor(3, 0);
    display.println("Calibrating. . .");
    display.display();
  }else{
    display.clearDisplay();
    display.setTextSize(2);
    display.setTextColor(SH110X_WHITE);
    display.setCursor(0, 0);
    display.println("Calibrate");
    display.setTextSize(1);
    display.print("Detect Level: ");
    display.println(THRESHOLD);
    display.println("\nPress A to reset");
    display.println("Press B to calibrate");
    display.println("Press C to go back");
    display.display();
  }

  return calibrateDevice;
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

void displayFingerPrintMenu(bool isRegistering){
  display.clearDisplay();
  display.setTextSize(1);
  display.setTextColor(SH110X_WHITE);
  display.setCursor(0, 0);
  display.println("Scan Fingerprint");
  display.println("C: Back");
  display.display();
  if(isRegistering){

  }
  else{
    if(getFingerprintID() == FINGERPRINT_OK){
      int i = finger.fingerID;
      user =  i;
      Serial.println(i);
      Serial.println("User: " + user);
      currentState = previousState;
      previousState = MAIN_MENU;
    }
    else if(getFingerprintID() == FINGERPRINT_NOTFOUND){
      display.println("Unknown user.\n Considered registering.");
      display.display();
      delay(2000);
      currentState = previousState;
      previousState = MAIN_MENU;
    }
    else{
      display.println("Please try again.");
      display.display();
      delay(2000);
      currentState = previousState;
      previousState = MAIN_MENU;
    }
    delay(5);
    //int timeout = 0;
    //for(int timeout = 0; getFingerprintID() != FINGERPRINT_OK && timeout < 5000; timeout += 5){ 
      //delay(5); 
    //}
  }
  
}

void error_found(errorCodes e){
  Serial.println("Error: Closing Program");
  display.clearDisplay();
  display.setTextSize(1);
  display.setTextColor(SH110X_WHITE);
  display.setCursor(3, 0);
  switch (e) {
    case VIBRATIONSENSOR_FAILED:
      display.println("Failed to find \nvibration sensor");
      break;
    case FINGERPRINT_FAILED:
      display.println("Failed to find \nfingerprint sensor");
      break;
    default:
      display.println("Unknown Error");
      break;
  }

  display.display();
  while(true){delay(10);}
}
