#include "Vibration_Sensor.h"

void VibrationSensorSetup(void) {
  if (!ism330dhcx.begin_I2C()) {
    // if (!ism330dhcx.begin_SPI(LSM_CS)) {
    // if (!ism330dhcx.begin_SPI(LSM_CS, LSM_SCK, LSM_MISO, LSM_MOSI)) {
    Serial.println("Failed to find ISM330DHCX chip");
    while (1) {
      delay(10);
    }
  }
  
  Serial.println("ISM330DHCX Found!");

  ism330dhcx.setAccelRange(LSM6DS_ACCEL_RANGE_4_G); // (+-4G Default)
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

// Detects vibrations by comparing the current standard deviation of the magnitude of acceleration to its "natural" state
bool detectVibration(void) {
  // Declare Sensor event
  sensors_event_t accel;
  sensors_event_t gyro;
  sensors_event_t temp;
  
  // Collect sample data and average magnitude
  //for (int i = 0; i < SAMPLES; i++) {
    ism330dhcx.getEvent(&accel, &gyro, &temp);
    
    double magnitude =      (accel.acceleration.x * accel.acceleration.x) +
                            (accel.acceleration.y * accel.acceleration.y) +
                            (accel.acceleration.z * accel.acceleration.z);
    sum += magnitude;
    sumSq += magnitude * magnitude;
    //delay(5); // Delay needed for sample rate of 416 Hz
  //}
  

  // Calibrate by setting threshold
  //if(!digitalRead(BUTTON_A)){
    //delay(2000);
    //THRESHOLD = stdDev * 1.5;
  //}
  
  // Calculate standard deviation every (5ms) * (# of samples)
  if((clk % SENSORDELAY) == 0){
    return calculateStdDev();
  }
  else{
    return false;
  }
 
}

bool calculateStdDev(){
  // Calculate Standard Deviation
  double mean = sum / (float)SAMPLES;
  double variance = (sumSq / (float)SAMPLES) - (mean * mean);
  double stdDev = sqrt(variance);
  sum = 0;
  sumSq = 0;
  
  //Serial.print(stdDev);
  //Serial.print(", ");
  //Serial.println(THRESHOLD);
 
  Serial.print(THRESHOLD);
  Serial.print(","); Serial.print(stdDev);
  Serial.print(","); Serial.print(test);
  Serial.print(","); Serial.println(stdDev > THRESHOLD);


  return  stdDev > THRESHOLD;
}
