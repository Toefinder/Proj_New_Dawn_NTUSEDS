#ifdef __x86_64__

#include <stdio.h>
#include "lib/calculate_altitude.h"

extern double calc_geopotential_alt(uint32_t, double);

int main() {
	double pressure;
	printf("Enter pressure (Pa) to calculate altitude (m): ");
	scanf("%lf", &pressure);
	printf("Geopotential altitude: %lfm\n", calc_geopotential_alt(pressure, 273.15));
	
}	

#endif
