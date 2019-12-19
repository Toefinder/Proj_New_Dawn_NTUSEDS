// The __cplusplus thing is required for the library to be compiled through Arduino
#ifdef __cplusplus
extern "C" {
#endif

#ifndef __x86_64
#include "Arduino.h"
#else
#include <stdio.h>
#endif
#include <math.h>
#include <stdint.h>

#ifndef CALC_ALTITUDE
#define CALC_ALTITUDE

#define G 9.80665 // Gravitational acceleration (ms-2)
#define M 0.0289644 // Molar mass of the Earth's air (kgmol-1)
#define R 8.3144598 // Universal gas constant (Jmol-1K-1)
#define EARTH_RADIUS 6371000000 // Earth's radius (m)

struct atmospheric_layer {
	int8_t b; /* atmosphere layer subscript */
	double Alt_b; /* Height above MSL of the atmospheric layer (m) */
	double P_b; /* Standard pressure of the atmospheric layer (Pa) */
	double T_b; /* Standard temperature of the atmospheric layer (K) */
	double L_b; /* Temperature lapse rate in the atmospheric layer (K m-1) */
};

double calc_geopotential_alt(uint32_t, double);
double calc_geometric_alt(double);

#endif

#ifdef __cplusplus
}
#endif
