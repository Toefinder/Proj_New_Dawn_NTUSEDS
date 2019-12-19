

/*    
  Code for NTU SEDS Project New Dawn High Altitude Balloon
  Created by the NTU SEDS Avionics Team
  
  Connections
  ===========
  Connect SCL to analog 5
  Connect SDA to analog 4
  Connect VDD to 3.3-5V DC
  Connect GROUND to common ground
   
 */
#include <calculate_altitude.h>
#include <Wire.h>
#include <Adafruit_Sensor.h>
#include <Adafruit_BNO055.h>
#include <MS5xxx.h>
#include <stdint.h>

/* Set the delay between fresh samples */
#define SAMPLERATE_DELAY_MS (1000)

// Check I2C device address and correct line below (by default address is 0x29 or 0x28)
//                                   id, address
Adafruit_BNO055 bno = Adafruit_BNO055(55, 0x28);

MS5xxx sensor(&Wire);

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

void readFrom_bno055(void) {
  sensors_event_t event;
  bno.getEvent(&event);
  /* The processing sketch expects data as roll, pitch, heading */
  Serial.print(F("Orientation: "));
  Serial.print((float)event.orientation.x);
  Serial.print(F(" "));
  Serial.print((float)event.orientation.y);
  Serial.print(F(" "));
  Serial.print((float)event.orientation.z);
  Serial.println(F(""));

  /* Also send calibration data for each sensor. */
  uint8_t sys, gyro, accel, mag = 0;
  bno.getCalibration(&sys, &gyro, &accel, &mag);
  Serial.print(F("Calibration: "));
  Serial.print(sys, DEC);
  Serial.print(F(" "));
  Serial.print(gyro, DEC);
  Serial.print(F(" "));
  Serial.print(accel, DEC);
  Serial.print(F(" "));
  Serial.println(mag, DEC);
  Serial.println("---");
}

void readFrom_ms5607(void) {
  sensor.ReadProm();
  sensor.Readout();
  Serial.print("Temperature [0.01 C]: ");
  Serial.println(sensor.GetTemp());
  Serial.print("Pressure [Pa]: ");
  Serial.println(sensor.GetPres());
  /* I can choose to test the CRC */
  Serial.print("Altitude (cal.) in meters: ");
  Serial.println(calc_geopotential_alt((uint32_t) sensor.GetPres(), sensor.GetTemp()));
  Serial.println("---");
}

void setup() {
  // put your setup code here, to run once:
  Serial.begin(115200);
  Serial.println("Beginning setup");
  initialize_bno055();
  initialize_ms5607();

}

void loop() {
  // put your main code here, to run repeatedly:
  readFrom_ms5607();
  delay(SAMPLERATE_DELAY_MS);
  readFrom_bno055();
  delay(SAMPLERATE_DELAY_MS);

}
