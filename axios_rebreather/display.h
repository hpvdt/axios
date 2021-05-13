/*
  Shift Register Connections
  Also the bit order that should be used in display array  
  QA - B
  QB - G
  QC - A
  QD - C
  QE - F
  QF - DP
  QG - E
  QH - D
*/

// Digit 0 is leftmost digit, displays are numbered from bottom to top

// Brightness pin and level
const byte displayEnablePin = 3;
const byte displayBrightness = 127; // 255 is max

// Display related values
const byte numberOfDisplays = 4;
volatile byte currentDigit = 0; // Stores current digit displayed
volatile byte displayArray[numberOfDisplays][4];
// Four bytes (one for each digit) for each display to store which segments should be illuminated


// Stores the digits to display. E.g. charCode[2] is what needs to be output for 2, [12] is 2 with a decimal point
const byte characterCodes[] = {B10111011, B10010000, B11100011, B11110001, B11011000, B01111001, B01111011, B10110000, B11111011, B11111001, B10111111, B10010100, B11100111, B11110101, B11011100, B01111101, B01111111, B10110100, B11111111, B11111101};
const byte dashCharacter = B01000000;

// Functions for display
void displaySetup();
void displayFloat(float dataToDisplay, byte displayIndex, byte decimalDigits);
void displayInt(unsigned long dataToDisplay, byte displayIndex);
void displayDashes(byte displayIndex);
