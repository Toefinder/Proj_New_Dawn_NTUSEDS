# Proj_New_Dawn_NTUSEDS

## Install the code
Install the `calculate_altitude` library by copying it to the `Arduino/libraries` directory.

## Testing the library 
Test out the library `calculate_altitude` but running the following commands.
```
cd lib
gcc -c calculate_altitude.c -lm
cd ..
gcc -static test_altitude.c lib/calculate_altitude.o -lm -o test_altitude.elf
```



