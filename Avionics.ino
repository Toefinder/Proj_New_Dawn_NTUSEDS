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
  
  Connections for MS5607 and BNO055 
  ==================================
  Connect SCL to analog 5
  Connect SDA to analog 4
  Connect VDD to 3.3-5V DC
  Connect GROUND to common ground

  Connections for SD card reader
  ===============================
  Connect SCK to Pin 13
  Connect MISO to Pin 12
  Connect MOSI to Pin 11
  Connect SS to Pin 4
   
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
char str_for_log_and_telemetry[100];

struct bno055_data bno055_data_struct;
struct ms5607_data ms5607_data_struct;
// TODO: Include a data structure for GPS time and coordinates

// TODO: Any error checking needed?
extern int write_to_file(const char *filename_to_write_to, const char *str_to_write){
    File file_to_write_to = SD.open(filename_to_write_to);
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

extern int initialize_sdcard(void){
  if (!SD.begin(2)) {
#ifdef DEBUG_MAIN
    Serial.println("initialization of SD card failed!");
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
    sprintf(str_for_log_and_telemetry, "%f,%f,%f,%d,%f,%f,%f", 
      bno055_data_struct.orientation_x,
      bno055_data_struct.orientation_y,
      bno055_data_struct.orientation_z,
      bno055_data_struct.internal_temp,
      ms5607_data_struct.temperature,
      ms5607_data_struct.pressure,
      ms5607_data_struct.altitude
    );
#ifdef DEBUG_MAIN
    Serial.println(str_for_log_and_telemetry);
#endif
    write_to_file("log_file.txt", str_for_log_and_telemetry);
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
