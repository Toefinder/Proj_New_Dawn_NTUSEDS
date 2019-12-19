#include "calculate_altitude.h"

#ifdef __x86_64
#define DEBUG
#endif


struct atmospheric_layer atmospheric_layers[7] = {
        {0, 0, 101325, 288.15, -0.0065},
        {1, 11000, 22632, 216.65, 0.0},
        {2, 20000, 5474.89, 216.65, 0.001},
        {3, 32000, 868.02, 228.65, 0.0028},
        {4, 47000, 110.91, 270.65, 0.0},
        {5, 51000, 66.94, 270.65, -0.0028},
        {6, 71000, 3.96, 214.65, -0.002}
};

int8_t b = 0;
double T_b = 288.15;
double Alt_b = 0;
double P_b = 101325;
double L_b = -0.0065;

double calc_geometric_alt (double geopotential_alt) {
	double geometric_alt = (double) EARTH_RADIUS * geopotential_alt / (EARTH_RADIUS - geopotential_alt);
	return geometric_alt;	
}
/*
Takes in Pressure and Temperature at current altitude (labeled as P_alt and T_alt respectively).
P_alt is in Pa, and T_alt is in K.
Returns the calculated geopotential altitude (as it assumes g is constant) using Barometric formula pressure equations. Returns in meters.
Valid up to 278000 ft (86km).
*/

double calc_geopotential_alt (uint32_t P_alt, double T_alt) {
	double geopotential_alt;
#ifdef DEBUG

	printf("calc_geopotential_alt called with the following values.\n\
T_alt = %lf\n\
P_alt = %u\n\
b = %d\n\
T_b = %lf\n\
P_b = %lf\n\
Alt_b = %lf\n\
L_b = %lf\n", 
T_alt, P_alt, (int) b, T_b, P_b, Alt_b, L_b);

#endif
	for(int possible_layer = 6; possible_layer > 0; possible_layer--){
		if ((double) P_alt < atmospheric_layers[possible_layer].P_b){
			b = possible_layer;
			T_b = atmospheric_layers[b].T_b;
			Alt_b = atmospheric_layers[b].Alt_b;
			P_b = atmospheric_layers[b].P_b;
			break;
		}
	}

	if(/* if lapse rate is 0 */ !L_b == 0.0){
		geopotential_alt = log(P_b/P_alt) * R * T_b / M / G + Alt_b; 
	} else {
		geopotential_alt = T_b / L_b * (pow(P_b/P_alt, (double) R * L_b / G / M) - 1 ) + Alt_b;
	}

#ifdef DEBUG
	printf("Returns geopotential alt: %lfm\n", geopotential_alt);
#endif
	return geopotential_alt;
}


