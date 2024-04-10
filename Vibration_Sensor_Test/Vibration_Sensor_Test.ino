#include <Adafruit_ISM330DHCX.h>
#include "arduinoFFT.h"

// For SPI mode, we need a CS pin
#define LSM_CS 10
// For software-SPI mode we need SCK/MOSI/MISO pins
#define LSM_SCK 13
#define LSM_MISO 12
#define LSM_MOSI 11

// FFT Constants
const uint16_t samples = 64; //(Default: 64) This value MUST ALWAYS be a power of 2
const double signalFrequency = 1000;
const double samplingFrequency = 5000; // (Default: 5000)
const uint8_t amplitude = 100;
const double ratio = twoPi * signalFrequency / samplingFrequency; // Fraction of a complete cycle stored at each sample (in radians)
  
/*
These are the input and output vectors
Input vectors receive computed results from FFT
*/
double vReal[samples];
double vImag[samples];

/* Create FFT object */
ArduinoFFT<double> FFT = ArduinoFFT<double>(vReal, vImag, samples, samplingFrequency);

// FFT Definitions
#define SCL_INDEX 0x00
#define SCL_TIME 0x01
#define SCL_FREQUENCY 0x02
#define SCL_PLOT 0x03

Adafruit_ISM330DHCX ism330dhcx;
void setup(void) {
  Serial.begin(115200);
  while (!Serial)
    delay(10); // will pause Zero, Leonardo, etc until serial console opens

  Serial.println("Ready to go!");

  if (!ism330dhcx.begin_I2C()) {
    Serial.println("Failed to find ISM330DHCX chip");
    while (1) {
      delay(10);
    }
  }

  Serial.println("ISM330DHCX Found!");

  // ism330dhcx.setAccelRange(LSM6DS_ACCEL_RANGE_2_G);
  Serial.print("Accelerometer range set to: ");
  switch (ism330dhcx.getAccelRange()) {
  case LSM6DS_ACCEL_RANGE_2_G:
    Serial.println("+-2G");
    break;
  case LSM6DS_ACCEL_RANGE_4_G:
    Serial.println("+-4G");
    break;
  case LSM6DS_ACCEL_RANGE_8_G:
    Serial.println("+-8G");
    break;
  case LSM6DS_ACCEL_RANGE_16_G:
    Serial.println("+-16G");
    break;
  }


  // ism330dhcx.setAccelDataRate(LSM6DS_RATE_12_5_HZ);
  Serial.print("Accelerometer data rate set to: ");
  switch (ism330dhcx.getAccelDataRate()) {
  case LSM6DS_RATE_SHUTDOWN:
    Serial.println("0 Hz");
    break;
  case LSM6DS_RATE_12_5_HZ:
    Serial.println("12.5 Hz");
    break;
  case LSM6DS_RATE_26_HZ:
    Serial.println("26 Hz");
    break;
  case LSM6DS_RATE_52_HZ:
    Serial.println("52 Hz");
    break;
  case LSM6DS_RATE_104_HZ:
    Serial.println("104 Hz");
    break;
  case LSM6DS_RATE_208_HZ:
    Serial.println("208 Hz");
    break;
  case LSM6DS_RATE_416_HZ:
    Serial.println("416 Hz");
    break;
  case LSM6DS_RATE_833_HZ:
    Serial.println("833 Hz");
    break;
  case LSM6DS_RATE_1_66K_HZ:
    Serial.println("1.66 KHz");
    break;
  case LSM6DS_RATE_3_33K_HZ:
    Serial.println("3.33 KHz");
    break;
  case LSM6DS_RATE_6_66K_HZ:
    Serial.println("6.66 KHz");
    break;
  }


  ism330dhcx.configInt1(false, false, true); // accelerometer DRDY on INT1
  ism330dhcx.configInt2(false, true, false); // gyro DRDY on INT2
}

void PrintVector(double *vData, uint16_t bufferSize, uint8_t scaleType)
{
  for (uint16_t i = 0; i < bufferSize; i++)
  {
    double abscissa;
    /* Print abscissa value */
    switch (scaleType)
    {
      case SCL_INDEX:
        abscissa = (i * 1.0);
	      break;
      case SCL_TIME:
        abscissa = ((i * 1.0) / samplingFrequency);
        break;
      case SCL_FREQUENCY:
        abscissa = ((i * 1.0 * samplingFrequency) / samples);
	      break;
    }
    if(scaleType!=SCL_PLOT)
    {
      Serial.print(abscissa, 6);
      if(scaleType==SCL_FREQUENCY)
        Serial.print("Hz");
      Serial.print(" ");
    }
    Serial.println(vData[i], 4);
  }
  Serial.println();
}


// Returns magnitude of 3D vector
double vectorMagnitude(double x, double y, double z){
  return sqrt(sq(x)+sq(y)+sq(z));
}

void readAcceleration(uint16_t iter, double magnitude){
  vReal[iter] = int8_t(amplitude * magnitude);/* Build data with positive and negative values*/
  //vReal[iter] = int8_t(amplitude * sin(iter * ratio) / 2.0); // Test Sample
  vImag[iter] = 0.0; //Imaginary part must be zeroed in case of looping to avoid wrong calculations and overflows
}

// Returns major peak
double calculateFFT(){
  FFT.windowing(FFTWindow::Hamming, FFTDirection::Forward);	/* Weigh data */
  FFT.compute(FFTDirection::Forward); /* Compute FFT */
  FFT.complexToMagnitude(); /* Compute magnitudes */
  PrintVector(vReal, samples>>1, SCL_PLOT);
  return FFT.majorPeak();
}

void loop() {
  sensors_event_t accel;
  sensors_event_t gyro;
  sensors_event_t temp;
  for (uint16_t i = 0; i < samples; i++)
  {
    //  /* Get a new normalized sensor event */
    
    ism330dhcx.getEvent(&accel, &gyro, &temp);
    readAcceleration(i, vectorMagnitude(accel.acceleration.x,accel.acceleration.y,accel.acceleration.z));
    delayMicroseconds(10000);
  }
  double maxFreq = calculateFFT();
  Serial.print("Highest Frequency: ");
  Serial.println(maxFreq,6);
  delay(3000);
  if(false){

    
    // serial plotter friendly format

      Serial.print(accel.acceleration.x);
      Serial.print(","); Serial.print(accel.acceleration.y);
      Serial.print(","); Serial.print(accel.acceleration.z);
      Serial.print(",");
      Serial.println(ism330dhcx.shake());

      delayMicroseconds(10000);
  }
}
