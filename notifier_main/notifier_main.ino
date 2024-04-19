/* 
  ELEG/CPEG398 Final Group Project - The Load N' Told Laundry Notification Device

  Purpose: Notify users of who's laundry is ongoing and has finished via push notifications on their mobile device

  Arduino IoT Cloud Variables description:
  String message; Contains messages sent from the remote. Can also be used to send message to the remote from the Arduino Device
  bool isFinishedLoad; Boolean value depicting when a load is finished

*/

// Include for ArduinoIoT Cloud
#include "thingProperties.h"

// Import OLED
#include "OLED_Display.h"


void setup() {
  // Initialize serial and wait for port to open:
  Serial.begin(115200);
  // This delay gives the chance to wait for a Serial Monitor without blocking if none is found
  delay(1500); 

  // Defined in thingProperties.h
  initProperties();

  // Connect to Arduino IoT Cloud
  ArduinoCloud.begin(ArduinoIoTPreferredConnection);
  
  /*
     The following function allows you to obtain more information
     related to the state of network and IoT Cloud connection and errors
     the higher number the more granular information you’ll get.
     The default is 0 (only errors).
     Maximum is 4
 */
  setDebugMessageLevel(2);
  ArduinoCloud.printDebugInfo();


  
  // Set up OLED display
  setup_OLED();
}

String user = "";
bool intakeResponse = false;
bool loadInProgress = false;
void printMessageFeed(String s){
  message = s;
}

void takeResponse(String prompt){
  if(prompt != ""){
    Serial.println(prompt);
    printMessageFeed(prompt);
  }
  intakeResponse = true;
  delay(500);
}

void loop() {
  OLED_loop();
  ArduinoCloud.update();
  
  switch(currentState){
    case MAIN_MENU:
      break;
    case START_MENU: 
      state_START_MENU();
      break;
    default:
      Serial.println("Error: Undefined State");
  }

  if(!digitalRead(BUTTON_A)){
    takeResponse("What's your name?");
  }
  delay(10);
}

void state_START_MENU(){
  // Wait for load (Look for Vibrations)

  // if load detected, display that a load is in progress
  if(loadInProgress){
    // If load isn't claimed, prompt a button to claim load
    if(user == ""){
      displayStartMenu(true,loadInProgress,""); // Parameters: isUnclaimed, loadInProgress
      // Button sends user to finger print state
    }
    else{
      displayStartMenu(true,loadInProgress,user);
    }
  }
  else{ // Display "Waiting for Load"
    displayStartMenu(false,loadInProgress,""); // Parameters: isUnclaimed, loadInProgress
  }
}


void onMessageChange()  {
  // Add your code here to act upon Message change
  if(intakeResponse){
    intakeResponse = false;
    user = message; // Save message to user's name
    Serial.println("Response from user: " + message); // debugging
  }
}