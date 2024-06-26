#ifndef Vibration_Sensor
#define Vibration_Sensor

#include <Adafruit_ISM330DHCX.h>

// For SPI mode, we need a CS pin
#define LSM_CS 10
// For software-SPI mode we need SCK/MOSI/MISO pins
#define LSM_SCK 13
#define LSM_MISO 12
#define LSM_MOSI 11

// Constant variables
const int SAMPLES = 64;
const int SAMPLERATE = 5; // 5ms or ~416Hz 
const int SENSORDELAY  = SAMPLES * SAMPLERATE;

// Global Variables
int test = 0;
float THRESHOLD = 1.0;  // 1 default
int clk = 0;
double sum = 0;
double sumSq = 0;
Adafruit_ISM330DHCX ism330dhcx;

// Function Declarations
bool VibrationSensorSetup(void);
bool detectVibration(void);
void calibrateSensor(void);


#endif