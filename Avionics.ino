

/*    
  Code for NTU SEDS Project New Dawn High Altitude Balloon
  Created by the NTU SEDS Avionics Team
  
  Connections
  ===========
  Connect SCL to analog 5
  Connect SDA to analog 4
  Connect VDD to 3.3-5V DC
  Connect GROUND to common ground


  Connect SCK to Pin 13
  Connect MISO to Pin 12
  Connect MOSI to Pin 11
  Connect SS to Pin 4
   
 */
#include <calculate_altitude.h>
#include <Wire.h>
#include <SD.h>
#include <Adafruit_Sensor.h>
#include <Adafruit_BNO055.h>
#include <MS5xxx.h>
#include <stdint.h>

/* Set the delay between fresh samples */
#define SAMPLERATE_DELAY_MS (1000)
#define TELEMETRY_DELAY_MS (5000)

// Check I2C device address and correct line below (by default address is 0x29 or 0x28)
//                                   id, address
Adafruit_BNO055 bno = Adafruit_BNO055(55, 0x28);

MS5xxx sensor(&Wire);
Sd2Card card;
SdVolume volume;
SdFile root;
// change this to match your SD shield or module;
// Arduino Ethernet shield: pin 4
// Adafruit SD shields and modules: pin 10
// Sparkfun SD shield: pin 8
const int chipSelect = 4;
File log_for_ms5607;
File log_for_bno055;

void initialize_bno055(void) {
  /* Initialise the sensor */
  if(!bno.begin())
  {
    /* There was a problem detecting the BNO055 ... check your connections */
    Serial.print("Ooops, no BNO055 detected ... Check your wiring or I2C ADDR!");
    while(1);
  }
  delay(1000);

  /* Use external crystal for better accuracy */
  bno.setExtCrystalUse(true);
  Serial.print("BNO Initialized.");
}

void initialize_ms5607(void) {
  if(sensor.connect() > 0) {
    Serial.println("Error connecting to the MS5607 board!");
    while(1);
  }
}

void readFrom_bno055(int is_telemtry) {
  sensors_event_t event;
  bno.getEvent(&event);
  if (is_telemtry){
    /* TODO */
    Serial.println("Telemtry for BNO055");
  } else {
    /* TODO: Check with Payloads team how they want their telemetry log formatted */
    log_for_bno055 = SD.open("bno055.txt", FILE_WRITE);
    if (log_for_bno055) {
      log_for_bno055.print("Time: ");
      log_for_bno055.println(millis());
      log_for_bno055.print(F("Orientation: "));
      log_for_bno055.print((float)event.orientation.x);
      log_for_bno055.print(F(" "));
      log_for_bno055.print((float)event.orientation.y);
      log_for_bno055.print(F(" "));
      log_for_bno055.print((float)event.orientation.z);
      log_for_bno055.println(F(""));    
  
  
      // Also send calibrationdata for each sensor.
      uint8_t sys, gyro, accel, mag = 0;
      bno.getCalibration(&sys, &gyro, &accel, &mag);
    
      log_for_bno055.print(F("Calibration: "));
      log_for_bno055.print(sys, DEC);
      log_for_bno055.print(F(" "));
      log_for_bno055.print(gyro, DEC);
      log_for_bno055.print(F(" "));
      log_for_bno055.print(accel, DEC);
      log_for_bno055.print(F(" "));
      log_for_bno055.println(mag, DEC);
      log_for_bno055.println("---");
      log_for_bno055.close();  
    } else {
      // if the file didn't open, print an error:
      Serial.println("error bno055.txt");
    }
    /* The processing sketch expects data as roll, pitch, heading */

  }
}

void readFrom_ms5607(int is_telemtry) {
  sensor.ReadProm();
  sensor.Readout();
  if(is_telemtry){
    Serial.println("Telemetry for MS5607");
  } else {
    log_for_ms5607 = SD.open("ms5607.txt", FILE_WRITE);
    if (log_for_ms5607){
      // TODO: Check with Payloads team how they want their telemetry log formatted
      log_for_ms5607.print("Time: ");
      log_for_ms5607.println(millis());
      log_for_ms5607.print("Temperature [0.01 C]: ");
      log_for_ms5607.println(sensor.GetTemp());
      log_for_ms5607.print("Pressure [Pa]: ");
      log_for_ms5607.println(sensor.GetPres());
      // I can choose to test the CRC
      log_for_ms5607.print("Altitude (cal.) in meters: ");
      log_for_ms5607.println(calc_geopotential_alt((uint32_t) sensor.GetPres(), sensor.GetTemp()));
      log_for_ms5607.println("---");
      log_for_ms5607.close();
    } else {
      // if the file didn't open, print an error:
      Serial.println("error ms5607.txt");
    }
  }
}

void setup() {
  // put your setup code here, to run once:
  Serial.begin(115200);
  Serial.println("Beginning setup");
  if (!SD.begin(4)) {
    Serial.println("initialization of SD card failed!");
    while (1);
  }
  if (SD.exists("ms5607.txt")){
    SD.remove("ms5607.txt");
  }
  if (SD.exists("bno055.txt")){
    SD.remove("bno055.txt");
  }
  initialize_bno055();
  initialize_ms5607();

}

unsigned long lastReadTime_ms5607;
unsigned long lastTelemtryTime_ms5607;
unsigned long lastReadTime_bno055;
unsigned long lastTelemtryTime_bno055;

void loop() {
  // put your main code here, to run repeatedly: 
  if((unsigned long)(millis() - lastReadTime_ms5607) > SAMPLERATE_DELAY_MS){
    Serial.print("lastReadTime_ms5607: ");
    Serial.println(lastReadTime_ms5607);
    readFrom_ms5607(0);
    lastReadTime_ms5607 = millis();
  }
  if((unsigned long)(millis() - lastReadTime_bno055) > SAMPLERATE_DELAY_MS){
    Serial.print("lastReadTime_bno055: ");
    Serial.println(lastReadTime_bno055);
    readFrom_bno055(0);
    lastReadTime_bno055 = millis();
  }
  if((unsigned long)(millis() - lastTelemtryTime_ms5607) > TELEMETRY_DELAY_MS){
    Serial.print("lastReadTime_ms5607: ");
    Serial.println(lastReadTime_ms5607);
    readFrom_ms5607(1);
    lastTelemtryTime_ms5607 = millis();
  }
  if((unsigned long)(millis() - lastTelemtryTime_bno055) > TELEMETRY_DELAY_MS){
    Serial.print("lastTelemtryTime_bno055: ");
    Serial.println(lastTelemtryTime_bno055);
    readFrom_bno055(1);
    lastTelemtryTime_bno055 = millis();
  }

}
