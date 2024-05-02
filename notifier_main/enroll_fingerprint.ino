
#include "get_fingerprint.h"
#include "OLED_Display.h"



uint8_t getFingerprintEnroll(String username) {
  int p = -1;
  uint8_t id = finger.getTemplateCount();
  Serial.print("Waiting for valid finger to enroll as #"); Serial.println(id+1);
  
  // Display Status update
  display.println("Scan Finger. . . ");
  display.println("C: Cancel");
  display.display();

  while (p != FINGERPRINT_OK) {
    p = finger.getImage();
    p = finger.image2Tz(1);
    switch (p) {
    case FINGERPRINT_OK:
      Serial.println("Image taken");
      break;
    }

    if(!digitalRead(BUTTON_C)){
      return -1;
    }
    ArduinoCloud.update();
  }

  Serial.println("Remove finger");

  // Display Status update
  display.clearDisplay();
  display.setCursor(0, 0);
  display.println("Remove finger. . .");
  display.display();
  delay(2000);

  p = 0;
  while (p != FINGERPRINT_FEATUREFAIL) {
    p = finger.getImage();
    p = finger.image2Tz(2);
  }
  Serial.print("ID "); Serial.println(id);
  p = -1;

  Serial.println("Place same finger again");
  
  // Display Status update
  display.clearDisplay();
  display.setCursor(0, 0);
  display.println("Place finger again...");
  display.println("C: Cancel");
  display.display();

  while (p != FINGERPRINT_OK) {
    p = finger.getImage();
    p = finger.image2Tz(2);
    switch (p) {
    case FINGERPRINT_OK:
      Serial.println("Image taken");
      break;
    }
    if(!digitalRead(BUTTON_C)){
      return -1;
    }
    ArduinoCloud.update();
  }


  // OK converted!
  Serial.print("Creating model for #");  Serial.println(id);
  
  // Display Status update
  display.clearDisplay();
  display.setCursor(0, 0);
  display.println("Verifying. . .");
  display.display();
  delay(1000);

  p = finger.createModel();
  if (p == FINGERPRINT_OK) {
    Serial.println("Prints matched!");

  }
  else {
    Serial.println("Error: Model could not be created");
    
    // Display Status update
    display.clearDisplay();
    display.setCursor(0, 0);
    display.println("Error: \nFingerprints did not match");
    display.println("Returning... \nPlease try again.");
    display.display();
    delay(3000);


    return p;
  }

  if(user == "Please enter name"){
    Serial.println("Error: No name was entered");
    
    // Display Status update
    display.clearDisplay();
    display.setCursor(0, 0);
    display.println("Error: \nNo name was entered");
    display.println("Returning... \nPlease try again.");
    display.display();
    delay(3000);

    return p;
  }


  int dupeCheck = finger.fingerSearch();
  if (dupeCheck == FINGERPRINT_OK) {
    Serial.println("Found a print match!");
    
    // Display Status update
    display.clearDisplay();
    display.setCursor(0, 0);
    display.println("Fingerprint already exists.");
    display.println("Replacing previous name. . .");
    display.display();
    delay(2000);

    return dupeCheck;
  }

  Serial.print("ID "); Serial.println(id);
  p = finger.storeModel(id);
  
  // Display Status update
  display.clearDisplay();
  display.setCursor(0, 0);
  display.println("Saving. . .");
  display.display();
  delay(1000);

  if (p == FINGERPRINT_OK) {
    Serial.println("Stored!");
    
    // Display Status update
    display.clearDisplay();
    display.setCursor(0, 0);
    display.println("Successfully Registered...");
    display.println("Ending registration...");
    display.display();
    delay(3000);

  } else {
    Serial.println("Error: Could not store fingerprint");
    
    // Display Status update
    display.clearDisplay();
    display.setCursor(0, 0);
    display.println("Error while saving...");
    display.println("Returning... Please try again.");
    display.display();
    delay(3000);

    return p;
  }

  return true;
}
