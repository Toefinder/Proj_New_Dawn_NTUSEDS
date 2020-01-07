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
 */


#include "sensors_proj_new_dawn.h"

// Check I2C device address and correct line below (by default address is 0x29 or 0x28)
//                                   id, address
Adafruit_BNO055 bno = Adafruit_BNO055(55, 0x28);
MS5xxx sensor(&Wire);

// -1 means error with initialization
extern int initialize_bno055(void) {
  /* Initialise the sensor */
  if(!bno.begin()){
    return -1;
  }
  delay(1000);
  /* Use external crystal for better accuracy */
  bno.setExtCrystalUse(true);
  return 0;
}

// -1 means error with initialization
extern int initialize_ms5607(void) {
  if(sensor.connect() > 0) {
    return -1;
  }
  return 0;
}

void readFrom_bno055(struct bno055_data *bno055_data_struct) {
  sensors_event_t event;
  bno.getEvent(&event);
  uint8_t sys, gyro, accel, mag = 0;
  bno.getCalibration(&sys, &gyro, &accel, &mag);
  bno055_data_struct->orientation_x = (float)event.orientation.x;
  bno055_data_struct->orientation_y = (float)event.orientation.y;
  bno055_data_struct->orientation_z = (float)event.orientation.z;
  bno055_data_struct->internal_temp = (uint8_t) bno.getTemp();
#ifdef DEBUG_SENSORS
  Serial.print("Reading from bno055: x=");
  Serial.print(bno055_data_struct->orientation_x);
  Serial.print(", y=");
  Serial.print(bno055_data_struct->orientation_y);
  Serial.print(", z=");
  Serial.print(bno055_data_struct->orientation_z);
  Serial.print(", t_int=");
  Serial.println(bno055_data_struct->internal_temp);
#endif
  /* TODO: Find out what other data we can populate */
  return;
}

void readFrom_ms5607(struct ms5607_data *ms5607_data_struct) {
  sensor.ReadProm();
  sensor.Readout();
  ms5607_data_struct->temperature = sensor.GetTemp() / 100;
  ms5607_data_struct->pressure = sensor.GetPres();
  ms5607_data_struct->altitude = calc_geopotential_alt((uint32_t)ms5607_data_struct->pressure);
#ifdef DEBUG_SENSORS
  Serial.print("Reading from ms5607: t_ext=");
  Serial.print(ms5607_data_struct->temperature);
  Serial.print(", Pa=");
  Serial.print(ms5607_data_struct->pressure);
  Serial.print(", alt=");
  Serial.println(ms5607_data_struct->altitude);
#endif
  return;
}
