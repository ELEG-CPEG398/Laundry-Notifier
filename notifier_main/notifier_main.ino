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

// Import Vibration Sensor
#include "Vibration_Sensor.h"

// Constant Variables
short booleanAverage = 0;
short booleanSamples = 0;

void setup() {
  // Initialize serial and wait for port to open:
  Serial.begin(115200);
  while (!Serial)
    delay(10); // will pause Zero, Leonardo, etc until serial console opens

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
  clk += 5;
  if((clk % 5000) == 0)
    clk = 0;
  
  delay(5);
}

void state_START_MENU(){
  // Wait for load (Look for Vibrations)
  if((clk % SENSORDELAY) == 0){
    booleanAverage += detectVibration();
    booleanSamples ++;
  }
  else{
    detectVibration();
  }
  
  
  if(loadInProgress){
    Serial.println("Load In Progress");
  }

  if ( (clk % 3200) == 0){
    Serial.println((float)booleanAverage/booleanSamples);
  }

  // Wait 3 seconds before changing changing boolean value
  if(loadInProgress && ((clk % 3000) == 0) && (booleanAverage/booleanSamples > 0.6)){
    loadInProgress = true;
    booleanAverage = 0;
    booleanSamples = 0;
  }
  else if(((clk % 3000) == 0) && (booleanAverage/booleanSamples <= 0.6)){
    loadInProgress = false;
    booleanAverage = 0;
    booleanSamples = 0;
  }

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