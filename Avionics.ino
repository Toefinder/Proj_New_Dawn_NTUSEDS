/* Proj New Dawn copyright (C) 2020 Tan WJ Solomon
 *
 * This program is free software; you can redistribute it and/or
 * modify it under the terms of the GNU General Public License
 * as published by the Free Software Foundation; either version 2
 * of the License, or (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, write to the Free Software
 * Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston, MA  02110-1301, USA.
  
  Connections for MS5607 and BNO055 (Uno)
  ==================================
  Connect SCL to analog 5
  Connect SDA to analog 4
  Connect VDD to 3.3-5V DC
  Connect GROUND to common ground

  Connections for SD card reader (Uno)
  ===============================
  Connect SCK to Pin 13
  Connect MISO to Pin 12
  Connect MOSI to Pin 11
  Connect SS to Pin 4

  Connections for MS5607 and BNO055 (Mega)
  ==================================
  Connect SCL to analog 21
  Connect SDA to analog 20
  Connect VDD to 3.3-5V DC
  Connect GROUND to common ground

  Connections for SD card reader (Mega)
  ===============================
  Connect SCK to Pin 52
  Connect MISO to Pin 50
  Connect MOSI to Pin 51
  Connect SS to Pin 53
   
 */

#include "sensors_proj_new_dawn.h"
#include <stdint.h>
#include "config.h"
#include "aprs.h"
#include <SPI.h>
#include <SD.h>
#include <EEPROM.h>

/* Set the delay between fresh samples */
#define SAMPLERATE_DELAY_MS (1000)
#define TELEMETRY_DELAY_MS (5000)

unsigned long lastLogTime;
unsigned long lastTelemtryTime;
String str_for_log_and_telemetry;

struct bno055_data bno055_data_struct;
struct ms5607_data ms5607_data_struct;
// TODO: Include a data structure for GPS time and coordinates

// TODO: Any error checking needed?
inline int write_to_file(const char *filename_to_write_to, const char *str_to_write){
    File file_to_write_to = SD.open(filename_to_write_to, FILE_WRITE);
    if (!file_to_write_to){
#ifdef DEBUG_MAIN
      Serial.print("Failed to open file: ");
      Serial.println(filename_to_write_to);
#endif
      return -1;
    }
    file_to_write_to.print(str_to_write);
    file_to_write_to.close();
    return 0;
}

inline int initialize_sdcard(void){
  pinMode(53, OUTPUT);
  //digitalWrite(53, HIGH);
  if(!SD.begin()){
#ifdef DEBUG_MAIN
    Serial.println("Failed to initialize SD card.");
#endif
    return -1;
  }
  // Store the number of computer resets on the EEPROM
  write_to_file("log_file.txt","orien_x,orien_y,orien_z,T_int,T_ext,Pa,alt");
  return 0;
}

void setup() {
  // put your setup code here, to run once:
  Serial.begin(GPS_BAUDRATE);
#ifdef DEBUG_MAIN
  Serial.println("Beginning setup. ");
#endif
  while(initialize_sdcard() == -1){
    // Problem with initializing SD card
  }
#ifdef DEBUG_MAIN
  Serial.println("Finished setting up SD card.");
#endif
  while(initialize_bno055() == -1){
    // Problem with initializing BNO055

  }
#ifdef DEBUG_MAIN
  Serial.println("Finished setting up BNO055 card.");
#endif
  while(initialize_ms5607() == -1){
    // Problem with initializing MS5607

  }
#ifdef DEBUG_MAIN
  Serial.println("Setup completed. ");
#endif
}

void loop() {
  // put your main code here, to run repeatedly: 
  if((unsigned long)(millis() - lastLogTime) > SAMPLERATE_DELAY_MS){
#ifdef DEBUG_MAIN
  Serial.println("Logging data.");
#endif
  readFrom_ms5607(&ms5607_data_struct);
  readFrom_bno055(&bno055_data_struct);
  // TODO: Get GPS time and coordinates from the LoPy
  // TODO: Process data into a string
/*
  sprintf(str_for_log_and_telemetry, "%d,%d,%d,%d,%d,%d,%d", 
    (int32_t) bno055_data_struct.orientation_x,
    (int32_t) bno055_data_struct.orientation_y,
    (int32_t) bno055_data_struct.orientation_z,
    (int32_t) bno055_data_struct.internal_temp,
    (int32_t) ms5607_data_struct.temperature,
    (int32_t) ms5607_data_struct.pressure,
    (int32_t) ms5607_data_struct.altitude
  );
*/
  str_for_log_and_telemetry = bno055_data_struct.orientation_x;
  str_for_log_and_telemetry += ",";
  str_for_log_and_telemetry += bno055_data_struct.orientation_y;
  str_for_log_and_telemetry += ",";
  str_for_log_and_telemetry += bno055_data_struct.orientation_z;
  str_for_log_and_telemetry += ",";
  str_for_log_and_telemetry += bno055_data_struct.internal_temp;
  str_for_log_and_telemetry += ",";
  str_for_log_and_telemetry += ms5607_data_struct.temperature;
  str_for_log_and_telemetry += ",";
  str_for_log_and_telemetry += ms5607_data_struct.pressure;
  str_for_log_and_telemetry += ",";
  str_for_log_and_telemetry += ms5607_data_struct.altitude;
  str_for_log_and_telemetry += ",";
  
#ifdef DEBUG_MAIN
    Serial.println(str_for_log_and_telemetry);
#endif
    write_to_file("logfile.txt", str_for_log_and_telemetry.c_str());
    lastLogTime = millis();
  }
  if((unsigned long)(millis() - lastTelemtryTime) > TELEMETRY_DELAY_MS){
#ifdef DEBUG_MAIN
    Serial.println("Downlinking data through APRS. ");
#endif
    /*  TODO
     *  Step 1: Get GPS and time data from the LoPy
     *  Step 2: Read the sensors and populate the APRS fields
     *  Step 3: Send out the APRS packet
     */
    readFrom_ms5607(&ms5607_data_struct);
    readFrom_bno055(&bno055_data_struct);
    // TODO: Get GPS time and coordinates from the LoPy
    // TODO: Process data into APRS packet
    //aprs_send();
    lastTelemtryTime = millis();
  }

}
