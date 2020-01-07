
/*

  Connect SCK to Pin 13
  Connect MISO to Pin 12
  Connect MOSI to Pin 11
  Connect SS to Pin 4

*/
extern int write_to_file(const char *filename_to_write_to, const char *str_to_write);

// change this to match your SD shield or module;
// Arduino Ethernet shield: pin 4
// Adafruit SD shields and modules: pin 10
// Sparkfun SD shield: pin 8
const int chipSelect = 4;