
#include "get_fingerprint.h"
#include "OLED_Display.h"



uint8_t getFingerprintEnroll() {
  int p = -1;

  finger.getTemplateCount();
  //uint8_t
  int id = finger.templateCount; 
  Serial.print("Waiting for valid finger to enroll as #"); Serial.println(++id);
  
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

  // Error: A Name should be entered beforehand
  if(user == "Please enter name"){
    Serial.println("Error: No name was entered");
    
    // Display Status update
    display.clearDisplay();
    display.setCursor(0, 0);
    display.println("Error: \nNo valid name was entered");
    display.println("Returning... \nPlease try again.");
    display.display();
    delay(3000);

    return p;
  }

  // If fingerprint already exist, change name of identifier
  int dupeCheck = finger.fingerSearch();
  if (dupeCheck == FINGERPRINT_OK) {
    Serial.println("Found a print match!");
    
    // Display Status update
    display.clearDisplay();
    display.setCursor(0, 0);
    display.println("F.Print already exists");
    display.println("Previous name:");
    display.println(registered_users[String(finger.fingerID)].as<String>());
    display.println("New name:");
    display.println(user);
    display.display();
    delay(5000);

    registered_users[String(finger.fingerID)] = user;

    return dupeCheck;
  }

  // Store new user info
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
    registered_users[String(id)] = user; // Must be succesfully saved to fingerprint sensor before saving to json file
    
    // Display Status update
    display.clearDisplay();
    display.setCursor(0, 0);
    display.println("Successfully\nRegistered");
    display.println("Ending...");
    display.display();
    delay(3000);

  } else {
    Serial.println("Error: Could not store fingerprint");
    
    // Display Status update
    display.clearDisplay();
    display.setCursor(0, 0);
    display.println("Error while saving...");
    display.println("Returning...\nPlease try again.");
    display.display();
    delay(3000);

    return p;
  }

  return true;
}
