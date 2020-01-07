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


#include <Adafruit_BNO055.h>
#include <Adafruit_Sensor.h>
#include <Arduino.h>
#include <Wire.h>
#include <MS5xxx.h>
#include <stdint.h>
#include "calculate_altitude.h"
#include "config.h"

/*
  Connections
  ===========
  Connect SCL to analog 5
  Connect SDA to analog 4
  Connect VDD to 3.3-5V DC
  Connect GROUND to common ground
*/

extern int initialize_bno055(void);
extern int initialize_ms5607(void);
extern void readFrom_bno055(struct bno055_data *bno055_data_struct);
extern void readFrom_ms5607(struct ms5607_data *ms5607_data_struct);

struct bno055_data {
  float orientation_x;
  float orientation_y;
  float orientation_z;
  uint8_t internal_temp;
  /* TODO: What other information can the bno055 hold? */
};

struct ms5607_data {
  float temperature;
  float pressure;
  float altitude;
};
