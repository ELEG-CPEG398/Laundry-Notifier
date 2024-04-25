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
const short MINIMUMLOADCYCLETIME = 60; // (Default: 60 seconds)

// Global Variables
short booleanSum = 0;
short booleanSamples = 0;
short loadTime = 0;
String user = "";
bool intakeResponse = false;
bool loadInProgress = false;

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

  // Set up Vibration Sensor
  VibrationSensorSetup();
}


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
  clk += SAMPLERATE;
  if(clk > 32000)
    clk = 0;

  

  
  delay(SAMPLERATE);
}

void state_START_MENU(){
  // Wait for load (Look for Vibrations)
  if((clk % SENSORDELAY) == 0){
    booleanSum += detectVibration();
    booleanSamples ++;
  }
  else{
    detectVibration();
  }
  
 
  if(clk % 1000)
    loadTime++;
  


  if ( (clk % SENSORDELAY) == 0){
    Serial.println(((float)booleanSum)/((float)booleanSamples));
  }

  // Wait 1 second before attempting to change boolean value
  if((clk % 1000) == 0){
    float booleanAverage = (float)booleanSum/((float)booleanSamples);
    // If a load isn't in progress (false) and the sensor is detecting vibrations (booleanAverage is true), set loadInProgress to true
    if(!loadInProgress  && ( booleanAverage >= 0.5)){
      loadInProgress = true;
      booleanSum = 0;
      booleanSamples = 0;
      isFinishedLoad = false;
    }
    // If a load is in progress and the sensor is not detecting vibration (booleanAverage is false), set loadInProgress to false
    else if(booleanAverage < 0.5){
      loadInProgress = false;
      booleanSum = 0;
      booleanSamples = 0;
      isFinishedLoad = loadTime > MINIMUMLOADCYCLETIME;
    }
    // if a load is in progress and the sensor continues to detect vibrations (booleanAverage is true), reset the booleanAverage
    else{
      booleanSum = 0;
      booleanSamples = 0;
    }
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