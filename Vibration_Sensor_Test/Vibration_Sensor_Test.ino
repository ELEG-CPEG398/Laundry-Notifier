// Basic demo for accelerometer/gyro readings from Adafruit ISM330DHCX

#include <Adafruit_ISM330DHCX.h>

#define BUTTON_A 0
#define BUTTON_B 16

// For SPI mode, we need a CS pin
#define LSM_CS 10
// For software-SPI mode we need SCK/MOSI/MISO pins
#define LSM_SCK 13
#define LSM_MISO 12
#define LSM_MOSI 11

// Global Variables
bool vibrating = false;
float THRESHOLD = 9.68;
int SAMPLES = 64;
Adafruit_ISM330DHCX ism330dhcx;

void setup(void) {
  Serial.begin(115200);
  while (!Serial)
    delay(10); // will pause Zero, Leonardo, etc until serial console opens

  Serial.println("Adafruit ISM330DHCX Vibration Test!");

  if (!ism330dhcx.begin_I2C()) {
    // if (!ism330dhcx.begin_SPI(LSM_CS)) {
    // if (!ism330dhcx.begin_SPI(LSM_CS, LSM_SCK, LSM_MISO, LSM_MOSI)) {
    Serial.println("Failed to find ISM330DHCX chip");
    while (1) {
      delay(10);
    }
  }

  Serial.println("ISM330DHCX Found!");

  //ism330dhcx.setAccelRange(LSM6DS_ACCEL_RANGE_4_G); // (+-4G Default)
  Serial.print("Accelerometer range set to: +-4G");

  //ism330dhcx.setGyroRange(LSM6DS_GYRO_RANGE_250_DPS); // (2000 degrees Default)
  Serial.print("Gyro range set to: 2000 degrees/s");

  ism330dhcx.setAccelDataRate(LSM6DS_RATE_416_HZ);
  Serial.print("Accelerometer data rate set to: 416 Hz");

  //ism330dhcx.setGyroDataRate(LSM6DS_RATE_12_5_HZ); // (104 Hz Default)
  Serial.print("Gyro data rate set to: 104 Hz");
  
  ism330dhcx.configInt1(false, false, true); // accelerometer DRDY on INT1
  ism330dhcx.configInt2(false, true, false); // gyro DRDY on INT2
}

// Rms value of the acceleration
void loop() {
  double sum = 0;
  double sumSq = 0;

  // Declare Sensor event
  sensors_event_t accel;
  sensors_event_t gyro;
  sensors_event_t temp;
  
  // Collect sample data and average magnitude
  for (int i = 0; i < SAMPLES; i++) {
    ism330dhcx.getEvent(&accel, &gyro, &temp);
    
    double magnitude =      (accel.acceleration.x * accel.acceleration.x) +
                            (accel.acceleration.y * accel.acceleration.y) +
                            (accel.acceleration.z * accel.acceleration.z);
    sum += magnitude;
    sumSq += magnitude * magnitude;
    
    delay(5);
  }
  
  // Calculate rms
  double mean = sum / SAMPLES;
  double variance = (sumSq / SAMPLES) - (mean * mean);
  double stdDev = sqrt(variance);

  // Calibrate by setting threshold
  if(!digitalRead(BUTTON_A)){
    delay(2000);
    THRESHOLD = stdDev * 1.5;
  }
  
  vibrating = stdDev > THRESHOLD;

  //serial plotter friendly format
  Serial.print(accel.acceleration.x);
  Serial.print(","); Serial.print(accel.acceleration.y);
  Serial.print(","); Serial.print(accel.acceleration.z);
  Serial.print(","); Serial.print(THRESHOLD);
  Serial.print(","); Serial.print(stdDev);
  Serial.print(","); Serial.println(vibrating);

  //delayMicroseconds(10000);
}
