#ifndef get_fingerprint
#define get_fingerprint

#include <Adafruit_Fingerprint.h>

#if (defined(__AVR__) || defined(ESP8266)) && !defined(__AVR_ATmega2560__)
// For UNO and others without hardware serial, we must use software serial...
// pin #2 is IN from sensor (GREEN wire)
// pin #3 is OUT from arduino  (WHITE wire)
// Set up the serial port to use softwareserial..
  SoftwareSerial mySerial(4, 5);
#else
// On Leonardo/M0/etc, others with hardware serial, use hardware serial!
// #0 is green wire, #1 is white
#define mySerial Serial1
#endif

// Global Variables
Adafruit_Fingerprint finger = Adafruit_Fingerprint(&mySerial);

// Function Definitions
bool get_fingerprint_setup(void);
uint8_t getFingerprintID(void);   
int getFingerprintIDez(void);

#endif