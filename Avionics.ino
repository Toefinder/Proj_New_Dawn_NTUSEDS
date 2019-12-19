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


#include <Wire.h>
#include <Adafruit_Sensor.h>
#include <Adafruit_BNO055.h>
#include <MS5xxx.h>

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
  Serial.println(BME280_Pa_to_Alt((uint32_t) sensor.GetPres()));
  Serial.println("---");
}

// Convert pressure in Pascals to altitude in millimeters via barometric formula
// input:
//   P - pressure in Pascals
// return: altitude in millimeters
int32_t BME280_Pa_to_Alt(uint32_t P) {
  // The value '101325.0' is 'standard' pressure at mean sea level (MSL) with temperature 15C

  // Hypsometric formula (for altitudes below 11km)
  // h = ((powf(P0 / P,1 / 5.257) - 1.0) * (T + 273.15)) / 0.0065

  // Original barometric formula
//  float alt_f = (44330.0 * (1.0 - powf(P / 101325.0,1 / 5.255))) * 1000;

  // Replace the powf() function with Taylor series expansion at point P = 101325
  // Using WolframAlpha to convert (44330.0 * (1.0 - powf(P / 101325.0,1 / 5.255)) into Taylor series
  // http://www.wolframalpha.com/input/?i=44330*%281+-+%28%28P%2F101325%29^%281%2F5.255%29%29%29+taylor+series+|+P%3D101325
  // The two terms of the series is enough for good result, take three of them to slightly improve precision
  //   -0.0832546 (P-101325)+3.32651×10^-7 (P-101325)^2-1.98043×10^-12 (P-101325)^3
  int32_t p1 = P - 101325; // Substitute for equation
  int32_t p2 = p1 * p1; // (P - 101325)^2
//  int32_t p3 = p1 * p2; // (P - 101325)^3
  // Calculate altitude centered at 'standard' sea level pressure (101325Pa)
//  float alt_t = ((-0.0832546 * p1) + (3.32651E-7 * p2) - (1.98043E-12 * p3)) * 1000.0;
  float alt_t = ((-0.0832546 * p1) + (3.32651E-7 * p2)) * 1000.0;

  // Taylor series with integers only (centered at 'standard' sea level pressure 101325Pa)
  int32_t alt_i = 0; // 0th term of series
  alt_i -= (p1 * 21313) >> 8; // 1th term Q24.8: 0.0832546 * 1000 -> (83 << 8) + (0.2546 * 256) -> 21313
  alt_i += (((p1 * p1) >> 8) * 22) >> 8; // 2nd term Q24.8: 3.32651E-7 * 1000 * 256 -> 22
  // It can be calculated by only shift and add:
  // ((p2 >> 8) * 22) >> 8 --> substitute (pp = p2 >> 8) --> (pp >> 4) + (pp >> 6) + (pp >> 7)
  // alt_i += (p2 >> 12) + (p2 >> 14) + (p2 >> 15); // 2nd term
  // ...but with same code size this will takes 3 more CPU clocks (STM32 hardware multiplier rocks =)

  // TODO: since the sensor operating range is 300 to 1100hPa, it is worth to center the Taylor series
  //       at other pressure value, something near of 90000Pa

/*
  printf("P: %uPa\t",P);
  printf("alt_f: %i.%03im\talt_t: %i.%03im\t",
      (int32_t)alt_f / 1000,
      (int32_t)alt_f % 1000,
      (int32_t)alt_t / 1000,
      (int32_t)alt_t % 1000
    );
  printf("diff_ft: %i.%03im\t",
      (int32_t)(alt_f - alt_t) / 1000,
      (int32_t)(alt_f - alt_t) % 1000
    );
  printf("diff_fi: %i.%03im\r\n",
      (int32_t)(alt_f - alt_i) / 1000,
      (int32_t)(alt_f - alt_i) % 1000
    );
*/

  return (int32_t)alt_t;

  // Calculation accuracy comparison: powf() vs Taylor series approximation
  //   P:      0Pa  alt_f: 44330.000m  alt_t:  8993.567m  diff: 35336.432m
  //   P:  10000Pa  alt_f: 15799.133m  alt_t:  7520.170m  diff:  8278.962m
  //   P:  40000Pa  alt_f:  7186.406m  alt_t:  4927.885m  diff:  2258.521m
  //   P:  50000Pa  alt_f:  5575.208m  alt_t:  3720.608m  diff:  1854.599m
  //   P:  60000Pa  alt_f:  4207.018m  alt_t:  4008.579m  diff:   198.438m
  //   P:  70000Pa  alt_f:  3012.615m  alt_t:  2934.363m  diff:    78.251m
  //   P:  80000Pa  alt_f:  1949.274m  alt_t:  1926.678m  diff:    22.595m
  //   P:  90000Pa  alt_f:   988.646m  alt_t:   985.524m  diff:     3.122m
  //   P: 100000Pa  alt_f:   110.901m  alt_t:   110.892m  diff:     0.009m
  //   P: 110000Pa  alt_f:  -698.421m  alt_t:  -697.199m  diff:    -1.221m
  //   P: 120000Pa  alt_f: -1450.201m  alt_t: -1438.769m  diff:   -11.432m
  // Thus higher/lower the sensor goes from centered value of 101325Pa the more error will give
  // a Taylor series calculation, but for altitudes in range of +/-2km from MSL this is more than enough

  // For altitudes which noticeably differs from the MSL, the Taylor series can be centered at appropriate value:
  // http://www.wolframalpha.com/input/?i=44330*%281+-+%28%28P%2F101325%29^%281%2F5.255%29%29%29+taylor+series+|+P%3DXXXXXX
  // where XXXXXX - pressure at centered altitude
}

void setup() {
  // put your setup code here, to run once:
  Serial.begin(115200);
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
